#ifndef EXEARGS_H
#define EXEARGS_H
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct args {
    int argc;
    const char **argv;
    const char opt_char;
} args;

typedef struct args_span {
    const char **ptr;
    int len;
} args_span;

/// @param max_count: (-1) for any, (0) for prefix match, (>0) for partial match
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

void set_accessed(bool *dest, args args, args_span span, bool value) {
    for (int i = 0; i < span.len && span.ptr[i]; ++i) dest[i] = value;
}
void reverse_accessed(bool *accessed, int n) {
    for (int i = 0; i < n; ++i) accessed[i] = !accessed[i];
}

int get_accessed(char *dest[], args args, const bool *accessed) {
    char **p = dest;
    for (int i = 0; i < args.argc; ++i) if (accessed[i]) *p++ = args.argv[i];
    return p - dest;
}

#endif