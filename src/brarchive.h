#include "common.h"
#ifndef BRARCHIVE_H
#define BRARCHIVE_H

class br_base {
public:
    static inline constexpr uint64_t magic          = 0x267052A0B125277D;
    static inline constexpr uint32_t format_version = 1;
    static inline constexpr uint32_t header_len     = sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint32_t);
    static inline constexpr uint8_t  name_len       = 0x100 - sizeof(uint32_t) - sizeof(uint32_t) - sizeof(uint8_t);
    static inline constexpr auto     extension      = ".brarchive";
    static inline constexpr auto     folder         = "__brarchive";
    static_assert(header_len == 0x10, "br_base::header_len must be 0x10");
    static_assert(name_len   == 0xF7, "br_base::name_len must be 0xF7");
};

namespace rpr {
RPR_USING;
using std::ostream;
using std::find_if;

class BrArch {
/*
*this binary: {
@header:
    uint64_t magic = 0x267052A0B125277D;
    uint32_t format_version = 1;
    uint32_t files_count;

@meta:
    struct {
        uint8_t  len;
        char     name[0xF7];
        uint32_t data_offset;
        uint32_t data_size;
    } meta[files_count];

@data:
    byte_t data[];
}
*/
public:
    typedef uint8_t byte_t;
    class elem_t {
    public:
        string         name = string(br_base::name_len, '\0');
        vector<byte_t> data = vector<byte_t>(0x00);
    };
private:
    uint32_t format_version_ = br_base::format_version;
    vector<elem_t> elems_ = {};

public:
    #define T_SELF template<typename Self>
    #define SELF this Self &&self

    BrArch() = default;
    BrArch(const uint32_t &format_version): format_version_(format_version) {}
    bool load(const string&);

    T_SELF inline auto begin(SELF) noexcept {return self.elems_.begin();}
    T_SELF inline auto end  (SELF) noexcept {return self.elems_.end();}

    /// @brief Add element to archive
    constexpr void operator<<(const elem_t &elem) {elems_.emplace_back(elem);}

    /// @brief Add elements from archive
    constexpr void operator<<(const BrArch &br) {
        for (const auto &elem : br) {elems_.emplace_back(elem);}
    }

    inline auto size()  const noexcept {return elems_.size();}
    inline auto empty() const noexcept {return elems_.empty();}

    inline bool contains(const string &name) const {
        auto it = find_if(
            elems_.cbegin(), elems_.cend(),
            [&name](const elem_t &elem) {return elem.name == name;}
        );
        return it != elems_.cend();
    }

    T_SELF constexpr const auto &operator[](SELF, const string &name) {
        static const elem_t default_elem = {"", {}};
        auto it = find_if(
            self.elems_.cbegin(), self.elems_.cend(),
            [&name](const elem_t &elem) {return elem.name == name;}
        );
        if (it == self.elems_.cend()) return default_elem;
        return *it;
    }
    T_SELF inline auto &at(SELF, const string &name) {
        if (!self.contains(name)) throw std::out_of_range("element not found");
        return *find_if(
            self.elems_.begin(), self.elems_.end(),
            [&name](const elem_t &elem) {return elem.name == name;}
        );
    }

    void   dump(ostream&) const;
    string dump() const;

    constexpr void clear() noexcept {
        elems_.clear();
    }

    #undef T_SELF
    #undef SELF
};

} // namespace rpr
#endif // BRARCHIVE_H