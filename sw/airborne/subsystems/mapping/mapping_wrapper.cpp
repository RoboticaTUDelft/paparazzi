/*
 * mapping_wrapper.cpp
 *
 *  Created on: 17 jan. 2013
 *      Author: kaj
 */

//#include "subsystems/mapping/mapping_wrapper.h"
#include "subsystems/mapping/Mapping.h"

extern "C" {
#include "subsystems/mapping.h"
}

Mapping * map;

extern "C" void mapping_init(void) {
#ifdef VEHICLE_WIDTH
	map = new Mapping (VEHICLE_WIDTH / 4);
#else
	map = new Mapping (0.1);
#endif
}

extern "C" void mapping_periodic(void) {
	map->mappingStep();
}
