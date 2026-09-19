#include "main_rpref_impl.h"
#include "handlers_impl.hpp"
#include "tools_meta.hpp"
#define NAMEOF(var) #var

rprmcp::Server main_reg_server(rprmcp::Server &server) {
#define MCP_TOOL(var) [](const nlohmann::json &var) -> rprmcp::CallToolResult
RPR_USING;
using namespace std;
using namespace rpr;

server.tool(meta::rpi_index, MCP_TOOL(args) {
    ostringstream oss;

    auto rp_dir      = rprmcp::get_arg<string>(args, meta::rpi_index.schema, NAMEOF(rp_dir));
    auto outfile_rpi = rprmcp::get_arg<string>(args, meta::rpi_index.schema, NAMEOF(outfile_rpi));

    if (!fs::exists(rp_dir)) throw runtime_error(NAMEOF(rp_dir) " does not exist: "s + rp_dir);
    RPIndexer rpi(rp_dir);
    rpi.index(nullptr, &oss);

    ofstream fout(outfile_rpi, ios::binary);
    if (!fout.is_open()) throw runtime_error("failed to open " NAMEOF(outfile_rpi) ": "s + outfile_rpi);
    rpi.dump(fout);
    fout.close();

    return rprmcp::CallToolResult(oss.str());
});
server.tool(meta::rpi_find, MCP_TOOL(args) {
    rprmcp::CallToolResult ret = {};

    auto infile_rpi = rprmcp::get_arg<string>        (args, meta::rpi_find.schema, NAMEOF(infile_rpi));
    auto keys       = rprmcp::get_arg<vector<string>>(args, meta::rpi_find.schema, NAMEOF(keys));
    if (!fs::exists(infile_rpi)) throw runtime_error(NAMEOF(infile_rpi) " does not exist: "s + infile_rpi);
    if (keys.empty())            throw invalid_argument("keys is empty");

    RPIndexer rpi;
    rpi.load(infile_rpi);

    for (uint32_t i = 0; i < rpr_base::size; ++i) {
        string compo_result = string(rpr_base::name_of(i)) + ":\n";
        auto fidx = rpi.get(i);
        bool has_val = false;
        for (const auto &key : keys) {
            auto val = fidx[key];
            if (val.empty()) continue;
            compo_result += key + ": " + val + "\n";
            has_val = true;
        }
        if (!has_val) continue;
        ret.add_text(std::move(compo_result));
    }
    return ret;
});
server.tool(meta::ac_to_mmd, MCP_TOOL(args) {
    namespace ns = hndimpl::ac_ref;
    rprmcp::CallToolResult ret = {};
    ostringstream oss;

    auto infile_act  = rprmcp::get_arg<string>(args, meta::ac_to_mmd.schema, NAMEOF(infile_act));
    auto infile_snm  = rprmcp::get_arg<string>(args, meta::ac_to_mmd.schema, NAMEOF(infile_snm));
    auto outfile_mmd = rprmcp::get_arg<string>(args, meta::ac_to_mmd.schema, NAMEOF(outfile_mmd));
    auto mmd_format  = rprmcp::get_arg<string>(args, meta::ac_to_mmd.schema, NAMEOF(mmd_format));

    ifstream fin(infile_act);
    if (!fin.is_open()) throw runtime_error("failed to open " NAMEOF(infile_act) ": "s + infile_act);
    ACtrllers acs; acs.load(fin); fin.close();

    auto dump_fn = &ACtrller::mmd_out<mmd_type::states>;
    bool mmd_available = [&]() -> bool {
        if (mmd_format == "fc") {dump_fn = &ACtrller::mmd_out<mmd_type::flow>;   return true;}
        if (mmd_format == "sd") {dump_fn = &ACtrller::mmd_out<mmd_type::states>; return true;}
        return false;
    }();
    if (!mmd_available) ret.add_text(
        NAMEOF(mmd_format) " is not supported: "s + mmd_format + ". Use default format: sd"
    );

    // mmd output
    ofstream fout(outfile_mmd, ios::binary);
    if (!fout.is_open()) throw runtime_error("failed to open " NAMEOF(outfile_mmd) ": "s + outfile_mmd);
    for (auto &[k, v] : acs) {
        mmd::comment(fout, k);
        (v.*dump_fn)(fout, true);
    }
    if (!fout.is_open()) fout.close();
    ret.add_text("mmd output success"s);

    // nested call chains
    if (infile_snm.empty()) return ret;
    fin.open(infile_snm);
    if (!fin.is_open()) {
        ret.add_text("failed to open " NAMEOF(infile_snm) ": "s + infile_snm);
        return ret;
    }
    ns::get_nested_act(fin, acs, oss); fin.close();
    ret.add_text(oss.str());
    oss.str(""); oss.clear();
    return ret;
});
server.tool(meta::sn_map, MCP_TOOL(args) {
    namespace ns = hndimpl::sn_map;
    ostringstream oss;

    //auto infile_ent  = rprmcp::get_arg<string>(args, meta::sn_map.schema, NAMEOF(infile_ent));
    //auto infile_rpi  = rprmcp::get_arg<string>(args, meta::sn_map.schema, NAMEOF(infile_rpi));
    //auto outfile_snm = rprmcp::get_arg<string>(args, meta::sn_map.schema, NAMEOF(outfile_snm));
    auto [infile_ent, infile_rpi, outfile_snm] = rprmcp::get_args<string>(
        args, meta::sn_map.schema,
        NAMEOF(infile_ent),
        NAMEOF(infile_rpi),
        NAMEOF(outfile_snm)
    );

    ifstream fin(infile_ent);
    ofstream fout(outfile_snm, ios::binary);
    MsgCollect error_collect;
    if (!fin .is_open()) error_collect.add("failed to open " NAMEOF(infile_ent)  ": "s + infile_ent);
    if (!fout.is_open()) error_collect.add("failed to open " NAMEOF(outfile_snm) ": "s + outfile_snm);
    if (!error_collect.empty()) throw runtime_error(error_collect.string());
    json obj = json::parse(fin, nullptr, true, true);
    if (!obj.is_object()) throw runtime_error(NAMEOF(infile_ent) " is not an object");
    Entity ent(obj);
    fin.close();

    fout << ns::index_entity(ent, infile_rpi, oss); fout.close();
    return rprmcp::CallToolResult(oss.str());
});
server.tool(meta::bone_coverage, MCP_TOOL(args) {
    namespace ns = hndimpl::bone_coverage;
    rprmcp::CallToolResult ret = {};

    auto id_geo     = rprmcp::get_arg<string>(args, meta::bone_coverage.schema, NAMEOF(id_geo));
    auto id_ani     = rprmcp::get_arg<string>(args, meta::bone_coverage.schema, NAMEOF(id_ani));
    auto infile_rpi = rprmcp::get_arg<string>(args, meta::bone_coverage.schema, NAMEOF(infile_rpi));
    if (!fs::exists(infile_rpi)) throw invalid_argument(NAMEOF(infile_rpi) " does not exist: "s + infile_rpi);

    RPIndexer rpi;
    rpi.load(infile_rpi);
    const auto &[geo_only, ani_only, both] = ns::sets_partition(
        rpi.load_from<Geometries>(rpr_base::geo, id_geo)[id_geo].bone_names(),
        rpi.load_from<Animations>(rpr_base::ani, id_ani)[id_ani].bone_names()
    );

    ret.add_json(NAMEOF(geo_only), geo_only);
    ret.add_json(NAMEOF(ani_only), ani_only);
    ret.add_json(NAMEOF(both),     both);
    return ret;
});
server.tool(meta::geo_tree, MCP_TOOL(args) {
    ostringstream oss;

    auto id_geo     = rprmcp::get_arg<string>(args, meta::geo_tree.schema, NAMEOF(id_geo));
    auto infile_rpi = rprmcp::get_arg<string>(args, meta::geo_tree.schema, NAMEOF(infile_rpi));
    if (!fs::exists(infile_rpi)) throw invalid_argument("infile_rpi: "s + infile_rpi + " is not found");

    RPIndexer rpi;
    rpi.load(infile_rpi);
    const auto geos = rpi.load_from<Geometries>(rpr_base::geo, id_geo);
    geos.at(id_geo).tree_out(oss);

    return rprmcp::CallToolResult(oss.str());
});
server.tool(meta::br_extract, MCP_TOOL(args) {
    namespace ns = hndimpl::br_proc;
    rprmcp::CallToolResult ret = {};

    auto infiles_br = rprmcp::get_arg<vector<string>>(args, meta::br_extract.schema, NAMEOF(infiles_br));
    if (infiles_br.empty()) throw invalid_argument(NAMEOF(infiles_br) " is empty"s);

    MsgCollect error_collect;
    map<string, uint32_t> count_map;
    for (auto &infile_br : infiles_br) {
        auto dst_dir = fs::path(infile_br).replace_extension("");
        fs::create_directories(dst_dir);
        count_map[infile_br] = ns::br_extract_impl(infile_br, dst_dir, error_collect);
    }
    if (!error_collect.empty()) throw runtime_error(error_collect.string());

    ret.add_text("extract archives: "s + to_string(count_map.size()));
    ret.add_json("handled files: ", count_map);
    return ret;
});
server.tool(meta::br_zip, MCP_TOOL(args) {
    namespace ns = hndimpl::br_proc;
    MsgCollect error_collect;

    auto infiles_br = rprmcp::get_arg<vector<string>>(args, meta::br_zip.schema, NAMEOF(infiles_br));
    auto with_data  = rprmcp::get_arg<bool>          (args, meta::br_zip.schema, NAMEOF(with_data));
    if (infiles_br.empty()) throw invalid_argument(NAMEOF(infiles_br) " is empty"s);

    // folder/file.json -> folder/folder.brarchive
    // if the path has no directory component (e.g. "file.json"), resolve it to an
    // absolute path so that the containing folder name can always be captured
    fs::path outfile = fs::path(infiles_br[0]).lexically_normal();
    if (outfile.parent_path().empty()) outfile = fs::absolute(outfile);
    outfile = outfile.parent_path() / outfile.parent_path().filename();
    outfile.replace_extension(br_base::extension);

    uint32_t count = ns::br_zip_impl(infiles_br, outfile.generic_string(), error_collect, with_data);
    if (!error_collect.empty()) throw runtime_error(error_collect.string());

    return rprmcp::CallToolResult("archived_count", count);
});
server.tool(meta::br_merge, MCP_TOOL(args) {
    namespace ns = hndimpl::br_proc;
    MsgCollect error_collect;

    auto infiles_br = rprmcp::get_arg<vector<string>>(args, meta::br_merge.schema, NAMEOF(infiles_br));
    auto outfile_br = rprmcp::get_arg<string>(args, meta::br_merge.schema, NAMEOF(outfile_br));
    if (infiles_br.empty()) throw invalid_argument("lost " NAMEOF(infiles_br) ": "s + infiles_br[0]);

    uint32_t count = ns::br_merge_impl(infiles_br, outfile_br, error_collect);
    if (!error_collect.empty()) throw runtime_error(error_collect.string());

    return rprmcp::CallToolResult("merged_count", count);
});
server.tool(meta::br_auto, MCP_TOOL(args) {
    namespace ns = hndimpl::br_proc;
    MsgCollect error_collect;

    auto command = rprmcp::get_arg<string>(args, meta::br_auto.schema, NAMEOF(command));
    auto rp_dir  = rprmcp::get_arg<string>(args, meta::br_auto.schema, NAMEOF(rp_dir));
    if (!fs::exists(rp_dir)) throw invalid_argument(NAMEOF(rp_dir) " does not exist: "s + rp_dir);

    auto count_map = ns::rp_auto_impl(command, rp_dir, error_collect);
    if (!error_collect.empty()) throw runtime_error(error_collect.string());

    map<string, uint32_t> handled_files;
    for (auto &[k, v] : count_map) handled_files[k.generic_string()] = v;
    return rprmcp::CallToolResult(
        "command_"s + std::move(command) + "_handled_count",
        std::move(handled_files)
    );
});
server.tool(meta::dir_compo_rename, MCP_TOOL(args) {
    namespace ns = hndimpl::compo_rename;
    using match_t = ns::match_t;
    rprmcp::CallToolResult ret    = {};
    vector<string> failed_opens   = {};
    vector<string> failed_parses  = {};
    vector<string> not_contains   = {};
    vector<string> failed_renames = {};
    uint32_t success_count = 0;
    error_code ec;

    auto compo_dir     = rprmcp::get_arg<fs::path>(args, meta::dir_compo_rename.schema, NAMEOF(compo_dir));
    auto match_type    = rprmcp::get_arg<string>  (args, meta::dir_compo_rename.schema, NAMEOF(match_type));
    auto json_ptr_str  = rprmcp::get_arg<string>  (args, meta::dir_compo_rename.schema, "json_pointer");
    auto ignore_prefix = rprmcp::get_arg<string>  (args, meta::dir_compo_rename.schema, NAMEOF(ignore_prefix));
    auto output_format = rprmcp::get_arg<string>  (args, meta::dir_compo_rename.schema, NAMEOF(output_format));
    if (!fs::exists(compo_dir)) throw invalid_argument(
        NAMEOF(compo_dir) " does not exist: "s + compo_dir.generic_string()
    );

    match_t mt = match_t::key;
    if      (match_type == "key")   mt = match_t::key;
    else if (match_type == "value") mt = match_t::value;
    else throw invalid_argument("invalid " NAMEOF(match_type) ": "s + match_type);

    json::json_pointer json_ptr(json_ptr_str);
    for (const auto &entry : fs::directory_iterator(compo_dir)) {
        ifstream fin(entry.path());
        if (!fin.is_open()) {
            failed_opens.emplace_back(entry.path().generic_string());
            continue;
        }

        json obj = json::parse(fin, nullptr, false, true);
        if (obj.is_discarded()) {
            failed_parses.emplace_back(entry.path().generic_string());
            continue;
        }
        string new_name;
        if (mt == match_t::key) {
            if (!obj.contains(json_ptr)) {
                not_contains.emplace_back(entry.path().generic_string());
                fin.close();
                continue;
            }
            const auto kv = obj.at(json_ptr);
            if (kv.is_object() && !kv.empty()) new_name = kv.begin().key();
        }
        else if (mt == match_t::value) {
            new_name = obj.value(json_ptr, "");
        }
        fin.close();

        if (new_name.empty()) continue;
        ns::remove_ignored(new_name, ignore_prefix);
        ns::legalize(new_name, "/?*\\:\'\"><|", '_');
        new_name = compo_dir.generic_string() + "/" + std::vformat(output_format, std::make_format_args(new_name));
        new_name += ".json";

        fs::rename(entry.path(), new_name, ec);
        if (ec) {
            failed_renames.emplace_back(entry.path().generic_string());
            ec.clear();
            continue;
        }
        ++success_count;
    }
    ret.add_text("renamed " + to_string(success_count) + " files");
    if (!failed_opens  .empty()) ret.add_json("failed_opens",   std::move(failed_opens));
    if (!failed_parses .empty()) ret.add_json("failed_parses",  std::move(failed_parses));
    if (!not_contains  .empty()) ret.add_json("not_contains",   std::move(not_contains));
    if (!failed_renames.empty()) ret.add_json("failed_renames", std::move(failed_renames));
    return ret;
});
server.tool(meta::rpi_compo_rename, MCP_TOOL(args) {
    rprmcp::CallToolResult ret    = {};
    vector<string> failed_renames = {};
    uint32_t success_count = 0;
    error_code ec;

    auto infile_rpi    = rprmcp::get_arg<string>(args, meta::rpi_compo_rename.schema, NAMEOF(infile_rpi));
    auto ignore_prefix = rprmcp::get_arg<string>(args, meta::rpi_compo_rename.schema, NAMEOF(ignore_prefix));
    auto output_format = rprmcp::get_arg<string>(args, meta::rpi_compo_rename.schema, NAMEOF(output_format));
    if (!fs::exists(infile_rpi)) throw invalid_argument("infile_rpi: "s + infile_rpi + " is not found");

    RPIndexer rpi;
    rpi.load(infile_rpi);
    for (uint32_t c = 0; c < rpr_base.size; ++c) {
        auto compo_dir = rpi.rp_dir() + "/" + rpr_base.folder_of(c);
        FileIndexer idx = rpi.get(c);
        vector<uint32_t> accessed; accessed.reserve(idx.files().size());
        for (const auto &[new_name, i] : idx) {
            if (find(accessed.begin(), accessed.end(), i) != accessed.end()) continue;
            accessed.emplace_back(i);
            fs::rename(idx.files().at(i), compo_dir + "/" + new_name, ec);
            if (ec) {
                failed_renames.emplace_back(idx.files().at(i));
                ec.clear();
                continue;
            }
        }
        ++success_count;
    }
    ret.add_text("renamed " + to_string(success_count) + " files");
    if (!failed_renames.empty()) ret.add_json("failed_renames", std::move(failed_renames));
    return ret;
});
server.tool(meta::unmerge, MCP_TOOL(args) {
    namespace ns = hndimpl::compo_merge;
    rprmcp::CallToolResult ret = {};
    map<string, uint32_t> unmerged_counts = {};
    vector<string> failed_opens  = {};
    vector<string> failed_parses = {};
    vector<string> not_contains  = {};
    error_code ec;

    auto infiles      = rprmcp::get_arg<vector<string>>(args, meta::unmerge.schema, NAMEOF(infiles));
    auto json_ptr_str = rprmcp::get_arg<string>        (args, meta::unmerge.schema, "json_pointer");
    json::json_pointer json_ptr(json_ptr_str);

    for (const auto &infile : infiles) {
        ifstream fin(infile);
        if (!fin.is_open()) {
            failed_opens.emplace_back(infile);
            continue;
        }

        json obj = json::parse(fin, nullptr, false, true);
        if (obj.is_discarded()) {
            failed_parses.emplace_back(infile);
            continue;
        }
        vector<pair<string, json>> objs = {};
        auto file_name = fs::path(infile).replace_extension("").generic_string();
        try {objs = ns::unmerge_impl(file_name, obj, json_ptr);}
        catch (const ns::not_object_or_array&) {
            not_contains.emplace_back(infile);
            continue;
        }
        unmerged_counts[infile] = 0;
        for (const auto &[k, v] : objs) {
            string new_name(k);
            ns::legalize(new_name, "/?*\\:\'\"><|", '_');
            ofstream fout(new_name + ".json");
            if (!fout.is_open()) {
                failed_opens.emplace_back(new_name);
                continue;
            }
            fout << v.dump();
            fout.close();
            ++unmerged_counts[infile];
        }
    }

    ret.add_json("unmerged_counts", std::move(unmerged_counts));
    if (!failed_opens .empty()) ret.add_json("failed_opens",  std::move(failed_opens));
    if (!failed_parses.empty()) ret.add_json("failed_parses", std::move(failed_parses));
    if (!not_contains .empty()) ret.add_json("not_contains",  std::move(not_contains));
    return ret;
});
server.tool(meta::merged, MCP_TOOL(args) {
    namespace ns = hndimpl::compo_merge;
    rprmcp::CallToolResult ret = {};

    auto infiles      = rprmcp::get_arg<vector<string>>(args, meta::merged.schema, NAMEOF(infiles));
    auto json_ptr_str = rprmcp::get_arg<string>        (args, meta::merged.schema, "json_pointer");
    if (infiles.empty()) throw invalid_argument(NAMEOF(infiles) " is empty"s);
    json::json_pointer json_ptr(json_ptr_str);

    auto [
        merged,
        failed_opens, failed_parses, not_contains,
        merged_count
    ] = ns::merged_impl(infiles, json_ptr);

    ret.add_json("merged_count", std::move(merged_count));
    if (!failed_opens .empty()) ret.add_json("failed_opens",  std::move(failed_opens));
    if (!failed_parses.empty()) ret.add_json("failed_parses", std::move(failed_parses));
    if (!not_contains .empty()) ret.add_json("not_contains",  std::move(not_contains));
    return ret;
});
server.tool(meta::mat_forest, MCP_TOOL(args) {
    auto infile = rprmcp::get_arg<string>(args, meta::mat_forest.schema, NAMEOF(infile));
    ifstream fin(infile);
    if (!fin.is_open()) throw runtime_error("failed to open " NAMEOF(infile) ": "s + infile);
    json obj = json::parse(fin, nullptr, true, true);
    if (!obj.is_object()) throw invalid_argument(NAMEOF(infile) " is not a json object");
    Material mat(obj);
    return rprmcp::CallToolResult("mat_forest", mat.forest());
});

return server;
#undef MCP_TOOL
} // rprmcp::Server main_reg_server(rprmcp::Server &server)

#undef NAMEOF