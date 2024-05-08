/*
* RB_settings.h
*
* Created: 9/16/2015 11:42:07 AM
*  Author: Anurag Vaddi Reddy
*/


#ifndef RB_SETTINGS_H_
#define RB_SETTINGS_H_

#include "RB_800.h"

////////////////////////////////////////////////////////////////////////////

#define MAX_VACUUM_RAMPUP_TIME	12 //12*250ms = 3 Seconds maximum rampup time
#define MIN_VACUUM_RAMPUP_TIME	0 //0*250ms = 0 Seconds minimum rampup time
#define MAX_BRUSH_RAMPUP_TIME	12 //12*250ms = 3 Seconds maximum rampup time
#define MIN_BRUSH_RAMPUP_TIME	0 //0*250ms = 0 Seconds minimum rampup time
#define MAX_PRE_SWP_RAMPUP_TIME	12 //12*250ms = 3 Seconds maximum rampup time
#define MIN_PRE_SWP_RAMPUP_TIME	0 //0*250ms = 0 Seconds minimum rampup time

void initialize_RB_settings(void);
void set_to_default(void);

char A2_serial_num[A2_SERIAL_NUM_LEN + 1];

void update_vaccum_slow_start(void);
void update_brush_slow_start(void);
void update_pre_swp_slow_start(void);

#endif /* RB_SETTINGS_H_ */