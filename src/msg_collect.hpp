#ifndef MSG_COLLECT_HPP
#define MSG_COLLECT_HPP
#include <string>
#include <vector>

class MsgCollect {
private:
    std::vector<std::string> msgs_ = {};

public:
    constexpr auto msgs() const noexcept {return msgs_;}
    constexpr bool empty()  const noexcept {return msgs_.empty();}
    constexpr void clear() noexcept {msgs_.clear();}
    constexpr MsgCollect &add(const std::string &msg) noexcept {
        msgs_.emplace_back(msg);
        return *this;
    }
    constexpr std::string string() const {
        if (msgs_.empty()) return "";
        if (msgs_.size() == 1) return msgs_[0];
        std::string ret = "";
        for (const auto &msg : msgs_) ret += (msg + ",\n");
        ret.resize(ret.size() - 2);
        return ret;
    }
};

#endif