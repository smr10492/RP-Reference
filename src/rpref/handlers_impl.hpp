#ifndef HANDLERS_IMPL_HPP
#define HANDLERS_IMPL_HPP
#include "main_rpref_impl.h"
#include "msg_collect.hpp"

namespace hndimpl {
RPR_DECL;
using namespace std;

namespace sn_map {
nlohmann::json index_entity(rpr::Entity &ent, const string &rpi_file, ostream &oss) {
    rpr::RPIndexer rpi;
    json ret = json::object();

    if (rpi_file.empty())    throw invalid_argument("rpi_file is empty");
    if (!rpi.load(rpi_file)) throw runtime_error("failed to load rpi file"s + rpi_file);

    // textures' values are already file paths
if (true) {
    oss << "indexing textures...\n";
    ret[rpr_base[rpr_base::tex]] = ent.tex();
    oss << "- used keys: " << ent.tex().size() << "\n";
}

if (true) {
    oss << "indexing materials...\n";
    ret[rpr_base[rpr_base::mat]] = json::object();
    rpr::FileIndexer mat_index = rpi.get(rpr_base::mat);

    oss << "- checked files: " << mat_index.files().size() << "\n";
    oss << "- checked keys:  " << mat_index.size() << "\n";
    oss << "- used keys:     " << ent.mat().size() << "\n";

    for (auto &[k, v] : ent.mat()) ret[rpr_base[rpr_base::mat]][k] = json::array({v, mat_index[v]});
}

if (true) {
    oss << "indexing models...\n";
    ret[rpr_base[rpr_base::geo]] = json::object();
    rpr::FileIndexer geo_index = rpi.get(rpr_base::geo);

    oss << "- checked files: " << geo_index.files().size() << "\n";
    oss << "- checked keys:  " << geo_index.size() << "\n";
    oss << "- used keys:     " << ent.geo().size() << "\n";

    for (auto &[k, v] : ent.geo()) ret[rpr_base[rpr_base::geo]][k] = json::array({v, geo_index[v]});
}

if (true) {
    oss << "indexing animations and a-controllers:\n";
    ret[rpr_base[rpr_base::ani]] = json::object();
    ret[rpr_base[rpr_base::act]] = json::object();

    oss << "- animations...\n";
    rpr::FileIndexer ani_index = rpi.get(rpr_base::ani);

    oss << "    - checked files: " << ani_index.files().size() << "\n";
    oss << "    - checked keys:  " << ani_index.size() << "\n";

    oss << "- a-controllers...\n";
    rpr::FileIndexer act_index = rpi.get(rpr_base::act);

    oss << "    - checked files: " << act_index.files().size() << "\n";
    oss << "    - checked keys:  " << act_index.size() << "\n";

    oss << "- used keys:       " << ent.anc().size() << "\n";

    for (auto &[k, v] : ent.anc()) {
        // a-controller prio
        if (!act_index[v].empty()) ret[rpr_base[rpr_base::act]][k] = json::array({
            // ani might still be present
            v, ani_index[v], act_index[v]
        });
        else ret[rpr_base[rpr_base::ani]][k] = json::array({
            v, ani_index[v]
        });
    }
}

    // render-controller declarations are stored with objects in an array, not KV pairs
    // but only the first element is used
if (true) {
    oss << "indexing render-controllers...\n";
    rpr::FileIndexer rct_index = rpi.get(rpr_base::rct);

    oss << "- checked files: " << rct_index.files().size() << "\n";
    oss << "- checked keys:  " << rct_index.size() << "\n";
    oss << "- used keys:     " << ent.rct().size() << "\n";

    for (auto &el : ent.rct()) {
        if (el.empty()) continue;
        string k = "";
        if      (el.is_object()) k = el.begin().key();
        else if (el.is_string()) k = el.get<string>();
        else continue;
        ret[rpr_base[rpr_base::rct]][k] = rct_index[k];
    }
}

    return ret;
}
} // namespace sn_map

namespace ac_ref {
void get_nested_act(ifstream &fin, rpr::ACtrllers &acs, ostream &oss) {
    json snm = json::parse(fin, nullptr, false, true);
    constexpr auto act_cstr = rpr_base[rpr_base::act];
    if (snm.is_discarded())         throw runtime_error("the short-name map is a discarded JSON object\n");
    if (!snm.is_object())           throw runtime_error("Short-name map file must be an object\n");
    if (!snm.contains(act_cstr))    throw runtime_error("Short-name map file must index \"act\" component\n");
    if (!snm[act_cstr].is_object()) throw runtime_error("The \"act\" component must be an object\n");
    json &snm_act = snm[act_cstr];
    if (snm_act.empty()) {oss << "a-controllers map is empty\n"; return;}

    oss << "a-controllers nested call chains:\n";
    for (auto &[k, v] : acs) {
        oss << k << ":";
        bool has_act = false;

        for (auto &[k, state] : v) // single state KV
        if (state.contains("animations") && state["animations"].is_array())
        for (auto s_json : state["animations"]) {
            // visit each animation
            if (!s_json.is_string()) continue;
            string s = s_json.get<string>();
            if (s.empty()) continue;
            if (!snm_act.contains(s) || !snm_act[s].is_array()) continue;

            // get act_fp from type [id: string, ani_fp: string, act_fp: string]
            string act_fp = "";
            if (!snm_act[s][2].is_string()) continue;
            act_fp = snm_act[s][2].get<string>();
            if (act_fp.empty()) continue;
            oss << "\n| " << k << ": " << s << " -> " << act_fp;
            has_act = true;
        }
        oss << ((has_act)? "\n" : " (none)\n");
    }
}
} // namespace ac_ref

namespace bone_coverage {
template<typename Tp>
tuple<vector<Tp>, vector<Tp>, vector<Tp>> sets_partition(const set<Tp> &a, const set<Tp> &b) {
    vector<Tp> a_only, b_only, both;
    a_only.reserve(a.size());
    b_only.reserve(b.size());
    both.reserve(min(a.size(), b.size()));

    auto it_a = a.begin();
    auto it_b = b.begin();

    // both
    while (it_a != a.end() && it_b != b.end()) {
        if      (*it_a < *it_b) {a_only.push_back(*it_a); ++it_a;}
        else if (*it_b < *it_a) {b_only.push_back(*it_b); ++it_b;}
        else {both.push_back(*it_a); ++it_a; ++it_b;}
    }

    // only
    while (it_a != a.end()) {a_only.push_back(*it_a); ++it_a;}
    while (it_b != b.end()) {b_only.push_back(*it_b); ++it_b;}

    return {a_only, b_only, both};
}

} // namespace bone_coverage

namespace br_proc {
using rpr::BrArch;

/// @brief extract brarchive file to a directory \
/// @brief (no overwrite, skip empty files)
/// @retval number of files extracted
uint32_t br_extract_impl(const string &infile, const fs::path &dst_dir, MsgCollect &error_collect) {
    BrArch br;
    if (!br.load(infile)) {
        error_collect.add("failed to load brarchive file: "s + infile);
        return 0;
    }
    if (br.empty()) return 0;

    uint32_t extract_count = 0;
    for (const auto &elem : br) {
        if (elem.name.empty()) continue;
        auto dst_path = dst_dir / elem.name;
        if (fs::exists(dst_path)) continue;
        ofstream fout(dst_path, ios::binary);
        if (!fout.is_open()) {
            error_collect.add("failed to open outfile: "s + dst_path.generic_string());
            continue;
        }
        fout.write(reinterpret_cast<const char*>(elem.data.data()), elem.data.size());
        fout.close();
        ++extract_count;
    }
    error_code ec;
    if (fs::is_empty(dst_dir, ec)) fs::remove(dst_dir, ec);

    return extract_count;
}

/// @brief archive files to a brarchive file
/// @retval number of files archived
uint32_t br_zip_impl(
    const vector<string> &infiles,
    const string &outfile,
    MsgCollect &error_collect,
    bool with_data = true
) {
    using byte_t = BrArch::byte_t;
    uint32_t ret = 0;
    BrArch br;
    ofstream fout(outfile, ios::binary);
    if (!fout.is_open()) throw runtime_error("failed to open outfile: "s + outfile);

    for (const auto &file : infiles) {
        ifstream fin(file, ios::binary);
        if (!fin.is_open()) {
            error_collect.add("failed to open infile: "s + file);
            continue;
        }
        vector<byte_t> data = {};
        if (with_data) data = vector<byte_t>(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
        br << BrArch::elem_t{file, data};
        fin.close();
        ++ret;
    }

    br.dump(fout);
    fout.close();
    return ret;
}

/// @brief merge all *.brarchive files to one
/// @retval number of files merged
uint32_t br_merge_impl(const vector<string> &infiles, const string &outfile, MsgCollect &error_collect) {
    uint32_t ret = 0;
    BrArch br;
    ofstream fout(outfile, ios::binary);
    if (!fout.is_open()) throw runtime_error("failed to open outfile: "s + outfile);

    for (const auto &file : infiles) {
        BrArch br_in;
        if (!br_in.load(file)) {
            error_collect.add("failed to load infile: "s + file);
            continue;
        }
        br << br_in;
        ++ret;
    }

    br.dump(fout);
    fout.close();
    return ret;
}

/// @brief save an empty brarchive file to a directory
/// @retval 0
/// @exception runtime_error if failed to open outfile
uint32_t save_empty_brarchive(const string &outfile) {
    BrArch br;
    ofstream fout(outfile, ios::binary);
    if (!fout.is_open()) throw runtime_error("failed to open outfile: "s + outfile);
    br.dump(fout);
    fout.close();
    return 0;
}

/// @brief automatic br_extract_impl()
/// @retval map (directory -> number of files extracted)
map<fs::path, uint32_t> rp_extract_impl(const fs::path &rp_dir, MsgCollect &error_collect) {
    map<fs::path, uint32_t> ret = {};
    error_code ec;
    MsgCollect sub_error_collect;
    const auto &skip_pd = fs::directory_options::skip_permission_denied;

    if (!fs::exists(rp_dir / br_base::folder)) throw runtime_error(
        "failed to find "s + br_base::folder + " in "s + rp_dir.generic_string()
    );
    for (const auto &entry : fs::recursive_directory_iterator(rp_dir / br_base::folder, skip_pd, ec)) {
        if (!entry.is_regular_file(ec)) continue;
        if (entry.path().extension() != br_base::extension) continue;
        auto &infile = entry.path();
        auto tmp = entry.path();

        // ./__brarchive/rp/__brarchive/folder.brarchive -> ./__brarchive/rp/folder
        // remove the last "__brarchive" part
        fs::path dst_dir = ".";
        vector<fs::path> parts; parts.reserve(3);
        for (const auto &p : tmp) parts.emplace_back(p);
        auto it = std::find_if(
            parts.rbegin(), parts.rend(),
            [](const fs::path& part) {return part == br_base::folder;}
        );
        if (it != parts.rend()) parts.erase((it + 1).base());
        if (!parts.empty()) dst_dir = parts[0];
        for (size_t i = 1; i < parts.size(); ++i) dst_dir /= parts[i];
        dst_dir.replace_extension("");

        fs::create_directories(dst_dir);
        ret[dst_dir] = br_extract_impl(infile.generic_string(), dst_dir, sub_error_collect);
        if (!sub_error_collect.empty()) error_collect.add(
            "in " + dst_dir.generic_string() + ": " + sub_error_collect.string()
        );
    }
    return ret;
}

/// @brief divide files in a directory into text(JSON) files and other files
/// @retval pair (text files, other files)
pair<vector<string>, vector<string>> txt_oth_divide(const fs::path &compo_dir) {
    const auto &skip_pd = fs::directory_options::skip_permission_denied;
    error_code ec;
    vector<string> txt_files, oth_files;
    for (const auto &sub_entry : fs::directory_iterator(compo_dir, skip_pd, ec)) {
        if (!sub_entry.is_regular_file(ec)) continue;
        const auto extension = sub_entry.path().extension();
        bool is_txt = (extension == ".json");
        (is_txt? txt_files : oth_files).emplace_back(sub_entry.path().generic_string());
    }
    return {txt_files, oth_files};
}

/// @brief automatic br_zip_impl()
/// @retval map (directory -> number of files archived)
map<fs::path, uint32_t> rp_zip_impl(const fs::path &rp_dir, MsgCollect &error_collect) {
    map<fs::path, uint32_t> ret = {};
    error_code ec;
    fs::path compo_dir;
    const auto &skip_pd = fs::directory_options::skip_permission_denied;
    const fs::path br_dir = rp_dir / br_base::folder;
    fs::create_directories(br_dir);

    auto _handle_common = [&](const fs::path &compo_dir, bool with_data = true) -> void {
        vector<string> files;
        for (const auto &entry : fs::directory_iterator(compo_dir, skip_pd, ec)) {
            if (!entry.is_regular_file(ec)) continue;
            files.emplace_back(entry.path().generic_string());
        }
        // ./folder -> ./folder.brarchive
        auto outfile = br_dir / fs::relative(compo_dir, rp_dir, ec);
        fs::create_directories(outfile.parent_path());
        outfile.replace_extension(br_base::extension);
        fs::path compo_rel_dir = fs::relative(compo_dir, rp_dir, ec);
        ret[compo_rel_dir] = br_proc::br_zip_impl(files, outfile.generic_string(), error_collect, with_data);
    };

    // special cases
    auto _handle_mat = [&](const fs::path &mat_dir) -> void {
        // TODO: consider adding recursive_directory_iterator
        fs::path br_name = (br_dir / rpr_base::folder_of(rpr_base::mat)).replace_extension(br_base::extension);
        vector<string> mat_files;
        for (const auto &entry : fs::directory_iterator(mat_dir, skip_pd, ec)) {
            if (!entry.is_regular_file(ec)) continue;
            if (entry.path().extension() != ".material") continue;
            mat_files.emplace_back(entry.path().generic_string());
        }
        fs::path mat_rel_dir = fs::relative(mat_dir, rp_dir, ec);
        if (mat_files.empty()) ret[mat_rel_dir] = save_empty_brarchive(br_name.generic_string());
        else ret[mat_rel_dir] = br_zip_impl(mat_files, br_name.generic_string(), error_collect, false);
    };

    auto _handle_snd = [&](const fs::path &snd_dir) -> void {
        // archive JSONs in snd_dir root
        // ignore sounds files
        vector<string> txt_files, oth_files;
        tie(txt_files, oth_files) = txt_oth_divide(snd_dir);
        auto outfile = br_dir / fs::relative(snd_dir, rp_dir, ec);
        fs::create_directories(outfile.parent_path());
        outfile.replace_extension(br_base::extension);
        ret[fs::relative(snd_dir, rp_dir, ec)] = br_zip_impl(txt_files, outfile.generic_string(), error_collect);

        for (const auto &entry : fs::recursive_directory_iterator(snd_dir, skip_pd, ec)) {
            if (!entry.is_directory(ec)) continue;
            _handle_common(entry.path(), false);
        }
    };

    auto _handle_txt = [&](const fs::path &txt_dir) -> void {
        // TODO: consider adding recursive_directory_iterator
        _handle_common(txt_dir, false);
        for (const auto &entry : fs::recursive_directory_iterator(txt_dir, skip_pd, ec)) {
            if (!entry.is_directory(ec)) continue;
            _handle_common(entry.path(), false);
        }
    };

    auto _handle_tex = [&](const fs::path &tex_dir) -> void {
        // skip all files in tex_dir root
        vector<string> txt_files, oth_files;
        for (const auto &entry : fs::recursive_directory_iterator(tex_dir, skip_pd, ec)) {
            if (!entry.is_directory(ec)) continue;
            tie(txt_files, oth_files) = txt_oth_divide(entry.path());

            fs::path parent_dir = br_dir / fs::relative(entry.path(), rp_dir, ec);
            fs::create_directories(parent_dir.parent_path());

            // ./folder -> ./folder.brarchive
            string outfile_merged = parent_dir.replace_extension(br_base::extension).generic_string();
            string outfile_txt    = outfile_merged + "__txt";
            string outfile_oth    = outfile_merged + "__oth";

            // archive jsons & index other files
            // remove rp_dir root from tex_dir
            fs::path it_rel_dir = fs::relative(entry.path(), rp_dir, ec);
            ret[it_rel_dir] = 0;
            ret[it_rel_dir] += br_proc::br_zip_impl(txt_files, outfile_txt, error_collect, true);
            ret[it_rel_dir] += br_proc::br_zip_impl(oth_files, outfile_oth, error_collect, false);
            br_proc::br_merge_impl({outfile_txt, outfile_oth}, outfile_merged, error_collect);

            fs::remove(outfile_txt, ec);
            fs::remove(outfile_oth, ec);
        }
    };

    map<fs::path, function<void(const fs::path&)>> handlers = {
        {rp_dir / rpr_base::folder_of(rpr_base::mat), _handle_mat},
        {rp_dir / rpr_base::folder_of(rpr_base::snd), _handle_snd},
        {rp_dir / "texts",                            _handle_txt}, // TODO: support rpr_base::folder_of(rpr_base::txt)
        {rp_dir / rpr_base::folder_of(rpr_base::tex), _handle_tex}
    };

    for (
        auto it = fs::recursive_directory_iterator(rp_dir, skip_pd, ec);
        it != fs::recursive_directory_iterator();
        ++it
    ) {
        const auto &entry = *it;
        if (!entry.is_directory(ec)) continue;
        compo_dir = entry.path();
        if (handlers.contains(compo_dir)) {
            handlers.at(compo_dir)(compo_dir);
            it.disable_recursion_pending();
            continue;
        }
        else if (compo_dir == rp_dir / br_base::folder) {
            // avoid recursive iteration
            it.disable_recursion_pending();
            continue;
        }
        else _handle_common(compo_dir);
    }

    return ret;
}

/// @brief automatically process brarchive files
/// @retval map (directory -> number of files processed)
map<fs::path, uint32_t> rp_auto_impl(const string &command, const fs::path &rp_dir, MsgCollect &error_collect) {
    static const map<string, decltype(&rp_extract_impl)> impls = {
        {"extract", rp_extract_impl},
        {"zip",     rp_zip_impl    }
    };
    if (!impls.contains(command)) throw invalid_argument("invalid command: "s + command);
    return impls.at(command)(rp_dir, error_collect);
}

} // namespace br_proc

namespace compo_rename {
enum class match_t {key, value};
inline void legalize(string &s, const string &match, char replace = '_') {
    if (s.empty()) return;

    for (auto &c : s) if (iscntrl(c) || (match.find(c) != string::npos)) c = replace;
    auto s_b = s.find_first_not_of(" \t\n\r\f\v");
    auto s_e = s.find_last_not_of (" \t\n\r\f\v");
    if (s_b == string::npos || s_e == string::npos) {
        s.clear();
        return;
    }
    s = s.substr(s_b, s_e - s_b + 1);
}
inline void remove_ignored(string &s, const string &prefix) {
    if (prefix.empty()) return;
    auto pos = s.find(prefix);
    if (pos != string::npos) s.erase(pos, prefix.length());
}
} // namespace compo_rename

namespace compo_merge {
class not_object_or_array {}; // exception
using compo_rename::legalize;
vector<pair<string, json>> unmerge_impl(const string &file_name, const json &obj, const json::json_pointer &json_ptr) {
    vector<pair<string, json>> ret;
    const json &sub = obj[json_ptr];

    if (sub.is_object()) {
        for (const auto &[k, v] : sub.items()) {
            json new_obj = obj;
            json new_sub = json::object();
            new_sub[k] = v;
            new_obj[json_ptr] = new_sub;
            ret.emplace_back(make_pair(k, std::move(new_obj)));
        }
    }
    else if (sub.is_array()) {
        for (size_t i = 0; i < sub.size(); ++i) {
            json new_obj = obj;
            json new_sub = json::array();
            new_sub.emplace_back(sub[i]);
            new_obj[json_ptr] = new_sub;
            ret.emplace_back(make_pair(file_name + "_" + to_string(i), std::move(new_obj)));
        }
    }
    else throw not_object_or_array();

    return ret;
}
tuple<
    json,           // merged json
    vector<string>, // failed opens
    vector<string>, // failed parses
    vector<string>, // not contains
    uint32_t        // merged count
> merged_impl(const vector<string> &infiles, const json::json_pointer &json_ptr) {
    vector<string> failed_opens  = {};
    vector<string> failed_parses = {};
    vector<string> not_contains  = {};
    uint32_t merged_count = 0;

    ifstream fin(infiles[0]);
    if (!fin.is_open()) throw runtime_error("failed to open file: "s + infiles[0]);
    json merged = json::parse(fin, nullptr, false, true);
    if (merged.is_discarded()) throw runtime_error("failed to parse file: "s + infiles[0]);

    json &sub = merged[json_ptr];
    if (sub.is_object()) {
        merged[json_ptr] = json::object();
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
            merged[json_ptr].merge_patch(obj[json_ptr]);
            ++merged_count;
        }
    }
    else if (sub.is_array()) {
        merged[json_ptr] = json::array();
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
            merged[json_ptr] += obj[json_ptr];
            ++merged_count;
        }
    }
    else throw not_object_or_array();
    return make_tuple(merged, failed_opens, failed_parses, not_contains, merged_count);
}
} // namespace compo_merge

} // namespace hndimpl
#endif