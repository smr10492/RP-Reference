#ifndef PIPEIO_H
#define PIPEIO_H
#include <stdio.h>
#include <stdbool.h>

static bool first_opt = true;

static int json_fputs(FILE *fout, const char *s) {
    if (!s || !fout) return EOF;

    const unsigned char *p = (const unsigned char*)s;
    int count = 0;
    int c;

    while ((c = *p++) != '\0') {
        const char *escape = NULL;
        char unicode_buf[7];

        switch (c) {
            case '"':  escape = "\\\""; break;
            case '\\': escape = "\\\\"; break;
            case '\b': escape = "\\b";  break;
            case '\f': escape = "\\f";  break;
            case '\n': escape = "\\n";  break;
            case '\r': escape = "\\r";  break;
            case '\t': escape = "\\t";  break;
            default: if (c < 0x20) {
                snprintf(unicode_buf, sizeof(unicode_buf), "\\u%04x", c);
                escape = unicode_buf;
            }
        }

        if (escape) {
            fputs(escape, fout);
            count += strlen(escape);
        }
        else {
            fputc(c, fout);
            ++count;
        }
    }
    return count;
}

void pbegin_opts(FILE *fout) {
    fputc('{', fout);
    first_opt = true;
}

void pprint_stropt(FILE *fout, const char *key, const char *val) {
    if (!first_opt) fputc(',', fout);
    //fprintf(fout, "\"%s\":\"%s\"", key, val);
    fprintf(fout, "\"%s\":\"", key);
    json_fputs(fout, val);
    fputc('\"', fout);
    first_opt = false;
}

void pprint_boolopt(FILE *fout, const char *key, bool val) {
    if (!first_opt) fputc(',', fout);
    //fprintf(fout, "\"%s\":\"%s\"", key, val);
    fprintf(fout, "\"%s\":", key);
    json_fputs(fout, val ? "true" : "false");
    first_opt = false;
}

void pprint_arrstropt(FILE *fout, const char *key, const char **begin, const char **end) {
    if (!first_opt) fputc(',', fout);
    fprintf(fout, "\"%s\":[", key);
    if (begin >= end) {fputc(']', fout); return;}
    //fprintf(fout, "\"%s\"", *begin);
    fputc('\"', fout);
    json_fputs(fout, *begin);
    fputc('\"', fout);
    for (const char **vals = begin + 1; vals < end; ++vals) {
        // fprintf(fout, ",\"%s\"", *vals);
        fputs(",\"", fout);
        json_fputs(fout, *vals);
        fputc('\"', fout);
    }
    fputc(']', fout);
    first_opt = false;
}

void pend_opts(FILE *fout) {
    fprintf(fout, "}\n");
    first_opt = false;
}

#endif