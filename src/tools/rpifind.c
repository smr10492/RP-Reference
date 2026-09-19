#include "exeargs.h"
#include "pipeio.h"

const char *const RPR_CMD = "rpr.exe -tool=rpi_find";

int main(int argc, const char *argv[]) {
    if (argc < 2) {puts(
        "Get all IDs of components to a map(id -> file)\n"
        "Usage: rpifind <infile_rpi> [keys...]"
    ); return 0;}
    if (argc == 3) return 0;

    FILE *pipe = popen(RPR_CMD, "w");
    if (!pipe) {fputs("failed to open rpr.exe pipe", stderr); return 2;}
    pbegin_opts     (pipe);
    pprint_stropt   (pipe, "infile_rpi", argv[1]);
    pprint_arrstropt(pipe, "keys", argv + 2, argv + argc);
    pend_opts       (pipe);
    fflush(pipe);

    return pclose(pipe);
}