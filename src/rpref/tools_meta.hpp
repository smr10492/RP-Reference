#ifndef TOOLS_META_HPP
#define TOOLS_META_HPP
#include "main_rpref_impl.h"
#define NAMEOF(var) #var

namespace meta {
using json = nlohmann::json;
using namespace rprmcp;

static constexpr const unsigned char schema[] = {
    #embed "schemata.json"
    , '\0'
};
static const json schema_json = json::parse(schema, nullptr, true, true);

static const tool_meta_t
ac_to_mmd = make_tool_meta(
    NAMEOF(ac_to_mmd),
    "Convert animation-controllers to mermaid states graphs",
    schema_json
),
rpi_index = make_tool_meta(
    NAMEOF(rpi_index),
    "Get all IDs of components to a map(id -> file)",
    schema_json
),
rpi_find = make_tool_meta(
    NAMEOF(rpi_find),
    "Get file path of IDs",
    schema_json
),
sn_map = make_tool_meta(
    NAMEOF(sn_map),
    "Get short-name map of an entity file",
    schema_json
),
bone_coverage = make_tool_meta(
    NAMEOF(bone_coverage),
    "Partition used bones to {both, geo_only, ani_only}",
    schema_json
),
geo_tree = make_tool_meta(
    NAMEOF(geo_tree),
    "Get geometry bone forest of a geometry file",
    schema_json
),
br_extract = make_tool_meta(
    NAMEOF(br_extract),
    "Extract each *.brarchive file into a folder with the same name as the file",
    schema_json
),
br_zip = make_tool_meta(
    NAMEOF(br_zip),
    "Archive all files to <dir: where the first file is>.brarchive",
    schema_json
),
br_merge = make_tool_meta(
    NAMEOF(br_merge),
    "Merge all *.brarchive files to one",
    schema_json
),
br_auto = make_tool_meta(
    NAMEOF(br_auto),
    "Automatically execute br_extract or br_zip for the whole Resource Pack (excluding subpacks)\n"
    "The runtime may be long, please do not interrupt it",
    schema_json
),
dir_compo_rename = make_tool_meta(
    NAMEOF(dir_compo_rename),
    "Rename component JSON files in a directory",
    schema_json
),
rpi_compo_rename = make_tool_meta(
    NAMEOF(rpi_compo_rename),
    "Rename component JSON files from RPI file",
    schema_json
),
unmerge = make_tool_meta(
    NAMEOF(unmerge),
    "Unmerge KVs from an object or an array (for merged.json)",
    schema_json
),
merged = make_tool_meta(
    NAMEOF(merged),
    "Merge KVs to an object or elements from an array",
    schema_json
),
mat_forest = make_tool_meta(
    NAMEOF(mat_forest),
    "View material extensions forest of a material file",
    schema_json
);
} // namespace meta

#undef NAMEOF
#endif