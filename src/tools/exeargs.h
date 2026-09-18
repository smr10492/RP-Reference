#ifndef EXEARGS_H
#define EXEARGS_H
#include <string.h>

typedef struct args {
    int argc;
    const char **argv;
    const char opt_char;
} args;

typedef struct args_span {
    const char **ptr;
    int len;
} args_span;

/// @brief 查找指定的参数并返回参数区域
/// @param key 参数名称，NULL表示匹配程序名（此时max_count无效）
/// @param max_count 最大匹配长度，-1表示完全匹配
/// @return 参数区域
args_span args_find(args args, const char *key, int max_count) {
    const char **p_begin = NULL;
    const char **p_end   = NULL;
    if (!key) p_begin = args.argv;
    else for (int i = 1; i < args.argc; ++i) switch (max_count) {
        case -1: if (0 == strcmp (key, args.argv[i]))              {p_begin = &args.argv[i]; break;}
        case  0: if (0 == strncmp(key, args.argv[i], strlen(key))) {p_begin = &args.argv[i]; break;}
        default: if (0 == strncmp(key, args.argv[i], max_count))   {p_begin = &args.argv[i]; break;}
    }
    if (!p_begin) return (args_span){NULL, 0};
    for (const char **p = p_begin + 1; p < args.argv + args.argc; ++p) if ((*p)[0] == args.opt_char) {
        p_end = p;
        break;
    }
    if (!p_end) p_end = args.argv + args.argc;
    return (args_span){p_begin, p_end - p_begin};
}

#endif