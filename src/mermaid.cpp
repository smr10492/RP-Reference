#include "mermaid.h"

namespace mmd {
using std::string;
using std::runtime_error;
// class Flowchart

inline string Flowchart::shaped(
    const string &s,
    const node_shape shape
) noexcept {
    static const char *const node_shape_l[] = {
        "[",
        "(",
        "((",
        "([",
        "{{"
    };
    static const char *const node_shape_r[] = {
        "]",
        ")",
        "))",
        "])",
        "}}"
    };
    auto idx = static_cast<uint32_t>(shape);
    return node_shape_l[idx] + s + node_shape_r[idx];
}

bool Flowchart::begin_graph(graph_mode mode) noexcept {
    if (graph_depth_ != 0) return false;

    os_ << ((mode == graph_mode::lr)? header_lr : header_tb) << '\n';

    ++graph_depth_;
    return true;
}

bool Flowchart::begin_subgraph(
    const string &id,
    const string &name,
    node_shape shape
) noexcept {
    if (id.empty()) return false;
    if (graph_depth_ == 0) return false;

    os_ << "subgraph "
        << (name.empty()? "" : this->shaped(name, shape))
        << " \n";

    ++graph_depth_;
    return true;
}

void Flowchart::end_graph() {
    if (graph_depth_ != 1) throw runtime_error(
        "Flowchart::end_graph: graph not closed"
    );

    os_ << "end\n";

    graph_depth_ = 0;
}

void Flowchart::end_subgraph() {
    if (graph_depth_ < 2) throw runtime_error(
        "Flowchart::end_subgraph: graph not closed"
    );

    os_ << "end\n";

    --graph_depth_;
}

void Flowchart::node_declare(
    const string &id,
    const string &name,
    node_shape shape
) {
    if (id.empty()) return;

    os_ << id
        << (name.empty()? "" : this->shaped(name, shape))
        << '\n';
}

void Flowchart::node_link(
    const string &id_src,
    const string &id_dst,
    const string &comment
) {
    if (id_src.empty() || id_dst.empty()) return;

    os_ << this->node_arrowed(id_src, id_dst, comment)
        << '\n';
}
} // namespace mmd

namespace mmd {
using std::string;
using std::runtime_error;
// class StateDiagram

bool StateDiagram::begin_graph() noexcept {
    if (graph_depth_ != 0) return false;

    os_ << "stateDiagram-v2\n";

    ++graph_depth_;
    return true;
}

bool StateDiagram::begin_multistate(const string &name) noexcept {
    if (graph_depth_ != 0) return false;

    os_ << "state " << name << "{\n";

    ++graph_depth_;
    return true;
}

void StateDiagram::end_graph() {
    if (graph_depth_ != 1) throw runtime_error(
        "StateDiagram::end_graph: graph not closed"
    );

    os_ << "}\n";

    graph_depth_ = 0;
}

void StateDiagram::end_subgraph() {
    if (graph_depth_ < 2) throw runtime_error(
        "StateDiagram::end_multistate: state not closed"
    );

    os_ << "}\n";

    --graph_depth_;
}

void StateDiagram::state_declare(
    const string &name,
    const string &comment
) {
    if (name.empty()) return;

    os_ << name
        << (comment.empty()? "" : ("%% " + comment))
        << '\n';
}

void StateDiagram::state_link(
    const string &name_src,
    const string &name_dst,
    const string &comment
) {
    if (name_src.empty() || name_dst.empty()) return;

    os_ << this->state_arrowed(name_src, name_dst, comment)
        << '\n';
}
} // namespace mmd