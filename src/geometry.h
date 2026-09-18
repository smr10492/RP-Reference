#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "common.h"
#include "merged_compo.hpp"

namespace rpr {
RPR_DECL;
using std::map;
using std::set;

class Geometry;
using Geometries = MergedCompo<Geometry>;
template<> inline Geometries::MergedCompo(): MergedCompo_base("1.12.0") {}
template<> void MergedCompo<Geometry>::load(istream &is);

class Geometry {
public:
    friend class MergedCompo<Geometry>;
    struct bone_t {
        string parent;
    };

private:
    map<string, bone_t> bones_ = {};

public:
    Geometry() = default;

    void load(const json &obj);

    inline set<string> bone_names() const {
        set<string> ret;
        for (const auto &[name, _] : bones_) ret.insert(name);
        return ret;
    }
    inline bool contains(const string &bone) const {return bones_.contains(bone);}

    inline auto begin() const noexcept {return bones_.begin();}
    inline auto end()   const noexcept {return bones_.end();}

    void tree_out(ostream &os) const;
};

} // namespace rpr

#endif