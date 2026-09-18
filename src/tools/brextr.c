#include "exeargs.h"
#include "pipeio.h"

const char *const RPR_CMD = "rpr.exe -tool=br_extract";

int main(int argc, const char *argv[]) {
    if (argc < 2) {puts(
        "Extract each *.brarchive file into a folder with the same name as the file\n"
        "Usage: brextr <infiles...>"
    ); return 0;}

    FILE *pipe = popen(RPR_CMD, "w");
    if (!pipe) {fputs("failed to open rpr.exe pipe", stderr); return 2;}
    pbegin_opts     (pipe);
    pprint_arrstropt(pipe, "infiles", argv + 1, argv + argc);
    pend_opts       (pipe);
    fflush(pipe);

    return pclose(pipe);
}