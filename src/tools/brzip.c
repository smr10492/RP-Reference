#include "exeargs.h"
#include "pipeio.h"

const char *const RPR_CMD = "rpr.exe -tool=br_zip";

int main(int argc, const char *argv[]) {
    args_span span;
    args args = {argc, argv, '-'};
    if (argc < 2) {puts(
        "Archive all files to <dir: where the first file is>.brarchive\n"
        "Usage: brzip [-thin] <infiles...>"
    ); return 0;}

    span = args_find(args, "-thin", -1);

    FILE *pipe = popen(RPR_CMD, "w");
    if (!pipe) {fputs("failed to open rpr.exe pipe", stderr); return 2;}
    pbegin_opts     (pipe);
    pprint_boolopt  (pipe, "with_data", !span.ptr);
    pprint_arrstropt(pipe, "infiles_br", argv + (span.ptr? 2 : 1), argv + argc);
    pend_opts       (pipe);
    fflush(pipe);

    return pclose(pipe);
}