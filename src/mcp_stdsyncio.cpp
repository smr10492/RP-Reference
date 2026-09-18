#include "mcp_stdsyncio.h"

namespace rprmcp {
using std::exception;
using std::flush;

json jsonrpc::result_response(const json &id, const json &result) {
    json ret = json::object();
    ret["jsonrpc"] = "2.0";
    ret["id"]      = id;
    ret["result"]  = result;
    return ret;
}
json jsonrpc::error_response(const json &id, int code, const string &msg) {
    json ret = json::object();
    ret["jsonrpc"]          = "2.0";
    ret["id"]               = id;
    ret["error"]["code"]    = code;
    ret["error"]["message"] = msg;
    return ret;
}

json Server::call(const string &tool_name, const json &args) {
    CallToolResult result = tools_[tool_name].handler(args);
    json ret_content   = json::object();
    json content_array = json::array();
    for (const auto &text : result.texts()) {
        content_array.push_back({
            {"type", "text"},
            {"text", text + "\n"}
        });
    }
    return json::object({
        {"content", content_array},
        {"structuredContent", json(result.jsons())}
    });
}
void Server::run_impl(const string &line) {
    try {
        json req = json::parse(line);
        if (!req.contains("id")) throw invalid_argument("Invalid Request: missing [/id]");
        json resp = handle_request(req);
        cout << resp.dump() << "\n" << flush;
    }
    catch (const exception &e) {
        static const json err = error_response(
            json::object(),
            -32700,
            "Parse error: " + string(e.what())
        );
        cout << err.dump() << endl;
    }
    catch (...) {
        cout << "Unknown error" << endl;
    }
}
json Server::handle_request(const json &req) {
    if (!req.contains("method")) return error_response(
        req.value("id", json::object()), -32600, "Invalid Request: missing [/method]"
    );

    string method = req["method"];
    json id = req.value("id", json::object());

    map<string, json> methods_map = {
        {"tools/list", [&]() {
            json tools_list = json::array();
            for (const auto &[name, entry] : tools_) {
                tools_list.push_back({
                    {"name", name},
                    {"description", entry.description},
                    {"inputSchema", entry.schema}
                });
            }
            return result_response(id, {{"tools", std::move(tools_list)}});
        }()},
#if __mcp_protocol_version < 20260728L
        {"initialize", [&]() {
            auto result = R"({
                "protocolVersion": "",
                "capabilities": {
                    "tools": {}
                },
                "serverInfo": {
                    "name": "",
                    "version": ""
                },
                "instructions": ""
            })"_json;
            result["protocolVersion"]       = this_t::protocol_version;
            result["serverInfo"]["name"]    = name_;
            result["serverInfo"]["version"] = version_;
            result["instructions"]          = instructions_;
            return result_response(id, result);
        }()},
        {"notifications/initialized", json::object()},
#endif
        {"ping", result_response(id, json::object())}
    };

    if (method == "tools/call") {
        if (!req.contains("params") || !req["params"].is_object()) {
            return error_response(id, -32602, "Invalid [/params]");
        }
        const auto &params = req["params"];
        if (!params.contains("name") || !params["name"].is_string()) {
            return error_response(id, -32602, "Invalid [/params/name]");
        }
        string tool_name = params["name"].get<string>();
        auto it = tools_.find(tool_name);
        if (it == tools_.end()) {
            return error_response(id, -32602, "Tool not found");
        }

        if (!params.contains("arguments") || !params["arguments"].is_object()) {
            return error_response(id, -32602, "Invalid [/params/arguments]");
        }
        const auto &args = params["arguments"];
        try {
            auto result = this->call(tool_name, args);
            result["resultType"] = "complete";
            return result_response(id, std::move(result));
        }
        catch (const invalid_argument &e) {
            return error_response(id, -32602, "Invalid params: "s + e.what());
        }
        catch (const exception &e) {
            return error_response(id, -32603, "Tool execution error: "s + e.what());
        }
        catch (...) {
            return error_response(id, -32603, "Unknown error");
        }
    }
    else if (methods_map.contains(method)) return methods_map[method];
    else return error_response(id, -32601, "Method not found");
}

} // namespace rprmcp