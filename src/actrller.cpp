#include "actrller.h"
#include "common_impl.hpp"

namespace rpr { // private
RPR_DECL;
// class ACtrllers

void ACtrllers::load(istream &is) {
    json obj = json::parse(is, nullptr, true, true);
    if (!obj.is_object()) return;
    if (obj.contains("format_version") && obj["format_version"].is_string()) {
        format_version_ = obj["format_version"].get<string>();
    }

    if (!obj.contains("animation_controllers") || !obj["animation_controllers"].is_object()) return;
    for (auto &[k, v] : obj["animation_controllers"].items()) {
        ACtrller act(v);
        if (act.empty()) continue;
        acontrollers_.try_emplace(std::move(k), std::move(act));
    }
}

} // namespace rpr

namespace rpr { // public
RPR_DECL;
// class ACtrller

ACtrller::ACtrller(const json &obj) {
    if (!obj.is_object()) return;

    if (!obj.contains("initial_state") || !obj["initial_state"].is_string()) return;
    initial_state_ = obj["initial_state"].get<string>();

    if (!obj.contains("states") || !obj["states"].is_object()) return;
    for (auto &[k, v] : obj["states"].items()) {
        if (!v.is_object()) continue;
        states_.try_emplace(std::move(k), std::move(v));
    }
}

template<> void ACtrller::mmd_out<mmd_type::flow>(ostream &os, bool with_header) {
    if (this->empty()) return;
    auto _name_dec = [](const string &name) {return "ST_" + name;};

    mmd::Flowchart fc(os);
    if (with_header) fc.begin_graph(mmd::lr);

    // initial_state and entry
    if (!states_.count(initial_state_)) throw runtime_error(
        "ACtrller::mmd_out: initial_state not found"
    );
    fc.node_declare("ST_entry", "__entry__", mmd::circle);

    // states declaration
    for (auto &it : states_) {
        if (it.second.empty()) continue;
        fc.node_declare(_name_dec(it.first), it.first);
    }
    fc.node_link("ST_entry", _name_dec(initial_state_));

    // states transition
    for (auto &it : states_) {
        if (it.second.empty()) continue;
        if (!it.second.contains("transitions")) continue;
        if (!it.second["transitions"].is_array()) continue;
        for (auto &e : it.second["transitions"]) {
            if (!e.is_object()) continue;
            auto it = e.begin();
            string cond_cmt = to_string(*it);
            fc.node_link(_name_dec(it.key()), it->get<string>(), cond_cmt);
        }
    }
}
template<> void ACtrller::mmd_out<mmd_type::states>(ostream &os, bool with_header) {
    if (this->empty()) return;
    auto _name_dec = [](const string &name) {return "ST_" + name;};

    mmd::StateDiagram sd(os);
    if (with_header) sd.begin_graph();

    // initial_state and entry
    if (!states_.count(initial_state_)) throw runtime_error(
        "ACtrller::mmd_out: initial_state not found"
    );

    // states declaration
    for (auto &it : states_) {
        if (it.second.empty()) continue;
        sd.state_declare(_name_dec(it.first), it.first);
    }
    sd.state_link("[*]", _name_dec(initial_state_));

    // states transition
    for (auto &it : states_) {
        if (it.second.empty()) continue;
        if (!it.second.contains("transitions")) continue;
        if (!it.second["transitions"].is_array()) continue;
        for (auto &e : it.second["transitions"]) {
            if (!e.is_object()) continue;
            auto it = e.begin();
            string cond_cmt = to_string(*it);
            sd.state_link(_name_dec(it.key()), it->get<string>(), cond_cmt);
        }
    }
}
} // namespace rpr