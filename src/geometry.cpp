#include "geometry.h"
#include "common_impl.hpp"

namespace rpr {
RPR_DECL;
// class Geometries

template<> void Geometries::load(istream &is) {
    json obj = json::parse(is, nullptr, true, true);
    if (!obj.is_object()) return;
    if (obj.contains("format_version") && obj["format_version"].is_string()) {
        format_version_ = obj["format_version"].get<string>();
    }

    if (!obj.contains("minecraft:geometry") || !obj["minecraft:geometry"].is_array()) return;
    for (const auto &g_json : obj["minecraft:geometry"]) {
        if (!g_json.is_object()) continue;
        string id = g_json.value("/description/identifier"_json_pointer, "");
        if (id.empty()) continue;
        Geometry geo; geo.load(g_json);
        compos_[id] = geo;
    }
}

} // namespace rpr

namespace rpr {
RPR_DECL;
// class Geometry

void Geometry::load(const json &obj) {
    if (!obj.is_object()) return;
    if (!obj.contains("description") || !obj["description"].is_object()) return;

    if (!obj.contains("bones") || !obj["bones"].is_array()) return;
    for (const auto &b : obj["bones"]) {
        if (!b.is_object()) continue;
        string name = b.value("name", "");
        if (name.empty()) continue;
        bones_[name].parent = b.value("parent", "");
    }
}

void Geometry::tree_out(std::ostream &os) const {
    using namespace std;

    // parent -> children
    multimap<string, string> bones_forest;
    for (const auto &[name, bone] : bones_) {
        bones_forest.insert({bone.parent, name});
    }

    // roots: set<string>
    vector<string> roots;
    for (const auto &[name, bone] : bones_) {
        if (bone.parent.empty() || !bones_.contains(bone.parent)) {
            roots.push_back(name);
        }
    }
    sort(roots.begin(), roots.end());

    auto print_node = [&](
        this auto &&_self,
        const string &name,
        set<string> &visited,
        const string &prefix = ""
    ) {
        os << prefix << "- " << name << "\n";
        string child_prefix = prefix + "    ";

        if (!bones_forest.contains(name)) return;
        const auto &bones_tree = bones_forest.equal_range(name);
        for (auto it = bones_tree.first; it != bones_tree.second; ++it) {
            const auto &bone = it->second;
            if (visited.contains(bone)) {
                os << child_prefix << "- " << bone << " (ERROR: cycle)\n";
                continue;
            }
            // record visited on path
            visited.insert(bone);
            _self(bone, visited, child_prefix);
            visited.erase(bone);
        }
    };

    set<string> visited;
    for (const string &root : roots) {
        visited.insert(root);
        print_node(root, visited);
        visited.erase(root);
    }
}

} // namespace rpr