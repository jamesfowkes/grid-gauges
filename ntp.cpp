#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TaskAction.h>

static const unsigned long START_OF_2018_EPOCH_TIME = 1514764800UL;

static WiFiUDP s_ntp_UDP;
static NTPClient s_time_client(s_ntp_UDP);
static bool s_ntp_time_ok = false;

static void ntp_debug_task_fn(TaskAction* pTask)
{
	(void)pTask;
	Serial.print("NTP time:");
	Serial.print(s_time_client.getFormattedTime());
	Serial.println(s_ntp_time_ok ? " (OK)" : " (not OK)");
}
static TaskAction s_ntp_debug_task(ntp_debug_task_fn, 5000, INFINITE_TICKS);

void ntp_setup()
{
    s_time_client.begin();
}

void ntp_loop()
{
	s_ntp_time_ok = s_time_client.getEpochTime() > START_OF_2018_EPOCH_TIME;
	if (!s_ntp_time_ok)
	{
    	s_time_client.update();
	}
    s_ntp_debug_task.tick();
}

unsigned long ntp_get_time()
{
	return s_time_client.getEpochTime();
}

bool ntp_got_valid_time()
{
	return s_ntp_time_ok;
}
