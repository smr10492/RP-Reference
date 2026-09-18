#include "animation.h"
#include "common_impl.hpp"

namespace rpr {
RPR_DECL;
// class Animations

template<> void Animations::load(istream &is) {
    json obj = json::parse(is, nullptr, true, true);
    if (!obj.is_object()) return;
    if (obj.contains("format_version") && obj["format_version"].is_string()) {
        format_version_ = obj["format_version"].get<string>();
    }

    if (!obj.contains("animations") || !obj["animations"].is_object()) return;
    for (const auto &[k, v] : obj["animations"].items()) {
        if (!v.is_object()) continue;
        Animation anim; anim.load(v);
        compos_[k] = std::move(anim);
    }
}

} // namespace rpr

namespace rpr {
RPR_DECL;
// class Animation

void Animation::load(const json &obj) {
    // loop
    if (obj.contains("loop")) {
        if (obj["loop"].is_boolean()) {
            loop_mode_ = (obj["loop"].get<bool>()? loop_mode_t::loop : loop_mode_t::once);
        }
        else {
            string loop_mode_str = obj.value("loop", "");
            if (loop_mode_str == "hold_on_last_frame") loop_mode_ = loop_mode_t::hold;
        }
    }
    else loop_mode_ = loop_mode_t::once;

    // animation length
    length_ = obj.value("length", 0.0);

    // bones
    if (!obj.contains("bones") || !obj["bones"].is_object()) return;
    for (const auto &[k, v] : obj["bones"].items()) {
        if (!v.is_object()) continue;
        bone_names_.emplace(k);
    }
}

} // namespace rpr