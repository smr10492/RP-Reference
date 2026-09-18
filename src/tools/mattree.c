#include "exeargs.h"
#include "pipeio.h"

const char *const RPR_CMD = "rpr.exe -tool=mat_forest";

int main(int argc, const char *argv[]) {
    if (argc < 2) {puts(
        "View material extensions forest of a material file\n"
        "Usage: mattree <infile>"
    ); return 0;}

    FILE *pipe = popen(RPR_CMD, "w");
    if (!pipe) {fputs("failed to open rpr.exe pipe", stderr); return 2;}
    pbegin_opts  (pipe);
    pprint_stropt(pipe, "infile", argv[1]);
    pend_opts    (pipe);
    fflush(pipe);

    return pclose(pipe);
}