/* SPDX-License-Identifier: Apache-2.0 */
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cutils/properties.h>

int __android_log_print(int priority, const char *tag, const char *fmt, ...)
{
    va_list args;
    int result;

    if (tag)
        fprintf(stderr, "%s: ", tag);
    va_start(args, fmt);
    result = vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    return result;
}

int property_get(const char *key, char *value, const char *default_value)
{
    const char *result = default_value ? default_value : "";

    if (!strcmp(key, "ro.adb.secure")) {
        const char *auth = getenv("ADBD_AUTH");
        result = auth ? auth : "0";
    } else if (!strcmp(key, "ro.product.name")) {
        result = "openwrt";
    } else if (!strcmp(key, "ro.product.model")) {
        result = "Galileo Gen 2";
    } else if (!strcmp(key, "ro.product.device")) {
        result = "galileo";
    }

    size_t len = strlen(result);
    if (len >= PROPERTY_VALUE_MAX)
        len = PROPERTY_VALUE_MAX - 1;
    memcpy(value, result, len);
    value[len] = '\0';
    return len;
}

int property_set(const char *key, const char *value)
{
    errno = ENOTSUP;
    return -1;
}

void adb_qemu_trace(const char *fmt, ...)
{
}
