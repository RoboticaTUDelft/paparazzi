#include "subsystems/mapping.h"
#include "subsystems/mapping/mapping_test.h"
#include "mcu_periph/sys_time_arch.h"

int mapping_timer;

void mapping_init(void) {
  mapping_navigation_init();
  SysTimeTimerStart(mapping_timer);
}

void mapping_periodic (void) {
  int curTime = SEC_OF_CPU_TICKS(SysTimeTimer(mapping_timer));
  if(curTime < 10)
    mapping_navigation_yaw_left();
  else
    mapping_navigation_neutral();
}
