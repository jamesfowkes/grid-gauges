#ifndef _NTP_H_
#define _NTP_H_

void ntp_setup();
void ntp_loop();
unsigned long ntp_get_time();
bool ntp_got_valid_time();

#endif
