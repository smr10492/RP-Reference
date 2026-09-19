#ifndef ANIMATION_H
#define ANIMATION_H
#include "common.h"
#include "merged_compo.hpp"

namespace rpr {
RPR_USING;
using std::map;
using std::set;

class Animation;
using Animations = MergedCompo<Animation>;
template<> inline Animations::MergedCompo(): MergedCompo_base("1.8.0") {}
template<> void MergedCompo<Animation>::load(istream &is);

class Animation {
public:
    friend class MergedCompo<Animation>;
    enum class loop_mode_t {
        once, // false
        loop, // true
        hold, // "hold_on_last_frame"
    };

private:
    loop_mode_t loop_mode_ = loop_mode_t::once;
    float length_ = 0.0f;
    set<string> bone_names_ = {};

public:
    Animation() = default;

    void load(const json &obj);

    void set_loop_mode(loop_mode_t loop_mode) noexcept {loop_mode_ = loop_mode;}
    void set_length(float length)             noexcept {length_ = length;}
    constexpr loop_mode_t   loop_mode() const noexcept {return loop_mode_;}
    constexpr float         length()    const noexcept {return length_;}

    constexpr const set<string> &bone_names() const noexcept {return bone_names_;}
    inline bool contains(const string &bone_name) const {return bone_names_.count(bone_name) > 0;}
};

} // namespace rpr

#endif
