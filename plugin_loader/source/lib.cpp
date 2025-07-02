#include <stdio.h>
#include <string.h>
#include <sstream>
#include "../../common/entry.h"

#include "../../common/plugin_common.h"
#include "../../common/notify.h"
#include <stdint.h>

attr_public const char* g_pluginName = "plugin_loader";
attr_public const char* g_pluginDesc = "Plugin loader.";
attr_public const char* g_pluginAuth = "illusiony";
attr_public uint32_t g_pluginVersion = 0x00000100;  // 1.00

extern "C" int plugin_load(SceEntry* args, const void* atexit_handler)
{
    // https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain/blob/63c0be5ffff09fbaebebc6b9a738d150e2da0205/samples/library_example/library_example/lib.cpp
    // Just a copy of example for nowAdd commentMore actions
    std::ostringstream os;
    os << "Hi I'm from the library!\n";
    printf("%s\n", os.str().c_str());
    return 0;
    Notify(TEX_ICON_SYSTEM, os.str().c_str());
    return 0;
}
