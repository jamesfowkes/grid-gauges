#include <Arduino.h>
#include <string.h>

#include <HTTPClient.h>

#include <TaskAction.h>

#include "fixed-length-accumulator.h"

#include "http-handler.h"

static HTTPClient s_http_client;

static uint16_t s_download_size = 0;
static uint16_t s_download_count = 0;

bool http_handle_get_stream(FixedLengthAccumulator& dst)
{
    static unsigned long last_run = 0;
    if (millis() == last_run) { return false; }

    WiFiClient * stream = s_http_client.getStreamPtr();
    bool done;

    done = !s_http_client.connected() || (s_download_count == s_download_size);
    if (!done)
    {
        size_t size = stream->available();
        while(size)
        {
            dst.writeChar(stream->read());
            size--;
            s_download_count++;
        }
    }
    else
    {
        s_download_count = 0;
        s_download_size = 0;
        Serial.println("Done.");    
    }

    return done;
}

bool http_start_download(char * url)
{
    bool success = false;
    s_http_client.begin(url);
    int response = s_http_client.GET();
    Serial.println("Starting download...");
    switch(response)
    {
        case HTTP_CODE_OK:
            s_download_size = s_http_client.getSize();
            success = s_download_size > 0;
            if (success)
            {
                s_download_count = 0;
                Serial.print("HTTP GET OK. Streaming ");
                Serial.print(s_download_size);
                Serial.println(" bytes...");
            }
            break;
        default:
            Serial.print("Got HTTP response ");
            Serial.println(response);
            break;
    }
    return success;
}

static void download_state_task_fn(TaskAction* pTask)
{
    (void)pTask;
    if (s_download_count)
    {
        Serial.print("Got ");
        Serial.print(s_download_count);
        Serial.print(" of ");
        Serial.print(s_download_size);
        Serial.println(" bytes.");
    }
}
static TaskAction s_download_state_task(download_state_task_fn, 100, INFINITE_TICKS);

void http_handler_loop()
{
    s_download_state_task.tick();
}
