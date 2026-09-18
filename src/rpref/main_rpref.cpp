//  ____ ____ ____    | RPRef
// | __ | __ | __ |** | ResourcePack Reference
// |   _|  __|   _|** | version 1.0.0
// |_|__|_|**|_|__|ef | https://github.com/smr10492/RP-Reference

/********************\
|* main entry point *|
\********************/

#include "main_rpref_impl.h"
#define SW_NAME "rpr"
#define SW_VERSION "1.0.0"

static const char *const instructions =
    "Analyze & handle Minecraft Resource Packs.\n"
    "All tools are stateless, and each execution is independent.\n"
    "Most tools rely on the tool `rpi_index` (best to run it first).\n"
    "This is a C++ program, please do keep your(or the user's) computers safe.";

static const char *const usage =
    "Usage: " SW_NAME " -mcp\n"
    "       " SW_NAME " -tool=<tool_name>\n"
    "       " SW_NAME " -info=<tool_name>";

int main(int argc, char *argv[]) {
    using namespace std;
    ios::sync_with_stdio(false);

    if (argc < 2) {
        cerr << instructions << "\n\n";
        cerr << usage << "\n";
        return 2;
    }

    rprmcp::Server server("RP-Reference", SW_VERSION, instructions);
    main_reg_server(server);

    if (strcmp(argv[1], "-mcp") == 0) {
        server.run();
    }
    else if (strncmp(argv[1], "-tool=", 6) == 0) {
        string tool_name = &argv[1][6];
        server.oneshot(tool_name);
    }
    else if (strncmp(argv[1], "-info=", 6) == 0) {
        string tool_name = &argv[1][6];
        server.info(tool_name);
    }
    else {
        cerr << usage << "\n";
        return 2;
    }
    return 0;
}

#undef SW_NAME
#undef SW_VERSION
