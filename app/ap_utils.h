
#include "FreeRTOS.h"
#include <time.h>

#ifdef PAKAI_RTC
time_t now_to_time(int now, struct tm waktu);
void hitung_wkt(unsigned int w, int *wx);
int sync_waktu_modem(unsigned int x);

#endif
