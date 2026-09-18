#include "exeargs.h"
#include "pipeio.h"

const char *const RPR_CMD = "rpr.exe -tool=rpi_index";

int main(int argc, const char *argv[]) {
    args_span span;
    args args = {argc, argv, '-'};
    const char *rpdir_p   = NULL;
    const char *outfile_p = NULL;
    if (argc < 2) {puts(
        "Get all IDs of components to a map(id -> file)\n"
        "Usage: rpidx -rp <rp_dir> -o <outfile=ThisRP.rpi>"
    ); return 0;}

    span      = args_find(args, "-rp", -1);
    if (span.len < 2) {fputs("missing -rp <rp_dir>", stderr); return 1;}
    rpdir_p   = span.ptr[1];
    span      = args_find(args, "-o", -1);
    outfile_p = (span.len < 2)? "ThisRP.rpi" : span.ptr[1];

    FILE *pipe = popen(RPR_CMD, "w");
    if (!pipe) {fputs("failed to open rpr.exe pipe", stderr); return 2;}
    pbegin_opts  (pipe);
    pprint_stropt(pipe, "rp_dir",      rpdir_p);
    pprint_stropt(pipe, "outfile_rpi", outfile_p);
    pend_opts    (pipe);
    fflush(pipe);

    return pclose(pipe);
}