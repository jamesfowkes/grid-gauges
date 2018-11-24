#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>

static WiFiManager s_wifiManager;

static void onAccessPointCallback(WiFiManager * pManager)
{
  (void)pManager;
  Serial.print("Started access point ");
  Serial.print(s_wifiManager.getConfigPortalSSID());
  Serial.print(" at ");
  Serial.println(WiFi.softAPIP());
}

void app_wifi_setup()
{
  s_wifiManager.setAPCallback(onAccessPointCallback);

  s_wifiManager.autoConnect("GridGauges-AP");

  Serial.print("Connected to ");
  Serial.print(WiFi.SSID());
  Serial.print(" as ");
  Serial.println(WiFi.localIP());
}

void app_wifi_wipe_credentials()
{
  WiFi.disconnect(true);
  WiFi.begin("0","0");
}

