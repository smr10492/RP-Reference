#ifndef COMMON_IMPL_HPP
#define COMMON_IMPL_HPP
#include "common.h"
#include <nlohmann/json.hpp>

inline std::string to_string(const nlohmann::json &obj) {
    using namespace std;
    using json = nlohmann::json;
    string ret = "";

    switch (obj.type()) {
        case json::value_t::string:          ret = obj.get<string>(); break;
        case json::value_t::boolean:         ret = obj.get<bool>()? "true" : "false"; break;
        case json::value_t::null:            ret = "null"; break;
        case json::value_t::number_integer:  ret = std::format("{}", obj.get<int64_t> ()); break;
        case json::value_t::number_unsigned: ret = std::format("{}", obj.get<uint64_t>()); break;
        case json::value_t::number_float:    ret = std::format("{}", obj.get<double>  ()); break;
        default:                             ret = obj.dump(); break;
    }

    return ret;
}

#endif // COMMON_IMPL_HPP