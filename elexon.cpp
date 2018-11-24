#include <Arduino.h>
#include <string.h>

#include <Preferences.h>

static char s_api_key[32];
static Preferences s_preferences;

static void print_key()
{
    char api_key[32] = "";
    size_t key_size = s_preferences.getString("apikey",api_key, 32);
    if (key_size)
    {
        Serial.println(api_key);
    }
    else
    {
        Serial.println("???");
    }
}

void elexon_setup()
{
    s_preferences.begin("elexon", false);
    Serial.print("API key:");
    print_key();
}

void elexon_set_api_key(char * key)
{
    s_preferences.putString("apikey", key);
    Serial.print("New API key:");
    print_key();
}

char * elexon_get_api_key()
{
    return s_api_key;
}
