#include <Esp.h>
#include <TaskAction.h>

#include "app-wifi.h"
#include "user-input.h"
#include "serial-handler.h"
#include "elexon.h"
#include "ntp.h"
#include "xml-processor.h"

static void led_task_fn(TaskAction* pTask)
{
    (void)pTask;
    static bool s_led = false;
    digitalWrite(5, s_led = !s_led);
}
static TaskAction s_led_task(led_task_fn, 500, INFINITE_TICKS);

void setup()
{
    XMLProcessor processor;
    Serial.begin(115200);

    pinMode(5, OUTPUT);

    app_wifi_setup();
    user_input_setup();
    ntp_setup();
    elexon_setup();

    processor.test(false);
    Serial.print("Latest time: ");
    Serial.println(processor.time());
    Serial.print("Total generation: ");
    Serial.print(processor.total());
    Serial.println("MW");
    Serial.print("Got ");
    Serial.print(processor.fuel_type_count());
    Serial.println(" fuel types:");
    for (uint8_t i=0; i<processor.fuel_type_count(); i++)
    {
        Serial.print(processor.get_fuel_type(i));
        Serial.print(": ");
        Serial.print(processor.get_fuel_generation(i));
        Serial.print("MW (");
        Serial.print(processor.get_fuel_percent(i));
        Serial.println("%)");
    }
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

