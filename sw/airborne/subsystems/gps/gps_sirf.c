#include "subsystems/gps.h"
#include <math.h>  

/* currently needed to get nav_utm_zone0 */
#include "subsystems/navigation/common_nav.h"
#include "math/pprz_geodetic_float.h"

/* parser status */
#define UNINIT        0
#define GOT_SYNC1     1
#define GOT_SYNC2     2
#define GOT_LEN1      3
#define GOT_LEN2      4
#define GOT_ID        5
#define GOT_PAYLOAD   6
#define GOT_CHECKSUM1 7

/* last error type */
#define GPS_SIRF_ERR_NONE         0
#define GPS_SIRF_ERR_OVERRUN      1
#define GPS_SIRF_ERR_MSG_TOO_LONG 2
#define GPS_SIRF_ERR_CHECKSUM     3
#define GPS_SIRF_ERR_UNEXPECTED   4
#define GPS_SIRF_ERR_OUT_OF_SYNC  5

#define GpsUartSend1(c) GpsLink(Transmit(c))
#define GpsUartSetBaudrate(_a) GpsLink(SetBaudrate(_a))
#define GpsUartRunning GpsLink(TxRunning)
#define GpsUartSendMessage GpsLink(SendMessage)

#define SirfInitCheksum() { gps_sirf.send_ck_a = gps_sirf.send_ck_b = 0; }
#define UpdateChecksum(c) { gps_sirf.send_ck_a += c; gps_sirf.send_ck_b += gps_sirf.send_ck_a; }
#define SirfTrailer() { GpsUartSend1(gps_sirf.send_ck_a);  GpsUartSend1(gps_sirf.send_ck_b); GpsUartSendMessage(); }

#define SirfSend1(c) { uint8_t i8=c; GpsUartSend1(i8); UpdateChecksum(i8); }
#define SirfSend2(c) { uint16_t i16=c; SirfSend1(i16&0xff); SirfSend1(i16 >> 8); }
#define SirfSend1ByAddr(x) { SirfSend1(*x); }
#define SirfSend2ByAddr(x) { SirfSend1(*x); SirfSend1(*(x+1)); }
#define SirfSend4ByAddr(x) { SirfSend1(*x); SirfSend1(*(x+1)); SirfSend1(*(x+2)); SirfSend1(*(x+3)); }

#define SirfHeader(nav_id, msg_id, len) {        \
    GpsUartSend1(Sirf_SYNC1);                    \
    GpsUartSend1(Sirf_SYNC2);                    \
    SirfInitCheksum();                           \
    SirfSend1(nav_id);                           \
    SirfSend1(msg_id);                           \
    SirfSend2(len);                              \
  }

struct GpsSirf gps_sirf;

void gps_impl_init(void) {
   gps_sirf.status = UNINIT;
   gps_sirf.msg_available = FALSE;
   gps_sirf.error_cnt = 0;
   gps_sirf.error_last = GPS_SIRF_ERR_NONE;
   
   uint8_t have_geo = 0;
   uint8_t have_nav = 0;
}

/* reads the payload that was stored in msg_buf by the parse method */
void gps_sirf_read_message(void) {
  if(gps_sirf.msg_id == SIRF_NAV_ID)  {
    gps.tow        = SIRF_NAV_TOW(gps_sirf.msg_buf) * 10;
    gps.week       = SIRF_NAV_week(gps_sirf.msg_buf);
    gps.ecef_pos.x = SIRF_NAV_XPOS(gps_sirf.msg_buf) * 100;
    gps.ecef_pos.y = SIRF_NAV_YPOS(gps_sirf.msg_buf) * 100;
    gps.ecef_pos.z = SIRF_NAV_ZPOS(gps_sirf.msg_buf) * 100;
    gps.ecef_vel.x = SIRF_NAV_XVEL(gps_sirf.msg_buf) * 100 / 8;
    gps.ecef_vel.y = SIRF_NAV_YVEL(gps_sirf.msg_buf) * 100 / 8;
    gps.ecef_vel.z = SIRF_NAV_ZVEL(gps_sirf.msg_buf) * 100 / 8;
    gps.speed_3d   = sqrt(gps.ecef_vel.x * gps.ecef_vel.x + gps.ecef_vel.y * gps.ecef_vel.y + gps.ecef_vel.z * gps.ecef_vel.z);
    gps.num_sv     = SIRF_NAV_SV(gps_sirf.msg_buf);
    /* Fix isn't in the message so we derive it from the amount of SV's */
    if(gps.num_sv == 3) {
      gps.fix = GPS_FIX_2D;
    } else if(gps.num_sv > 3) {
      gps.fix = GPS_FIX_3D;
    } else {
      gps.fix = GPS_FIX_NONE;
    }
    /* No PDOP in the msg, so we use HDOP instead */
    gps.pdop       = SIRF_NAV_HDOP(gps_sirf.msg_buf)* 100 / 5;   
    uint8_t have_nav = 1;

  } else if(gps_sirf.msg_id == SIRF_GEO_ID) {
    gps.lla_pos.lat = RadOfDeg(SIRF_GEO_LAT(gps_sirf.msg_buf));
    gps.lla_pos.lon = RadOfDeg(SIRF_GEO_LON(gps_sirf.msg_buf));
    gps.lla_pos.alt = SIRF_GEO_ALT(gps_sirf.msg_buf) * 10;
    gps.hmsl        = SIRF_GEO_HMSL(gps_sirf.msg_buf) * 10;
    gps.pacc        = sqrt(SIRF_GEO_HPE(gps_sirf.msg_buf) * SIRF_GEO_HPE(gps_sirf.msg_buf) + SIRF_GEO_VPE(gps_sirf.msg_buf) * SIRF_GEO_VPE(gps_sirf.msg_buf));
    gps.gspeed      = SIRF_GEO_GSPEED(gps_sirf.msg_buf);
    gps.sacc        = SIRF_GEO_HVE(gps_sirf.msg_buf);
    gps.course      = RadOfDeg(SIRF_GEO_COURSE(gps_sirf.msg_buf)) * 100000;
    gps.num_sv      = SIRF_GEO_SV(gps_sirf.msg_buf);  
    /* Fix isn't in the message so we derive it from the amount of SV's */
    if(gps.num_sv == 3) {
      gps.fix = GPS_FIX_2D;
    } else if(gps.num_sv > 3) {
      gps.fix = GPS_FIX_3D;
    } else {
      gps.fix = GPS_FIX_NONE;
    }
    
#if GPS_USE_LATLONG
    /* Computes from (lat, long) in the referenced UTM zone */
    struct LlaCoor_f lla_f;
    lla_f.lat = ((float) gps.lla_pos.lat) / 1e7;
    lla_f.lon = ((float) gps.lla_pos.lon) / 1e7;
    struct UtmCoor_f utm_f;
    utm_f.zone = nav_utm_zone0;
    /* convert to utm */
    utm_of_lla_f(&utm_f, &lla_f);
    /* copy results of utm conversion */
    gps.utm_pos.east = utm_f.east*100;
    gps.utm_pos.north = utm_f.north*100;
    gps.utm_pos.alt = gps.lla_pos.alt;
    gps.utm_pos.zone = nav_utm_zone0;
#endif    
    uint8_t have_geo = 1;
  }
}
/****************************************************************************
 * fields not yet covered: nb_channels, svinfos, ned_vel and cacc *
 ****************************************************************************/
/* Sirf parsing */
void gps_sirf_parse( uint8_t c ) {
     if (gps_sirf.status < GOT_PAYLOAD) {
    gps_sirf.ck_a += c;
    gps_sirf.ck_b += gps_sirf.ck_a;
  }
  switch (gps_sirf.status) {
  /* try to read a start sequence */
  case UNINIT:
    if (c == SIRF_SYNC1)
      gps_sirf.status++;
    break;
  case GOT_SYNC1:
    if (c != SIRF_SYNC2) {
      gps_sirf.error_last = GPS_SIRF_ERR_OUT_OF_SYNC;
      goto error;
    }
    gps_sirf.ck_a = 0;
    gps_sirf.ck_b = 0;
    gps_sirf.status++;
    break;
  /* Start sequence has been received -> reads the length of the payLoad */
  case GOT_SYNC2:
    if (gps_sirf.msg_available) {
      /* Previous message has not yet been parsed: discard this one */
      gps_sirf.error_last = GPS_SIRF_ERR_OVERRUN;
      goto error;
    }
    gps_sirf.len = c;
    gps_sirf.status++;
    break;
  case GOT_LEN1:
    gps_sirf.len |= (c<<8);
    if (gps_sirf.len > GPS_SIRF_MAX_PAYLOAD) {
      gps_sirf.error_last = GPS_SIRF_ERR_MSG_TOO_LONG;
      goto error;
    }
    gps_sirf.status++;
    break;
  case GOT_LEN2:
    gps_sirf.msg_idx = 0;
    gps_sirf.msg_id = c;
    gps_sirf.status++;
    break;
  /* Length of payload received -> reads id of the message */
  case GOT_ID:
    gps_sirf.msg_buf[gps_sirf.msg_idx] = c;
    gps_sirf.msg_idx++;
    if (gps_sirf.msg_idx >= gps_sirf.len - 1) { //len - 1 because msg id is within the payload
      gps_sirf.status++;
    }
    break;
  /* payload received -> reads checksum */
  case GOT_PAYLOAD:
    if (c != gps_sirf.ck_a) {
      gps_sirf.error_last = GPS_SIRF_ERR_CHECKSUM;
      goto error;
    }
    gps_sirf.status++;
    break;
  case GOT_CHECKSUM1:
    if (c != gps_sirf.ck_b) {
      gps_sirf.error_last = GPS_SIRF_ERR_CHECKSUM;
      goto error;
    }
    gps_sirf.msg_available = TRUE;
    goto restart;
    break;
  default:
    gps_sirf.error_last = GPS_SIRF_ERR_UNEXPECTED;
    goto error;
  }
  return;
 error:
  gps_sirf.error_cnt++;
 restart:
  gps_sirf.status = UNINIT;
  return;
}
