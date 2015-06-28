
#include "monita.h"

#ifndef __APP_RPM__
#define __APP_RPM__

//unsigned int giliran;
unsigned int data_putaran[JUM_DIGITAL];
unsigned int data_hit[JUM_DIGITAL];

void set_konter_rpm (int st, unsigned int period);
void set_konter_onoff (int i, int onoff);
void reset_konter(void);
void hitung_rpm(void);
void data_frek_rpm (void);
void hitung_running_hours();


#endif
