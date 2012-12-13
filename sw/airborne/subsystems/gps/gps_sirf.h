#ifndef GPS_SIRF_H
#define GPS_SIRF_H

#include "mcu_periph/uart.h"
#include "sirf_protocol.h"
#include "mcu_periph/sys_time.h"
/* Check if pprz_geodetic_float.h is included */
/* Check if gps.h is included */

#define GPS_SIRF_MAX_PAYLOAD 1024
struct GpsSirf {
  bool_t msg_available;
  uint8_t msg_buf[GPS_SIRF_MAX_PAYLOAD] __attribute__ ((aligned));
  uint8_t msg_id;
  uint8_t msg_class;

  uint8_t status;
  uint16_t len;
  uint8_t msg_idx;
  uint8_t ck_a, ck_b;
  uint8_t error_cnt;
  uint8_t error_last;
  
  uint8_t have_geo;
  uint8_t have_nav;
};

/* Message ID 2 from GPS. Total payload length should be 41 bytes. */
struct GPS_MSG_2 {
	uint8_t msg_id; 		/* hex value 0x02 ( = decimal 2) */
	int32_t x_pos; 			/* x-position in m */
	int32_t y_pos; 			/* y-position in m */
	int32_t z_pos; 			/* z-position in m */
	int16_t vx; 			/* x-velocity * 8 in m/s */
	int16_t vy; 			/* y-velocity * 8 in m/s */
	int16_t vz; 			/* z-velocity * 8 in m/s */
	uint8_t mode1;
	uint8_t hdop; 			/* horizontal dilusion of precision *5 (0.2 precision) */
	uint8_t mode2;
	uint16_t week;
	uint32_t tow; 			/* time of week in seconds * 10^2 */
	uint8_t num_sat; 		/* Number of satellites in fix */
	uint8_t ch1prn; 		/* pseudo-random noise, 12 channels */
	uint8_t ch2prn;
	uint8_t ch3prn;
	uint8_t ch4prn;
	uint8_t ch5prn;
	uint8_t ch6prn;
	uint8_t ch7prn;
	uint8_t ch8prn;
	uint8_t ch9prn;
	uint8_t ch10prn;
	uint8_t ch11prn;
	uint8_t ch12prn;
} __attribute__ ((packed));


/* Message ID 41 from GPS. Total payload length should be 91 bytes. */
struct GPS_MSG_41 {
	uint8_t msg_id; 			/* hex value 0x29 (= decimal 41) */
	uint16_t nav_valid; 		/* if equal to 0x0000, then navigation solution is valid */
	uint16_t nav_type;
	uint16_t extended_week_number;
	uint32_t tow; 				/* time of week  in seconds *10^3] */
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint16_t second;
	uint32_t sat_id; 			/* satellites used in solution. Each satellite corresponds with a bit, e.g. bit 1 ON = SV 1 is used in solution */
	int32_t latitude; 			/* in degrees (+= North) *10^7 */
	int32_t longitude; 			/* in degrees (+= East) *10*7 */
	int32_t alt_ellipsoid; 		/* in meters *10^2 */
	int32_t alt_msl; 			/* in meters *10^2 */
	int8_t map_datum;
	uint16_t sog; 				/* speed over ground, in m/s * 10^2 */
	uint16_t cog; 				/* course over ground, in degrees clockwise from true north * 10^2 */
	int16_t mag_var; 			/* not implemented */
	int16_t climb_rate;			/* in m/s * 10^2 */
	int16_t heading_rate; 		/* in deg/s * 10^2 */
	uint32_t ehpe; 				/* estimated horizontal position error, in meters * 10^2 */
	uint32_t evpe; 				/* estimated vertical position error, in meters * 10^2 */
	uint32_t ete; 				/* estimated time error, in seconds * 10^2 */
	uint16_t ehve; 				/* estimated horizontal velocity error in m/s * 10^2 */
	int32_t clock_bias; 		/* in m * 10^2 */
	uint32_t clock_bias_err; 	/* in m * 10^2 */
	int32_t clock_drift; 		/* in m/s * 10^2 */
	uint32_t clock_drift_err; 	/* in m/s * 10^2 */
	uint32_t distance; 			/* Distance traveled since reset in m */
	uint16_t distance_err; 		/* in meters */
	uint16_t heading_err; 		/* in degrees * 10^2 */
	uint8_t num_sat; 			/* Number of satellites used for solution */
	uint8_t hdop; 				/* Horizontal dilution of precision x 5 (0.2 precision) */
	uint8_t add_info; 			/* Additional mode info */
} __attribute__ ((packed));


void parse_msg41(struct GPS_MSG_41* p) {
	gps.tow = p->tow;
	gps.hmsl = p->alt_msl*10;
	gps.num_sv = p->num_sat;
	gps.nb_channels = p ->num_sat;

	/* read latitude, longitude and altitude from packet */
	gps.lla_pos.lat = RadOfDeg(p->latitude);
	gps.lla_pos.lon = RadOfDeg(p->longitude);
	gps.lla_pos.alt = p->alt_ellipsoid * 10;

	#if GPS_USE_LATLONG
	  /* convert to utm */
	  struct UtmCoor_f utm_f;
	  utm_f.zone = nav_utm_zone0;
	  utm_of_lla_f(&utm_f, &lla_pos);

	  /* copy results of utm conversion */
	  gps.utm_pos.east = utm_f.east*100;
	  gps.utm_pos.north = utm_f.north*100;
	  gps.utm_pos.alt = gps.lla_pos.alt;
	  gps.utm_pos.zone = nav_utm_zone0;
	#endif

	gps.sacc = p->ehve;
	gps.course = RadOfDeg(p->cog)*10^5;
	gps.gspeed = RadOfDeg(p->sog)*10^5;
	gps.cacc = RadOfDeg(p->heading_err)*10^5;
	gps.pacc = p->ehpe;
	gps.pdop = p->hdop * 20;

	if (p->nav_type & 0x7 >= 0x4)
		gps.fix = GPS_FIX_3D;
	else if (p->nav_type & 0x7 >= 0x3)
		gps.fix = GPS_FIX_2D;
	else
		gps.fix = GPS_FIX_NONE;

}

void parse_msg2(struct GPS_MSG_2* p) {
	gps.week = p->week;

	gps.ecef_pos.x = p->x_pos * 100;
	gps.ecef_pos.y = p->y_pos * 100;
	gps.ecef_pos.z = p->z_pos * 100;

	gps.ecef_vel.x = p->vx*100/8;
	gps.ecef_vel.y = p->vy*100/8;
	gps.ecef_vel.z = p->vz*100/8;

	struct EnuCoor_f enu; /* speed NED in cm/s */
	enu_of_ecef_point_f(&enu, def, ecef);
	ENU_OF_TO_NED(*ned, enu);
}


extern struct GpsSirf gps_sirf;  

extern void gps_sirf_read_message(void);
extern void gps_sirf_parse(uint8_t c);

/*
 * This part is used by the autopilot to read data from a uart
 */
#define __GpsLink(dev, _x) dev##_x
#define _GpsLink(dev, _x)  __GpsLink(dev, _x)
#define GpsLink(_x) _GpsLink(GPS_LINK, _x)

#define GpsBuffer() GpsLink(ChAvailable())

#define GpsEvent(_gps_uptodate_callback) {        \
  if (GpsBuffer()) {                              \
    ReadGpsBuffer();                              \
  }                                               \
  if (gps_ubx.msg_available) {                    \
    gps_sirf_read_message();                      \
    if ((gps_sirf.msg_id == SIRF_GEO_ID ||        \
      gps_sirf.msg_id == SIRF_NAV_ID) &&          \
      gps_sirf.have_geo && gps_sirf.have_nav  ) { \
      if (gps.fix == GPS_FIX_3D) {                \
        gps.last_fix_ticks = sys_time.nb_sec_rem; \
        gps.last_fix_time = sys_time.nb_sec;      \
      }                                           \
      uint8_t have_geo = 0;                       \
      uint8_t have_nav = 0;                       \
      _gps_uptodate_callback();                   \
    }                                             \
    gps_sirf.msg_available = FALSE;               \
  }                                               \
}

#define ReadGpsBuffer() {					                \
  while (GpsLink(ChAvailable())&&!gps_sirf.msg_available)	\
    gps_sirf_parse(GpsLink(Getch()));			            \
}

