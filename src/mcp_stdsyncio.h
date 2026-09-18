#include "common_impl.hpp"
#ifndef MCP_STDSYNCIO_H
#define MCP_STDSYNCIO_H
#define __mcp_protocol_version 20251125L

namespace rprmcp {
using namespace std;
using json = nlohmann::json;

class CallToolResult {
public: 
    using this_t = CallToolResult;

private:
    vector<string>    texts_; // normal content
    map<string, json> jsons_; // structured content

public:
    inline CallToolResult() = default;
    inline CallToolResult(const string &text): texts_{{text}} {}
    inline CallToolResult(const string &key, const json &value): jsons_{{key, value}} {}

    inline const auto &texts() const {return texts_;}
    inline const auto &jsons() const {return jsons_;}

    // raw text
    template<typename U> requires convertible_to<string, remove_reference_t<U>>
    inline this_t &add_text(U &&text) {texts_.emplace_back(std::forward<U>(text)); return *this;}

    // structured content
    template<typename U> requires convertible_to<json, remove_reference_t<U>>
    inline this_t &add_json(const string &key, U &&value) {jsons_[key] = std::forward<U>(value); return *this;}
};

template<typename Tp>
Tp get_arg(const json &args, const json &schema, const string &key) {
    if (args.contains(key)) {
        try {return args[key].get<Tp>();}
        catch (const json::exception &e) {throw invalid_argument("Invalid argument ["s + key + "]: " + e.what());}
    }
    try {return schema.at("properties").at(key).at("default").get<Tp>();}
    catch (const json::out_of_range &e) {throw invalid_argument("Missing argument ["s + key + "]: " + e.what());}
    catch (const json::type_error &e)   {throw invalid_argument("Invalid argument ["s + key + "]: " + e.what());}
    catch (const exception &e)          {throw invalid_argument("Invalid argument ["s + key + "]: " + e.what());}
    catch (...)                         {throw invalid_argument("Invalid argument ["s + key + "]: unknown error");}
}
template<typename Tp, typename... Tkeys>
inline auto get_args(const json &args, const json &schema, Tkeys&&... keys) {
    return make_tuple(get_arg<Tp>(args, schema, std::forward<Tkeys>(keys)) ...);
}

struct tool_meta_t {
    string name;
    string description;
    json schema;
};
struct tool_t {
    string description;
    json schema;
    function<CallToolResult(const json&)> handler;
};

inline tool_meta_t make_tool_meta(const string &name, const string &description, const json &schema) {
    if (!schema.contains(name)) throw invalid_argument(name + " is not found in schema");
    return tool_meta_t{name, description, schema[name]};
}

inline namespace jsonrpc {
    json result_response(const json &id, const json &result);
    json error_response(const json &id, int code, const string &msg);
} // namespace jsonrpc

class Server {
public:
    using this_t = Server;
    static inline constexpr auto protocol_version = "2025-11-25";

private:
    const string name_;
    const string version_;
    const string instructions_;
    map<string, tool_t> tools_ = {};

public:
    void run_impl(const string &line);
    json handle_request(const json &req);

public:
    Server(const string &name, const string &version, const string &instructions = ""):
        name_(name),
        version_(version),
        instructions_(instructions)
    {}

    const auto &name()         const noexcept {return name_;}
    const auto &version()      const noexcept {return version_;}
    const auto &instructions() const noexcept {return instructions_;}

    inline void tool(
        const string &name,
        const string &description,
        const json &schema,
        function<CallToolResult(const json&)> handler
    ) {
        tools_[name] = tool_t{description, schema, handler};
    }
    inline void tool(
        const tool_meta_t &meta,
        function<CallToolResult(const json&)> handler
    ) {
        this->tool(meta.name, meta.description, meta.schema, handler);
    }

    inline bool contains(const string &tool_name) const {return tools_.contains(tool_name);}

    json call(const string &tool_name, const json &args);
    inline void run() {
        string line;
        while (getline(cin, line)) {
            if (line.empty()) continue;
            run_impl(line);
        }
    }
    inline void oneshot(const string &tool_name) {
        if (!this->contains(tool_name)) {  
            cerr << "Error: Tool not found.\n";
            return;
        }
        string line;
        getline(cin, line);
        if (line.empty()) {
            cerr << "Error: No input provided for oneshot mode.\n";
            return;
        }
        json result = json::object();
        try {result = this->call(tool_name, json::parse(line));}
        catch (const exception &e) {
            cerr << "Error: " << e.what() << endl;
            return;
        }
        catch (...) {
            cout << "Unknown error" << endl;
            return;
        }
        for (const auto &s : result["content"]) cout << s << "\n";
        cout << "Structured Content: " << result["structuredContent"].dump(2) << "\n";
    }
    inline void info(const string &tool_name) {
        if (!this->contains(tool_name)) {  
            cerr << "Error: Tool not found.\n";
            return;
        }
        cout << tools_[tool_name].description << "\n";
        cout << "Schema: " << tools_[tool_name].schema.dump(2) << "\n";
    }
};

} // namespace rprmcp

#endif