#include "lib_implementation.h"
#include "commands.h"

#if HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdio.h>
#include <string.h>
#ifdef HAVE_LIBUSB_1_0
#include <libusb.h>
#else
#include <usb.h>
#endif

#include "config.h"
#include "dfu-device.h"
#include "dfu.h"
#include "atmel.h"
#include "arguments.h"
#include "commands.h"

#ifdef _DEBUG
int debug = 1000;
#else
int debug = 0;
#endif

#ifdef HAVE_LIBUSB_1_0
libusb_context *usbcontext;
#endif

dfu_device_t dfu_device;
#ifdef HAVE_LIBUSB_1_0
  struct libusb_device *device = NULL;
#else
  struct usb_device *device = NULL;
#endif

void set_debug(const int verbosity){
  debug = verbosity;
  if( debug >= 200 ) {
#ifdef HAVE_LIBUSB_1_0
    libusb_set_debug(usbcontext, debug );
#else
    usb_set_debug( debug );
#endif
  }
}

int init_device(const uint32_t vendor,
                 const uint32_t product,
                 const uint32_t bus_number,
                 const uint32_t device_address,
                 dfu_device_t *dfu_device,
                 const dfu_bool initial_abort,
                 const dfu_bool honor_interfaceclass){
  device = dfu_device_init( vendor, product,
                            bus_number, device_address,
                            dfu_device,
                            initial_abort,
                            honor_interfaceclass);
  return device != NULL;
}

int init_short_usbaddr(const uint32_t bus_number,
               const uint32_t device_address){
  return init_device(
      0x03eb, 0x2FF1, bus_number, device_address, &dfu_device,
      0, 0
  );
}

int init_short(){
  return init_device(0x03eb, 0x2FF1, 0, 0, &dfu_device, 0, 0);
}

int is_device_open(){
  return dfu_device.handle == NULL ? 0 : 1;
}

int close_device(){
  if (!is_device_open()) return 0;

  int rv;
#ifdef HAVE_LIBUSB_1_0
  rv = libusb_release_interface( dfu_device.handle, dfu_device.interface );
#else
  rv = usb_release_interface( dfu_device.handle, dfu_device.interface );
#endif

  if( is_device_open() ) {
#ifdef HAVE_LIBUSB_1_0
    libusb_close(dfu_device.handle);
#else
    if( 0 != usb_close(dfu_device.handle) ) {
            fprintf( stderr, "%s: failed to close the handle.\n", progname );
            retval = DEVICE_ACCESS_ERROR;
        }
#endif
  }

  return rv;
}

struct programmer_arguments args;


void pre_command_launch();

void init_args(char* argv[], size_t argc){
  memset( &args, 0, sizeof(args) );
  parse_arguments(&args, argc, argv);
}

void init_usb(){
  memset( &dfu_device, 0, sizeof(dfu_device) );

#ifdef HAVE_LIBUSB_1_0
  if (libusb_init(&usbcontext)) {
    fprintf( stderr, "%s: can't init libusb.\n", "lib" );
    return; // DEVICE_ACCESS_ERROR;
  }
#else
  usb_init();
#endif

  set_debug(debug);
}

int deinit(){
#ifdef HAVE_LIBUSB_1_0
  libusb_exit(usbcontext);
#endif
}

int init(){
  init_usb();

  if(!init_short()){
    printf("Error in init - check device access privileges. Run with sudo?\n");
    close_device();
    return 1;
  }
  return 0;
}

int32_t launch(){
  if (!is_device_open()) return -1;

  char *argv[] = { strdup("\0"),
                   strdup("at32uc3a3256s"),
                   strdup("launch")
  };
  init_args(argv, 3);

  pre_command_launch();
  int32_t ret = execute_launch(&dfu_device, &args);
  if (ret != 0) return ret;

  return RET_SUCCESS;
}


int32_t erase(){
  if (!is_device_open()) return -1;

  char *argv[] = {
      strdup("\0"),
      strdup("at32uc3a3256s"),
      strdup("erase"),
  };
  init_args(argv, 3);

  pre_command_launch();
  int32_t ret = execute_erase(&dfu_device, &args);
  if (ret != 0) return ret;

  return RET_SUCCESS;
}

#include <unistd.h>
#include <stdbool.h>

int32_t flash(const char *firmware_path) {
  if (!is_device_open()) return -1;

  char *argv[] = {
                  strdup("\0"),
                  strdup("at32uc3a3256s"),
                  strdup("flash"),
                  strdup("--suppress-bootloader-mem"),
                  strdup(firmware_path),
  };
  init_args(argv, 5);

  const bool firmware_file_exists = access(args.com_flash_data.file, F_OK ) != -1;
  if(!firmware_file_exists) {
    printf("Firmware file does not exist!\n");
    return -2;
  }

  pre_command_launch();
  int32_t ret = execute_flash(&dfu_device, &args);
  if (ret != 0) return ret;

  return RET_SUCCESS;
}

void pre_command_launch() { dfu_device.type = args.device_type; }