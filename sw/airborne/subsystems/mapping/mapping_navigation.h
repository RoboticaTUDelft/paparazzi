#ifndef MAPPING_NAVIGATION_H
#define MAPPING_NAVIGATION_H


extern volatile int mapping_frame_available;

#define MappingControlEvent(_callback) { \
  if(mapping_frame_available) {          \
    _callback();                         \
    mapping_frame_available = 0;     \
  }                                      \
}

extern void mapping_navigation_init(void);

/* rotorcraft moves forward and maintains the altitude */
extern void mapping_navigation_forward(void);
/* rotorcraft enters AP_MODE_HOVER_Z_HOLD and tries to maintain position and altitude */
extern void mapping_navigation_stop(void);
/* rotorcraft yaws to the right while hovering, maintaining position and altitude */
extern void mapping_navigation_yaw_right(void);
/* rotorcraft yaws to the left while hovering, maintaining position and altitude */
extern void mapping_navigation_yaw_left(void);
/* rotorcraft maintains AP_MODE but does change it's attitude */
extern void mapping_navigation_neutral(void);


#endif /* MAPPING_NAVIGATION_H */
