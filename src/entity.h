#ifndef ENTITY_H
#define ENTITY_H
#include "common.h"

namespace rpr {
RPR_DECL;

/// @brief Entity or Attachable
class Entity {
protected:
    string format_version_ = "1.26.0";
    string identifier_ = "";
    std::map<string, string>
        materials_,
        textures_,
        geometry_,
        animations_,
        particles_,
        sounds_;
    vector<json> rcontrollers_;
    bool is_attachable_ = false;

    void _parse_description_impl(const json &obj);

public:
    Entity() = default;
    Entity(const string &format_version):
        format_version_(format_version)
    {}
    Entity(const json &obj) {load(obj);}

    void load(const json &obj);

    inline const decltype(materials_)    &mat() {return materials_;}    // material
    inline const decltype(textures_)     &tex() {return textures_;}     // textures
    inline const decltype(geometry_)     &geo() {return geometry_;}     // geometry
    inline const decltype(animations_)   &anc() {return animations_;}   // animations and a-controllers
    inline const decltype(rcontrollers_) &rct() {return rcontrollers_;} // render-controllers

    inline bool is_attachable() const noexcept {return is_attachable_;}
};
} // namespace rpr

#endif // ENTITY_H