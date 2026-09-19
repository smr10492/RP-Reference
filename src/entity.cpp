#include "entity.h"
#include "common_impl.hpp"

namespace rpr {
RPR_USING;

inline void Entity::_parse_description_impl(const json &descr) {
    if (descr.contains("identifier") && descr["identifier"].is_string()) {
        identifier_ = descr["identifier"].get<string>();
    }

    if (descr.contains("materials") && descr["materials"].is_object()) {
        materials_ = descr["materials"].get<decltype(materials_)>();
    }
    if (descr.contains("textures") && descr["textures"].is_object()) {
        textures_ = descr["textures"].get<decltype(textures_)>();
    }
    if (descr.contains("geometry") && descr["geometry"].is_object()) {
        geometry_ = descr["geometry"].get<decltype(geometry_)>();
    }

    if (descr.contains("animations") && descr["animations"].is_object())
    for (auto &[id, anim] : descr["animations"].items()) {
        if (anim.is_string()) animations_[id] = anim.get<string>();
    }

    if (descr.contains("render_controllers") && descr["render_controllers"].is_array()) {
        rcontrollers_ = descr["render_controllers"].get<decltype(rcontrollers_)>();
    }
}
void Entity::load(const json &obj) {
    auto ent_jptr = "/minecraft:client_entity/description"_json_pointer;
    auto att_jptr = "/minecraft:attachable/description"_json_pointer;

    if (!obj.contains("format_version") || !obj["format_version"].is_string()) return;
    format_version_ = obj["format_version"].get<string>();

    is_attachable_ = obj.contains(att_jptr);
    if (!is_attachable_ && !obj.contains(ent_jptr)) return;

    auto &descr = is_attachable_? obj[att_jptr] : obj[ent_jptr];
    _parse_description_impl(descr);
}
} // namespace rpr
