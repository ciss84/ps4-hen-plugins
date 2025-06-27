#include "git_ver.h"
#include "../extern/GoldHEN/include/Common.h"

#define GOLDHEN_PATH "/data/GoldHEN"

#define MAX_PATH_ 260

#define attr_module_hidden __attribute__((weak)) __attribute__((visibility("hidden")))
#define attr_public __attribute__((visibility("default")))

#if (__FINAL__) == 1
#define BUILD_TYPE "(Release)"
#define debug_printf(a, args...)
#else
#define BUILD_TYPE "(Debug)"
#define debug_printf(a, args...) printf("[%s] (%s:%d) " a, __func__, __FILE__, __LINE__, ##args)
#endif

#define final_printf(a, args...) printf("(%s:%d) " a, __FILE__, __LINE__, ##args)
#define boot_ver() {\
    final_printf("Commit: %s Branch: %s Build: %i %s\n", GIT_COMMIT, GIT_VER, GIT_NUM, BUILD_TYPE);\
    final_printf("Built: %s\n", BUILD_DATE);\
    final_printf("GoldHEN SDK Ver: 0x%08x\n", GOLDHEN_SDK_VERSION);\
    final_printf("GoldHEN System SDK Ver: 0x%08x\n", sys_sdk_version());\
}

// Takes hardcoded input string 2 to strlen against during compile time.
// startsWith(input_1, "input 2");
#define startsWith(str1, str2) (strncmp(str1, str2, __builtin_strlen(str2)) == 0)
