
/**
 * 1. Make it work as a library ***
 * 2. Make it work on libusbX
 * 3. Compile libusbX statically
 * 4. Compile it cross-platform for Windows
 * 5. Compile it on macOS
 */

#include "src/lib_implementation.h"

int main(int argc, char** argv){

  if (argc < 2){
    return 1;
  }

  if(init())
    return -1;

  switch (argv[1][0]){
    case 'l': launch(); break;
    case 'e': erase(); break;
    case 'f':{
      if (argc == 3)
        flash(argv[2]);
      else
        return 2;
    }
      break;
    default: break;
  }
  close_device();
  deinit();

  return 0;
}