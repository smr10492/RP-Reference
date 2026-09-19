#include "exeargs.h"
#include "pipeio.h"

const char *const RPR_CMD = "rpr.exe -tool=geo_tree";

int main(int argc, const char *argv[]) {
    if (argc < 3) {puts(
        "Get all IDs of components to a map(id -> file)\n"
        "Usage: geotree <infile_rpi> <id_geo>"
    ); return 0;}

    FILE *pipe = popen(RPR_CMD, "w");
    if (!pipe) {fputs("failed to open rpr.exe pipe", stderr); return 2;}
    pbegin_opts  (pipe);
    pprint_stropt(pipe, "infile_rpi", argv[1]);
    pprint_stropt(pipe, "id_geo",     argv[2]);
    pend_opts    (pipe);
    fflush(pipe);

    return pclose(pipe);
}