#ifndef COMMON_H
#define COMMON_H

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <random>
#include <chrono>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <nlohmann/json_fwd.hpp>

namespace mmd {} 

#define RPR_DECL \
    using namespace mmd; \
    namespace fs = std::filesystem; \
    using json = nlohmann::json; \
    using std::cout, std::clog, std::cerr; \
    using std::string; \
    using std::vector; \
    using std::move; \
    using ifstream      = std::ifstream; \
    using istream       = std::istream; \
    using ostream       = std::ostream; \
    using runtime_error = std::runtime_error

#include "rpr_base.hpp"

inline std::string to_string(const nlohmann::json &obj);

#include "template_inst.h"
#warning (visible when compiling the PCH or not using the PCH)
#endif // COMMON_H