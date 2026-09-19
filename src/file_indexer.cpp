#include "file_indexer.h"
#include "common_impl.hpp"

namespace rpr {
RPR_USING;
// class FileIndexer

void FileIndexer::index([[maybe_unused]] void *data) {
    this->clear();
    if (!key_getter_) return;
    if (!fs::is_directory(folder_)) return;

    uint32_t i = 0;
    for (const auto &entry : fs::recursive_directory_iterator(
        folder_,
        fs::directory_options::skip_permission_denied
    )) {
        if (!entry.is_regular_file() || entry.path().extension() != suffix_) continue;

        auto find_strings = key_getter_(entry.path().generic_string(), data);
        for (auto &s : find_strings) kvs_.emplace(std::move(s), i);

        files_.emplace_back(fs::relative(entry.path(), folder_).generic_string());
        ++i;
    }
}

void FileIndexer::index_files(const vector<fs::path> &files, [[maybe_unused]] void *data) {
    if (!key_getter_) return;

    uint32_t i = 0;
    for (const auto &file : files) {
        if (!fs::is_regular_file(file)) continue;

        auto find_strings = key_getter_(file.generic_string(), data);
        for (auto &s : find_strings) kvs_.emplace(std::move(s), i);

        files_.emplace_back(fs::relative(file, folder_).generic_string());
        ++i;
    }
}

void FileIndexer::dump(ostream &os) const {
    if (os.fail()) return;

    // files_
    uint32_t files_size = (uint32_t)files_.size();
    os.write((const char*)&files_size, sizeof(files_size));
    for (auto &f : files_) {
        uint8_t len = (uint8_t)f.size();
        os.write((const char*)&len, sizeof(len));
        os.write(f.data(), len);
    }

    // kvs_
    uint32_t idx_size = (uint32_t)kvs_.size();
    os.write((const char*)&idx_size, sizeof(idx_size));
    for (auto &[k, v] : kvs_) {
        uint8_t len = (uint8_t)k.size();
        os.write((const char*)&len, sizeof(len));
        os.write(k.data(), len);
        os.write((const char*)&v, sizeof(v));
    }
}
} // namespace rpr

namespace rpr {
RPR_USING;
// class RPIndexer

FileIndexer RPIndexer::get_idx_impl(uint32_t compo_enum, istream &is) const {
    FileIndexer ret(rpr_base::folder_of(compo_enum), "", nullptr);
    if (is.fail()) return ret;

    // files_
    uint32_t files_count = 0;
    is.read(reinterpret_cast<char*>(&files_count), sizeof(files_count));
    ret.files_.reserve(files_count);
    for (uint32_t i = 0; i < files_count; ++i) {
        uint8_t len = 0;
        is.read(reinterpret_cast<char*>(&len), sizeof(len));
        string s(len, '\0');
        is.read(s.data(), len);
        ret.files_.emplace_back(std::move(s));
    }

    // kvs_
    uint32_t idx_count = 0;
    is.read(reinterpret_cast<char*>(&idx_count), sizeof(idx_count));
    ret.kvs_.reserve(idx_count);
    for (uint32_t i = 0; i < idx_count; ++i) {
        uint8_t len = 0;
        is.read(reinterpret_cast<char*>(&len), sizeof(len));
        string key(len, '\0');
        is.read(key.data(), len);
        uint32_t val = 0;
        is.read(reinterpret_cast<char*>(&val), sizeof(val));
        ret.kvs_.emplace(std::move(key), val);
    }
    return ret;
}

void RPIndexer::index([[maybe_unused]] void *data, ostream *log) {
    using byte_t = rpr::BrArch::byte_t;
    using elem_t = rpr::BrArch::elem_t;

    auto _index_dump = [&](const FileIndexer &index, rpr_base::compos_t c) {
        std::ostringstream oss("", std::ios::binary); index.dump(oss);
        string odata_str = oss.str();
        rpi_br_ << elem_t{
            string(rpr_base[c]) + this->elem_suffix,
            vector<byte_t>(odata_str.begin(), odata_str.end())
        };
    };
    auto _ani_like_index = [&](const char *type_name, FileIndexer::key_getter_t kg, rpr_base::compos_t c) {
        fs::path type_dir = fs::path(rp_dir_) / type_name;
        if (fs::is_directory(type_dir)) {
            if (log) *log << type_name << "...\n";
            FileIndexer type_index(type_dir.string(), ".json", kg); type_index.index();
            _index_dump(type_index, c);
        }
    };

    // RP path
    rpi_br_.clear();
    rpi_br_ << elem_t{
        this->dir_name,
        vector<byte_t>(rp_dir_.begin(), rp_dir_.end())
    };

    fs::path mat_dir = fs::path(rp_dir_) / "materials";
    if (fs::is_directory(mat_dir)) {
        if (log) *log << "materials...\n";
        FileIndexer mat_index(mat_dir.string(), ".material", rpr::rpidx::key_getter<rpr_base::mat>); mat_index.index();
        _index_dump(mat_index, rpr_base::mat);
    }
    fs::path tex_dir = fs::path(rp_dir_) / "textures";
    if (fs::is_directory(tex_dir)) {
        if (log) *log << "textures...\n";
        FileIndexer tex_index(tex_dir.string(), ".json", rpr::rpidx::key_getter<rpr_base::tex>);
        tex_index.index_files({
            tex_dir / "item_texture.json",
            tex_dir / "terrain_texture.json"
        });
        _index_dump(tex_index, rpr_base::tex);
    }
    _ani_like_index("models",                rpr::rpidx::key_getter<rpr_base::geo>, rpr_base::geo);
    _ani_like_index("animations",            rpr::rpidx::key_getter<rpr_base::ani>, rpr_base::ani);
    _ani_like_index("animation_controllers", rpr::rpidx::key_getter<rpr_base::act>, rpr_base::act);
    _ani_like_index("render_controllers",    rpr::rpidx::key_getter<rpr_base::rct>, rpr_base::rct);
    _ani_like_index("particles",             rpr::rpidx::key_getter<rpr_base::prt>, rpr_base::prt);
    fs::path snd_dir = fs::path(rp_dir_) / "sounds";
    if (fs::is_directory(snd_dir)) {
        if (log) *log << "sounds...\n";
        FileIndexer snd_index(snd_dir.string(), ".json", rpr::rpidx::key_getter<rpr_base::snd>);
        snd_index.index_files({
            snd_dir / "sound_definitions.json"
        });
        _index_dump(snd_index, rpr_base::snd);
    }
}
} // namespace rpr

namespace rpr { inline namespace rpidx {
RPR_USING;

static bool _try_get_json(const string &file, json &obj) {
    using namespace std;

    ifstream fin(file); if (!fin.is_open()) return false;
    obj = json::parse(fin, nullptr, false, true);
    if (obj.is_discarded()) {
        clog << file << " is discarded JSON object\n";
        return false;
    }
    fin.close();

    return true;
};

/// @note format like:
/// @note "format_version": number,
/// @note [obj_key]: {[elem]: object}
static vector<string> _ani_like_key_getter(
    const string &file,
    const char *obj_key,
    [[maybe_unused]] void *data
) {
    vector<string> ret = {};
    json obj;
    if (!_try_get_json(file, obj)) return ret;

    if (!obj.contains("format_version") || !obj["format_version"].is_string()) return ret;
    if (!obj.contains(obj_key) || !obj[obj_key].is_object()) return ret;

    for (auto &[k, v] : obj[obj_key].items()) {
        if (!v.is_object()) continue;
        ret.push_back(k);
    }

    return ret;
}
template<> inline vector<string>
key_getter<rpr_base::ani>(const string &file, [[maybe_unused]] void *data) {
    return _ani_like_key_getter(file, "animations", data);
}
template<> inline vector<string>
key_getter<rpr_base::act>(const string &file, [[maybe_unused]] void *data) {
    return _ani_like_key_getter(file, "animation_controllers", data);
}
template<> inline vector<string>
key_getter<rpr_base::rct>(const string &file, [[maybe_unused]] void *data) {
    return _ani_like_key_getter(file, "render_controllers", data);
}
template<> inline vector<string>
key_getter<rpr_base::snd>(const string &file, [[maybe_unused]] void *data) {
    return _ani_like_key_getter(file, "sound_definitions", data);
}

template<> vector<string>
key_getter<rpr_base::mat>(const string &file, [[maybe_unused]] void *data) {
    vector<string> ret = {};
    json obj;
    if (!_try_get_json(file, obj)) return ret;

    if (!obj.is_object()) return ret;
    if (!obj.contains("materials") || !obj["materials"].is_object()) return ret;
    if (!obj["materials"].contains("version")) return ret;
    obj.erase("version");

    for (auto &[k, v] : obj["materials"].items()) {
        if (!v.is_object()) continue;
        std::string_view sv = k;
        auto pos = sv.rfind(':');
        ret.emplace_back(sv.substr(0, pos));
    }

    return ret;
}
template<> vector<string>
key_getter<rpr_base::tex>(const string &file, [[maybe_unused]] void *data) {
    vector<string> ret = {};
    json obj;
    if (!_try_get_json(file, obj)) return ret;

    if (!obj.is_object()) return ret;
    if (!obj.contains("resource_pack_name") || !obj["resource_pack_name"].is_string()) return ret;
    if (!obj.contains("texture_name")       || !obj["texture_name"]      .is_string()) return ret;
    if (!obj.contains("texture_data")       || !obj["texture_data"]      .is_object()) return ret;

    for (auto &[k, v] : obj["texture_data"].items()) {
        if (!v.is_object()) continue;
        ret.push_back(k);
    }

    return ret;
}
template<> vector<string>
key_getter<rpr_base::geo>(const string &file, [[maybe_unused]] void *data) {
    vector<string> ret = {};
    json obj;
    if (!_try_get_json(file, obj)) return ret;

    if (!obj.is_object()) return ret;
    if (!obj.contains("format_version") || !obj["format_version"].is_string()) return ret;
    if (!obj.contains("minecraft:geometry") || !obj["minecraft:geometry"].is_array()) return ret;

    auto descr_jptr = "/description/identifier"_json_pointer;
    for (auto &o : obj["minecraft:geometry"]) {
        if (!o.is_object()) continue;
        if (!o.contains(descr_jptr) || !o[descr_jptr].is_string()) continue;
        ret.push_back(o[descr_jptr].get<string>());
    }

    return ret;
}
template<> vector<string>
key_getter<rpr_base::prt>(const string &file, [[maybe_unused]] void *data) {
    vector<string> ret = {};
    json obj;
    if (!_try_get_json(file, obj)) return ret;

    if (!obj.is_object()) return ret;
    if (!obj.contains("format_version")  || !obj["format_version"] .is_string()) return ret;
    if (!obj.contains("particle_effect") || !obj["particle_effect"].is_object()) return ret;

    auto descr_jptr = "/particle_effect/description/identifier"_json_pointer;
    if (!obj.contains(descr_jptr) || !obj[descr_jptr].is_string()) return ret;
    ret.push_back(obj[descr_jptr].get<string>());

    return ret;
}
} // namespace rpidx
} // namespace rpr
