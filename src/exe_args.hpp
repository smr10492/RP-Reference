#ifndef EXE_ARGS_HPP
#define EXE_ARGS_HPP
#include <span>
#include <cstring>
#include <algorithm>
#include <vector>
#include <concepts>
#include "template_inst.h"

/// @brief Parse executable arguments
/// @param opt_char_ Option character, default '-' (does not add to lookup key).
class ExeArgs {
public:
    typedef std::span<const char*> arg_t;
    typedef std::vector<const char*> mergedarg_t;

    template<typename Tp>
    constexpr void set_accessed(const Tp &accessed, bool value = true) noexcept
    requires (std::same_as<Tp, arg_t> || std::same_as<Tp, mergedarg_t>) {
        for (const auto &arg : accessed) accessed_[&arg - argv_] = value;
    }

private:
    const int    argc_ = 0;
    const char **argv_ = nullptr;
    char opt_char_     = '-';
    std::vector<bool> accessed_ = decltype(accessed_)(argc_, false);

public:
    ExeArgs() = delete;
    constexpr explicit ExeArgs(int argc, const char **argv, char opt_char = '-') noexcept:
        argc_(argc),
        argv_(argv),
        opt_char_(opt_char),
        accessed_(decltype(accessed_)(argc_, false))
    {}

    constexpr ExeArgs &set_opt_char(char opt_char) noexcept {
        opt_char_ = opt_char;
        return *this;
    }
    constexpr char opt_char() const noexcept {
        return opt_char_;
    }

    /// @brief Find specified argument
    /// @param key Argument name, default nullptr (match program name, max_count is invalid)
    /// @param max_count Maximum match length, default -1 (complete match)
    /// @return Argument region
    constexpr arg_t find(const char *key = nullptr, int max_count = -1) {
        const char **p_begin = ((!key)? argv_ : std::find_if(
            (argv_ + 1), (argv_ + argc_), [key, max_count](const char *arg) {
                if      (max_count == -1) return (0 == strcmp (key, arg));
                else if (max_count ==  0) return (0 == strncmp(key, arg, strlen(key)));
                else                      return (0 == strncmp(key, arg, max_count));
            }
        ));
        if (p_begin == (argv_ + argc_)) return arg_t();
        const char **p_end = std::find_if((p_begin + 1), (argv_ + argc_), [this](const char *arg) -> bool {
            return arg[0] == opt_char_;
        });

        auto ret = arg_t(p_begin, p_end);
        this->set_accessed(ret);
        return ret;
    }
    /// @brief Find multiple arguments
    /// @param key Argument name, default nullptr (match program name, max_count is invalid)
    /// @param max_count Maximum match length, default -1 (complete match)
    /// @return Merged argument group (only keep one argument head)
    constexpr mergedarg_t find_multi(const char *key = nullptr, int max_count = -1) {
        mergedarg_t ret = {};

        if (!key) {
            auto arg = this->find(key, max_count);
            if (arg.empty()) return ret;
            ret.insert(ret.end(), arg.begin(), arg.end());
            return ret;
        }

        for (int i = 1; i < argc_; ++i) {
            static bool key_pushed = false;
            if ([key, max_count](const char *arg) {
                if      (max_count == -1) return (0 == strcmp (key, arg));
                else if (max_count ==  0) return (0 == strncmp(key, arg, strlen(key)));
                else                      return (0 == strncmp(key, arg, max_count));
            }(argv_[i])) {
                static bool skip_head = false;
                key_pushed = true;
                this->set_accessed(arg_t(&argv_[i], &argv_[i]));
                if (!skip_head) {
                    ret.emplace_back(argv_[i]);
                    skip_head = true;
                }
                else ++i;
            }
            if (argv_[i][0] == opt_char_) key_pushed = false;
            if (key_pushed) ret.emplace_back(argv_[i]);
        }

        this->set_accessed(ret);
        return ret;
    }
    constexpr mergedarg_t assessed_args() {
        mergedarg_t ret = {};
        for (int i = 0; i < argc_; ++i) if (accessed_[i]) {
            ret.emplace_back(argv_[i]);
        }
        return ret;
    }
    constexpr mergedarg_t rest_args() {
        mergedarg_t ret = {};
        for (int i = 0; i < argc_; ++i) if (!accessed_[i]) {
            ret.emplace_back(argv_[i]);
        }
        return ret;
    }
};

#endif // EXE_ARGS_HPP