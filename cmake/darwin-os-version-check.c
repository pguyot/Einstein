#ifndef _DARWIN_C_SOURCE
#define _DARWIN_C_SOURCE
#endif

#include <stdint.h>
#include <stdlib.h>
#include <sys/sysctl.h>

static int32_t parse_darwin_product_version(uint32_t *out_major,
                                            uint32_t *out_minor,
                                            uint32_t *out_subminor) {
    char buf[64];
    size_t len = sizeof(buf) - 1;
    if (sysctlbyname("kern.osproductversion", buf, &len, NULL, 0) != 0 || len == 0) {
        return 0;
    }
    buf[len] = '\0';

    char *p = buf;
    *out_major = (uint32_t)strtoul(p, &p, 10);
    *out_minor = (*p == '.') ? (uint32_t)strtoul(p + 1, &p, 10) : 0;
    *out_subminor = (*p == '.') ? (uint32_t)strtoul(p + 1, &p, 10) : 0;
    return 1;
}

int32_t __isOSVersionAtLeast(int32_t major, int32_t minor, int32_t subminor) {
    static uint32_t cur_major = 0;
    static uint32_t cur_minor = 0;
    static uint32_t cur_subminor = 0;
    static int initialized = 0;
    if (!initialized) {
        if (!parse_darwin_product_version(&cur_major, &cur_minor, &cur_subminor)) {
            return 0;
        }
        initialized = 1;
    }
    if (major < 0 || minor < 0 || subminor < 0) {
        return 0;
    }
    uint32_t req_major = (uint32_t)major;
    uint32_t req_minor = (uint32_t)minor;
    uint32_t req_subminor = (uint32_t)subminor;
    if (cur_major != req_major) {
        return cur_major > req_major;
    }
    if (cur_minor != req_minor) {
        return cur_minor > req_minor;
    }
    return cur_subminor >= req_subminor;
}

int32_t __isPlatformVersionAtLeast(uint32_t platform,
                                   uint32_t major,
                                   uint32_t minor,
                                   uint32_t subminor) {
    (void)platform;
    return __isOSVersionAtLeast((int32_t)major, (int32_t)minor, (int32_t)subminor);
}
