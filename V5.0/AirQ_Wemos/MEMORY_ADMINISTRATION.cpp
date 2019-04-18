#include "MEMORY_ADMINISTRATION.h"
#include "configuration.h"
#include "user_interface.h"

int MEMORY_ADMINISTRATION::freeSpaceReportSerial()
{
      int freeSpace = system_get_free_heap_size();
      return freeSpace;
}
