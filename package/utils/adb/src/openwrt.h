/* SPDX-License-Identifier: Apache-2.0 */
#ifndef OPENWRT_ADBD_COMPAT_H
#define OPENWRT_ADBD_COMPAT_H

#include <errno.h>
#include <sys/types.h>

#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS
#define __END_DECLS
#endif

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) \
    __extension__ ({ __typeof__(expression) result; \
        do { result = (expression); } while (result == -1 && errno == EINTR); \
        result; })
#endif

#endif
