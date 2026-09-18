#ifndef MERMAID_H
#define MERMAID_H
#include <iostream>
#include <string>
#include <cstdint>

namespace mmd {
using std::string;
using std::ostream;

enum mmd_type: uint8_t {
    flow, states
};

inline namespace flowchart {

enum node_shape: uint32_t {
    square     = 0, //  [...]
    round      = 1, //  (...)
    circle     = 2, // ((...))
    stadium    = 3, // ([...])
    asymmetric = 4  // {{...}}
};

const char *const header_lr = "graph LR";
const char *const header_tb = "graph TB";
enum graph_mode: uint8_t {lr, tb};

inline void comment(ostream &os, const string &s) {
    os << "%%" << s << "\n";
}
inline void multi_comment(ostream &os, const string &s) {
    os << "%%{" << s << "}%%\n";
}

} // namespace flowchart

class Flowchart {
private:
    ostream &os_;
    uint32_t graph_depth_ = 0;

    inline string shaped(
        const string &s,
        const node_shape shape = node_shape::square
    ) noexcept;

    inline string node_arrowed(
        const string &id_src,
        const string &id_dst,
        const string &comment = ""
    ) noexcept {
        const string cmt = (comment.empty()? "" : ("|" + comment + "|"));
        return id_src + " -->" + cmt + " " + id_dst;
    }

public:
    Flowchart() = delete;
    Flowchart(ostream &os):
        os_(os), graph_depth_(0)
    {}

    bool begin_graph(graph_mode mode = graph_mode::lr) noexcept;
    bool begin_subgraph(
        const string &id,
        const string &name = "",
        node_shape shape = node_shape::square
    ) noexcept;

    void end_graph();
    void end_subgraph();

    void node_declare(
        const string &id,
        const string &name = "",
        node_shape shape = node_shape::square
    );
    void node_link(
        const string &id_src,
        const string &id_dst,
        const string &comment = ""
    );
};

class StateDiagram {
private:
    ostream &os_;
    uint32_t graph_depth_ = 0;

    constexpr string state_arrowed(
        const string &name_src,
        const string &name_dst,
        const string &comment = ""
    ) noexcept {
        const string cmt = (comment.empty()? "" : (": " + comment));
        return name_src + " --> " + name_dst + cmt;
    }

public:
    StateDiagram() = delete;
    StateDiagram(ostream &os):
        os_(os), graph_depth_(0)
    {}

    bool begin_graph() noexcept;
    bool begin_multistate(const string &name) noexcept;

    void end_graph();
    void end_subgraph();

    void state_declare(
        const string &name,
        const string &comment = ""
    );
    void state_link(
        const string &name_src,
        const string &name_dst,
        const string &comment = ""
    );
};

} // namespace mmd
#endif // MERMAID_H