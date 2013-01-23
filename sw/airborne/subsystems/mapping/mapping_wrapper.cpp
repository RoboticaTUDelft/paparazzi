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
	map = new Mapping (0.2);
}

extern "C" void mapping_periodic(void) {
	map->mappingStep();
}
