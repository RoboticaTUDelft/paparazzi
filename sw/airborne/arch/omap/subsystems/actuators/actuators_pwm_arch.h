/*
 * Copyright (C) 2012 Dino Hensen <dino.hensen@gmail.com>
 *
 * This file is part of paparazzi.
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef ACTUATORS_PWM_ARCH_H_
#define ACTUATORS_PWM_ARCH_H_

#include <stdint.h>
#include "subsystems/actuators/actuators_pwm.h"

#ifndef ACTUATORS_PWM_NB
#define ACTUATORS_PWM_NB 4
#endif

#define SERVOS_TICS_OF_USEC(_v) (_v)

#define ActuatorPwmSet(_i, _v) { actuators_pwm_values[_i] = _v; }
#define ActuatorsPwmCommit() actuators_pwm_commit();

uint16_t actuators_pwm_values[ACTUATORS_PWM_NB];

//extern void actuators_pwm_arch_init(void);
void actuators_pwm_commit(void);

#endif /* ACTUATORS_PWM_ARCH_H_ */