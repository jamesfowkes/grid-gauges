#ifndef _ELEXON_H_
#define _ELEXON_H_

void elexon_setup();
void elexon_loop();
void elexon_set_api_key(char * key);
void elexon_download();
void elexon_print();

void elexon_update(GridPower& power);

#endif

