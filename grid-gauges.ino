#include <Esp.h>
#include <TaskAction.h>

#include "app-wifi.h"
#include "user-input.h"
#include "serial-handler.h"
#include "elexon.h"
#include "ntp.h"

static void led_task_fn(TaskAction* pTask)
{
    (void)pTask;
    static bool s_led = false;
    digitalWrite(5, s_led = !s_led);
}
static TaskAction s_led_task(led_task_fn, 500, INFINITE_TICKS);

void setup()
{
    Serial.begin(115200);

    pinMode(5, OUTPUT);

    app_wifi_setup();
    user_input_setup();
    ntp_setup();
    elexon_setup();
}

void loop()
{
    user_input_loop();
    serial_loop();
    ntp_loop();
    elexon_loop();
    
    if (user_input_check_and_clear())
    {
        Serial.println("Clearing WiFi credentials");
        app_wifi_wipe_credentials();
        delay(3000);
        Serial.println("Restarting...");
        ESP.restart();
        delay(1000);
    }

    s_led_task.tick();
}

