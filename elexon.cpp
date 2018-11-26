#include <Arduino.h>
#include <string.h>

#include <Preferences.h>

#include <HTTPClient.h>

#include <TaskAction.h>

#include "fixed-length-accumulator.h"

#include "http-handler.h"
#include "xml-processor.h"

enum elexon_state
{
    STATE_INIT,
    STATE_DOWNLOADING,
    STATE_DOWNLOADED,
};
typedef enum elexon_state ELEXON_STATE;

static const char XML_URL_FORMAT[] = "http://downloads.elexonportal.co.uk/fuel/download/latest?key=%s";

static char s_api_key[32];
static Preferences s_preferences;

static char s_url[64];

static const uint16_t MAX_XML_SIZE = 4096;
static char s_xml_buffer[MAX_XML_SIZE];
static FixedLengthAccumulator s_xml_accumulator(s_xml_buffer, MAX_XML_SIZE);

static bool s_download_flag = false;

static ELEXON_STATE s_state = STATE_INIT;
static XMLProcessor s_processor;

static void print_key()
{
    if (strlen(s_api_key))
    {
        Serial.println(s_api_key);
    }
    else
    {
        Serial.println("???");
    }
}

static void update_url()
{
    if (strlen(s_api_key))
    {
        sprintf(s_url, XML_URL_FORMAT, s_api_key);
        Serial.print("URL: ");
        Serial.println(s_url);
    }
}

void elexon_setup()
{
    s_preferences.begin("elexon", false);
    s_preferences.getString("apikey",s_api_key, 32);
    Serial.print("API key:");
    print_key();
    update_url();
}

void elexon_loop()
{
    switch(s_state)
    {
    case STATE_INIT:
    case STATE_DOWNLOADED:
        if (s_download_flag)
        {
            s_download_flag = false;
            s_xml_accumulator.reset();
            if (http_start_download(s_url))
            {
                s_state = STATE_DOWNLOADING;
            } 
        }
        break;
    case STATE_DOWNLOADING:
        if (handle_get_stream(s_xml_accumulator))
        {
            s_processor.process(s_xml_accumulator.c_str(), s_xml_accumulator.length());
            s_state = STATE_DOWNLOADED;
        }
        break;
    default:
        break;
    }

    http_handler_loop();
}
void elexon_set_api_key(char * key)
{
    s_preferences.putString("apikey", key);
    Serial.print("New API key:");
    print_key();
    update_url();
}

char * elexon_get_api_key()
{
    return s_api_key;
}

void elexon_download()
{
    s_download_flag = true;
}

void elexon_print()
{
    if (s_state == STATE_DOWNLOADED)
    {
        Serial.print("Latest time: ");
        Serial.println(s_processor.time());
        Serial.print("Total generation: ");
        Serial.println(s_processor.total());
    }
}

void elexon_copy(char * buffer)
{
    strcpy(buffer, s_xml_accumulator.c_str());
}
