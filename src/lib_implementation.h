#ifndef DFU_PROGRAMMER_LIB_IMPLEMENTATION_H
#define DFU_PROGRAMMER_LIB_IMPLEMENTATION_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RET_SUCCESS (0)

int init();
int close_device();
int is_device_open();
int deinit();
void set_debug(int verbosity);

int32_t erase();
int32_t flash(const char *firmware_path);
int32_t launch();

#ifdef __cplusplus
}
#endif

#endif //DFU_PROGRAMMER_LIB_IMPLEMENTATION_H
