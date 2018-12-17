#include <Arduino.h>
#include <string.h>

#include <Preferences.h>

#include <HTTPClient.h>

#include <TaskAction.h>

#include "fixed-length-accumulator.h"

#include "grid-gauges.h"
#include "http-handler.h"
#include "xml-parser.h"
#include "ntp.h"

enum elexon_state
{
    STATE_INIT,
    STATE_DOWNLOADING,
    STATE_DOWNLOADED,
};
typedef enum elexon_state ELEXON_STATE;

static const char XML_URL_FORMAT[] = "http://downloads.elexonportal.co.uk/fuel/download/latest?key=%s";

static const bool PRINT_ACCUMULATOR = false;

static char s_api_key[32];
static Preferences s_preferences;

static char s_url[64];

static const uint16_t MAX_XML_SIZE = 4096;
static char s_xml_buffer[MAX_XML_SIZE];
static FixedLengthAccumulator s_xml_accumulator(s_xml_buffer, MAX_XML_SIZE);

static ELEXON_STATE s_state = STATE_INIT;
static XMLParser s_parser;

static unsigned long s_next_download_time = 0;

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

static bool is_time_to_download()
{
    bool valid_time = ntp_got_valid_time();
    bool time_to_download = s_next_download_time <= ntp_get_time();
    bool download_flag = application_check_flag(eApplicationFlag_Download);
    
    if (time_to_download)
    {
        Serial.print("Downloading on time: ");
        Serial.print(s_next_download_time);
        Serial.print(" <= ");
        Serial.println(ntp_get_time());
    }

    if (download_flag)
    {
        Serial.println("Downloading on flag.");
    }
    return valid_time && (time_to_download || download_flag);
}

static void print_latest()
{
    if (s_state == STATE_DOWNLOADED)
    {
        Serial.print("Latest time: ");
        Serial.println(s_parser.epoch_time());
        Serial.print("Total generation: ");
        Serial.print(s_parser.total());
        Serial.println("MW");
        Serial.print("Got ");
        Serial.print(s_parser.fuel_type_count());
        Serial.println(" fuel types");
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

static void idle_task_fn(TaskAction* pTask)
{
    (void)pTask;
    if (is_time_to_download())
    {
        s_xml_accumulator.reset();
        if (http_start_download(s_url))
        {
            s_state = STATE_DOWNLOADING;
        } 
    }
}
static TaskAction s_idle_task(idle_task_fn, 1000, INFINITE_TICKS);

void elexon_loop()
{
    switch(s_state)
    {
    case STATE_INIT:
    case STATE_DOWNLOADED:
        s_idle_task.tick();
        break;
    case STATE_DOWNLOADING:
        if (http_handle_get_stream(s_xml_accumulator))
        {
            s_parser.parse(s_xml_accumulator.c_str(), s_xml_accumulator.length(), true);
            Serial.println("Elexon download complete");
            Serial.flush();
            if (PRINT_ACCUMULATOR)
            {
                char * s = s_xml_accumulator.c_str();
                Serial.println("Accumulator: ");
                Serial.print(s_xml_accumulator.length());
                Serial.println(" bytes");
                for (uint16_t i = 0; i < s_xml_accumulator.length(); i++)
                {
                    Serial.print(s[i]);
                    if (i % 32)
                    {
                        Serial.flush();
                    }
                }
            }
            s_state = STATE_DOWNLOADED;
            s_next_download_time = ntp_get_time() + (6 * 60);
            application_set_flag(eApplicationFlag_DownloadComplete);
        }
        break;
    default:
        break;
    }

    http_handler_loop();

    if (application_check_flag(eApplicationFlag_Print))
    {
        print_latest();
    }
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

void elexon_copy(char * buffer)
{
    strcpy(buffer, s_xml_accumulator.c_str());
}
