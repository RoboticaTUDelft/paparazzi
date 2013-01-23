#include "subsystems/mapping/mapping_navigation.h"

#include "guidance/guidance_h.h"
#include "subsystems/radio_control.h"
#include "paparazzi.h"

volatile int mapping_frame_available;
pprz_t mapping_navigation_prev_mode;

void mapping_navigation_init(void) {
  mapping_frame_available = 0;
  mapping_navigation_prev_mode = 0;
}

void mapping_navigation_forward(void) {
  radio_control.values[RADIO_ROLL] = 0;
  /* negative pitch of 5 percent */
  radio_control.values[RADIO_PITCH] = MIN_PPRZ / 20;
  radio_control.values[RADIO_THROTTLE] = MAX_PPRZ;
  /* MODE_AUTO2 should be AP_MODE_ATTITUDE_Z_HOLD */
  radio_control.values[RADIO_MODE] = MAX_PPRZ;
  radio_control.values[RADIO_YAW] = 0;

  mapping_frame_available = 1;
  mapping_navigation_prev_mode = radio_control.values[RADIO_MODE];
}

void mapping_navigation_stop(void) {
  radio_control.values[RADIO_ROLL] = 0;
  /* negative pitch of 5 percent */
  radio_control.values[RADIO_PITCH] = 0;
  radio_control.values[RADIO_THROTTLE] = MAX_PPRZ;
  /* MODE_AUTO2 should be AP_MODE_HOVER_Z_HOLD */
  radio_control.values[RADIO_MODE] = 0;
  radio_control.values[RADIO_YAW] = 0;

  mapping_frame_available = 1;
  mapping_navigation_prev_mode = radio_control.values[RADIO_MODE];
}

void mapping_navigation_yaw_left(void) {
  radio_control.values[RADIO_ROLL] = 0;
  radio_control.values[RADIO_PITCH] = 0;
  radio_control.values[RADIO_THROTTLE] = MAX_PPRZ;
  /* MODE_AUTO1 should be AP_MODE_HOVER_Z_HOLD */
  radio_control.values[RADIO_MODE] = 0;
  radio_control.values[RADIO_YAW] = MAX_PPRZ;

  mapping_frame_available = 1;
  mapping_navigation_prev_mode = radio_control.values[RADIO_MODE];
}

void mapping_navigation_yaw_right(void) {
  radio_control.values[RADIO_ROLL] = 0;
  radio_control.values[RADIO_PITCH] = 0;
  radio_control.values[RADIO_THROTTLE] = MAX_PPRZ;
  /* MODE_AUTO1 should be AP_MODE_HOVER_Z_HOLD */
  radio_control.values[RADIO_MODE] = 0;
  radio_control.values[RADIO_YAW] = MIN_PPRZ;

  mapping_frame_available = 1;
  mapping_navigation_prev_mode = radio_control.values[RADIO_MODE];
}

void mapping_navigation_neutral(void) {
  radio_control.values[RADIO_ROLL] = 0;
  radio_control.values[RADIO_PITCH] = 0;
  radio_control.values[RADIO_THROTTLE] = 0;
  /* MODE_AUTO1 should be AP_MODE_HOVER_Z_HOLD */
  radio_control.values[RADIO_MODE] = mapping_navigation_prev_mode;
  radio_control.values[RADIO_YAW] = 0;

  mapping_frame_available = 1;
}
