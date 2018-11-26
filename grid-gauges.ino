#include <Esp.h>

#include "app-wifi.h"
#include "user-input.h"
#include "serial-handler.h"
#include "elexon.h"
#include "ntp.h"

void setup()
{
  Serial.begin(115200);

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
}

