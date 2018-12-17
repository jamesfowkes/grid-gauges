#ifndef _GRID_GAUGES_H_
#define _GRID_GAUGES_H_

typedef enum _eApplicationFlag
{
	eApplicationFlag_Print,
	eApplicationFlag_Download,
	eApplicationFlag_DownloadComplete,
	APPLICATION_FLAG_COUNT
} eApplicationFlag;

void application_set_flag(eApplicationFlag flag);
bool application_check_flag(eApplicationFlag flag);

#endif
