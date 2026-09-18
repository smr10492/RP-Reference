#ifndef RPR_BASE_HPP
#define RPR_BASE_HPP
#include <cstdint>
#include <array>

class rpr_base {
public:
    enum compos_t: uint32_t {
        mat = 0, tex = 1, geo = 2, ani = 3,
        act = 4, rct = 5, prt = 6, snd = 7
    };

    static constexpr auto compos_str = std::array{
        "mat", "tex", "geo", "ani",
        "act", "rct", "prt", "snd"
    };
    static constexpr auto compos_str_full = std::array{
        "material",      "texture",       "geometry",  "animations",
        "a-controllers", "r-controllers", "particles", "sounds"
    };
    static constexpr auto compos_folder = std::array{
        "materials",             "textures",           "models",    "animations",
        "animation_controllers", "render_controllers", "particles", "sounds"
    };
    static_assert(
        std::size(compos_str) == std::size(compos_str_full) &&
        std::size(compos_str) == std::size(compos_folder),
        "different std::size() of {compos_str, compos_str_full, compos_folder}"
    );
    static constexpr size_t size = std::size(compos_str);

    // get 3-char string tag of the component
    static constexpr auto operator[](uint32_t compo_enum) noexcept {return compos_str[compo_enum];}
    static constexpr auto tag_of    (uint32_t compo_enum) noexcept {return compos_str[compo_enum];}

    // get full string of the component
    static constexpr auto name_of(uint32_t compo_enum) noexcept {return compos_str_full[compo_enum];}

    // get folder name of the component
    static constexpr auto folder_of(uint32_t compo_enum) noexcept {return compos_folder[compo_enum];}
};
inline class rpr_base rpr_base;
static_assert(sizeof(rpr_base) == 1, "class rpr_base has non-static member(s)");

#endif