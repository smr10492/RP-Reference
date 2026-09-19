#ifndef ACTRLLER_H
#define ACTRLLER_H
#include "common.h"
#include "mermaid.h"

namespace rpr {
RPR_USING;
using std::map;

/// @brief 单个动画控制器（状态机）
class ACtrller {
private:
    string initial_state_ = "default";
    map<string, json> states_;

public:
    ACtrller() = default;
    ACtrller(const json &obj);
    inline bool empty() const noexcept {return states_.empty();}

    inline bool contains(const string &name) const {return states_.contains(name);}
    inline json &operator[](const string &name) {return states_[name];}
    inline const json &at(const string &name) const {return states_.at(name);}

    auto begin() noexcept {return states_.begin();}
    auto end()   noexcept {return states_.end();}

    template<mmd::mmd_type Tmt>
    void mmd_out(ostream &os, bool with_header = true) {}

    template<mmd::mmd_type Tmt>
    inline string mmd_dump(bool with_header = true) {
        std::ostringstream oss;
        mmd_out<Tmt>(oss, with_header);
        return oss.str();
    }
};

/// @brief 动画控制器文件
class ACtrllers {
private:
    string format_version_ = "1.10.0";
    map<string, ACtrller> acontrollers_;

public:
    ACtrllers() = default;
    ACtrllers(const string &format_version):
        format_version_(format_version)
    {}

    void load(istream &is);

    inline bool contains(const string &name) const {return acontrollers_.contains(name);}
    inline ACtrller &operator[](const string &name) {return acontrollers_[name];}
    inline ACtrller &at(const string &name) {return acontrollers_.at(name);}

    inline bool empty() const noexcept {return acontrollers_.empty();}

    inline auto begin() noexcept {return acontrollers_.begin();}
    inline auto end()   noexcept {return acontrollers_.end();}
};

} // namespace rpr
namespace rpr {
RPR_USING;
using namespace mmd;

template<> void ACtrller::mmd_out<mmd_type::flow>  (ostream &os, bool with_header);
template<> void ACtrller::mmd_out<mmd_type::states>(ostream &os, bool with_header);

} // namespace rpr
#endif // ACTRLLER_H