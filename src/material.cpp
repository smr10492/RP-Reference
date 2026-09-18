#include "material.h"
#include "common_impl.hpp"

namespace rpr {
RPR_DECL;
using std::set;

Material::Material(const json &obj):
    version_(obj.value("version", "1.0.0"))
{
    for (const auto &[name, mat] : obj["materials"].items()) {
        mats_.emplace_back(name, mat);
    }
}

json Material::forest() const {
    typedef struct key_t {
        string name;
        string parent;
    } key_t;
    typedef struct node_t {
        string  name;
        node_t *parent;
        bool    as_parent; // appear on parent side
    } node_t;

    json ret = json::object();
    ret["defined"]   = json::object();
    ret["undefined"] = json::object();
    vector<key_t>  keys  = {};
    vector<node_t> nodes = {};
    keys .reserve(mats_.size());
    nodes.reserve(mats_.size());

    for (const auto &[name, _] : mats_) {
        auto pos = name.rfind(":");
        keys .emplace_back(name.substr(0, pos), name.substr(pos + 1));
        nodes.emplace_back(name.substr(0, pos),  nullptr, false);
        nodes.emplace_back(name.substr(pos + 1), nullptr, true);
    }
    for (auto &[name, parent] : keys) {
        auto parent_it = find_if(
            nodes.begin(), nodes.end(),
            [&parent](const node_t &n) {return n.name == parent;}
        );
        if (parent_it == nodes.end()) {
            ret["undefined"][name] = json::object();
            continue;
        }
        (*(parent_it - 1)).parent = &(*parent_it);
    }

    vector<node_t*> parent_stack = {};
    parent_stack.reserve(nodes.size());
    for (auto &node : nodes) {
        auto node_p = &node;
        while (node_p->parent) {
            parent_stack.emplace_back(node_p);
            node_p = node_p->parent;
        }
        for (auto it = parent_stack.rbegin(); it != parent_stack.rend(); ++it) {
            ret["defined"][(*it)->name] = json::object();
        }
        parent_stack.clear();
    }

    return ret;
}

} // namespace rpr