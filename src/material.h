#ifndef MATERIAL_H
#define MATERIAL_H
#include "common.h"

namespace rpr {
RPR_USING;
using std::pair;

class Material {
private:
    string version_ = "1.0.0";
    vector<pair<string, json>> mats_ = {};

public:
    Material() = default;
    Material(const json &obj);

    inline void load(const json &obj) {*this = Material(obj);}

    json forest() const;
};

} // namespace rpr

#endif // MATERIAL_H