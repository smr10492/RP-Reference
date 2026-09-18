#ifndef FILE_INDEX_H
#define FILE_INDEX_H
#include "common.h"
#include "brarchive.h"

namespace rpr {
RPR_DECL;

class RPIndexer;

/// @brief Indexer from key to file
class FileIndexer {
/*
*this binary: {
@files:
    uint32_t files_count;
    struct {
        uint8_t len;
        char    name[len];
    } files[files_count];

@indexes:
    uint32_t idx_count;
    struct {
        uint8_t  len;
        char     key[len];
        uint32_t val;
    } indexes[idx_count];
}
*/
public:
    friend class RPIndexer;
    typedef vector<string> (*key_getter_t)(const string &file, void *data);

private:
    std::unordered_map<string, uint32_t> kvs_;
    vector<string> files_;
    string folder_ = "";
    string suffix_ = ""; // filter files by suffix
    key_getter_t key_getter_ = nullptr;

public:
    FileIndexer() = default;
    FileIndexer(const string &folder, const string &suffix, key_getter_t kg = nullptr) noexcept:
        folder_(folder),
        suffix_(suffix),
        key_getter_(kg)
    {}
    constexpr auto set_key_getter(key_getter_t kg) -> decltype(*this)& {
        key_getter_ = kg;
        return *this;
    }
    inline auto set_folder(const string &folder) -> decltype(*this)& {
        folder_ = folder;
        return *this;
    }
    inline auto set_suffix(const string &suffix) -> decltype(*this)& {
        suffix_ = suffix;
        return *this;
    }

    inline const decltype(suffix_) &suffix() const noexcept {return suffix_;}
    inline const decltype(files_)  &files()  const noexcept {return files_;}
    inline const decltype(folder_) &folder() const noexcept {return folder_;}

    inline size_t size()  const noexcept {return kvs_.size();}
    inline bool   empty() const noexcept {return kvs_.empty();}

    /// @brief Index files in directory (after setting directory)
    void index(void *data = nullptr);

    /// @brief Index files
    void index_files(const vector<fs::path> &files, void *data = nullptr);

    inline bool contains(const string &s) const {return kvs_.contains(s);}
    inline const string &operator[](const string &s) const {
        static const string empty_string = "";
        auto it = kvs_.find(s);
        if (it == kvs_.end()) return empty_string;
        return files_[it->second];
    }
    inline uint32_t index_at(const string &s) const {
        return kvs_.at(s);
    }
    inline const string &at(const string &s) const {
        return files_.at(kvs_.at(s));
    }

    inline void clear() noexcept {
        kvs_  .clear();
        files_.clear();
    }

    inline auto begin() const noexcept {return kvs_.begin();}
    inline auto end()   const noexcept {return kvs_.end();}

    /// @brief Dump indexer to binary stream
    void dump(ostream &os) const;
};

/// @brief rpr::FileIndexer set for each component
class RPIndexer {
/*
*this binary: same as class BrArch ()
*/
public:
    using elem_t = FileIndexer;
    static constexpr const char *dir_name    = ".rpdir"; // file name for directory
    static constexpr const char *elem_suffix = ".rci";   // suffix for element files

private:
    string rp_dir_ = "";
    BrArch rpi_br_ = BrArch(2);

    elem_t get_idx_impl(uint32_t compo_enum, istream &is) const;

public:
    RPIndexer() = default;
    RPIndexer(const char *rp_dir): rp_dir_(rp_dir) {}
    RPIndexer(const string &rp_dir): RPIndexer(rp_dir.c_str()) {}

    inline bool load(const string &file) {
        if (!rpi_br_.load(file)) return false;
        rp_dir_ = string(rpi_br_[this->dir_name].data.begin(), rpi_br_[this->dir_name].data.end());
        return true;
    }

    constexpr const string &rp_dir() const noexcept {return rp_dir_;}

    /// @brief Get component indexer
    inline elem_t get(uint32_t compo_enum) const {
        auto &rpi_data = rpi_br_[string(rpr_base[compo_enum]) + this->elem_suffix].data;
        auto rpi_iss = std::istringstream(string(rpi_data.begin(), rpi_data.end()), std::ios::binary);
        return get_idx_impl(compo_enum, rpi_iss);
    }

    /// @brief Call .load on component file with ID
    /// @tparam Tp Component type
    /// @note requires Tp::load(std::istream&)
    template<typename Tp>
    Tp load_from(uint32_t compo_enum, const string &id) const
    requires requires(Tp compo_enum, std::istream &fin) {compo_enum.load(fin);} {
        using namespace std;

        auto compo_idx = this->get(compo_enum);
        if (!compo_idx.contains(id)) throw runtime_error(
            "the ID of "s + rpr_base::name_of(compo_enum) + " is not found: " + id
        );

        fs::path compo_fp = fs::path(rp_dir_) / rpr_base::folder_of(compo_enum) / compo_idx[id];
        ifstream compo_fin(compo_fp);
        if (!compo_fin.is_open()) throw runtime_error(
            "failed to open "s + rpr_base::name_of(compo_enum) + " file: " + compo_fp.generic_string()
        );

        Tp ret; ret.load(compo_fin); compo_fin.close();
        return ret;
    }

    /// @brief Index files in directory (after setting directory)
    /// @param log Log stream
    void index(void *data = nullptr, ostream *log = nullptr);
    inline void dump(ostream &os) const {rpi_br_.dump(os);}
};
} // namespace rpr

namespace rpr { inline namespace rpidx {
RPR_DECL;

template<rpr_base::compos_t c>
vector<string> key_getter(const string &file, void *data = nullptr) = delete;

template<> vector<string> key_getter<rpr_base::mat>(const string &file, void *data);
template<> vector<string> key_getter<rpr_base::tex>(const string &file, void *data);
template<> vector<string> key_getter<rpr_base::geo>(const string &file, void *data);
template<> vector<string> key_getter<rpr_base::ani>(const string &file, void *data);
template<> vector<string> key_getter<rpr_base::act>(const string &file, void *data);
template<> vector<string> key_getter<rpr_base::rct>(const string &file, void *data);
template<> vector<string> key_getter<rpr_base::prt>(const string &file, void *data);
template<> vector<string> key_getter<rpr_base::snd>(const string &file, void *data);

} // namespace rpidx
} // namespace rpr

#endif // FILE_INDEX_H