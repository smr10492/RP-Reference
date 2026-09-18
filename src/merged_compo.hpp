#ifndef MERGED_COMPO_HPP
#define MERGED_COMPO_HPP
#include <string>
#include <map>
#include <istream>
#include <type_traits>

namespace rpr {
using std::string;
using std::map;
using std::istream;

class MergedCompo_base {
protected:
    string format_version_;

public:
    MergedCompo_base() = default;
    MergedCompo_base(const string &format_version): format_version_(format_version) {}

    inline const string &format_version() const noexcept {
        return format_version_;
    }
};
static_assert(!std::is_polymorphic_v<MergedCompo_base>, "MergedCompo_base must be non-polymorphic");

/// @brief Standard merged component KVs:
/// ```
/// {
///   format_version: number,
///   [component: string]: {
///     [key: string]: Tp,
///   }
/// }
/// ```
/// @tparam Tp Component type
template<typename Tp>
class MergedCompo : public MergedCompo_base {
private:
    using MergedCompo_base::format_version_;
    map<string, Tp> compos_ = {};

public:
    #define T_SELF template<typename Self>
    #define SELF this Self &&self
    using MergedCompo_base::format_version;

    MergedCompo(): MergedCompo_base() {}
    MergedCompo(const string &format_version): MergedCompo_base(format_version) {}

    void load(istream &is) = delete;

    inline bool contains(const string &identifier) const {return compos_.contains(identifier);}

    // no const version
    inline auto &operator[](const string &identifier) {return compos_[identifier];}

    // deducing this for (const?) version
    T_SELF inline auto &at  (SELF, const string &identifier) {return self.compos_.at(identifier);}
    T_SELF inline auto begin(SELF) noexcept {return self.compos_.begin();}
    T_SELF inline auto end  (SELF) noexcept {return self.compos_.end();}

    #undef T_SELF
    #undef SELF
};

} // namespace rpr

#endif