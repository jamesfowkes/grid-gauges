#include <Arduino.h>
#include <string.h>

#include <HTTPClient.h>

#include <TaskAction.h>

#include "fixed-length-accumulator.h"

#include "http-handler.h"

static HTTPClient * sp_http_client;

static uint16_t s_download_size = 0;
static uint16_t s_download_count = 0;

bool http_handle_get_stream(FixedLengthAccumulator& dst)
{
    static unsigned long last_run = 0;
    
    if (millis() == last_run) { return false; }
    if (!sp_http_client) { return false; }

    WiFiClient * stream = sp_http_client->getStreamPtr();
    bool done = s_download_count == s_download_size;

    if (stream)
    {
        if (!done)
        {
            size_t size = stream->available();
            while(size)
            {
                dst.writeChar(stream->read());
                size--;
                s_download_count++;
            }
            Serial.println("Stream read end");
            Serial.print("Download count: ");
            Serial.println(s_download_count);
            Serial.print("Download size: ");
            Serial.println(s_download_size);
            Serial.flush();
        }
        done = s_download_count == s_download_size;
    }
    else
    {
        Serial.println("No HTTP stream");
        Serial.flush();
    }
    if (done)
    {
        s_download_count = 0;
        s_download_size = 0;
        Serial.println("Done.");
        Serial.flush();
        Serial.println("HTTP client end");
        Serial.flush();
        delete sp_http_client;
        
    }

    return done;
}

bool http_start_download(char * url)
{
    bool success = false;

    sp_http_client = new HTTPClient();

    sp_http_client->setReuse(true);
    sp_http_client->begin(url);
    int response = sp_http_client->GET();
    Serial.println("Starting download...");
    Serial.flush();
    switch(response)
    {
        case HTTP_CODE_OK:
            s_download_size = sp_http_client->getSize();
            success = s_download_size > 0;
            if (success)
            {
                s_download_count = 0;
                Serial.print("HTTP GET OK. Streaming ");
                Serial.print(s_download_size);
                Serial.println(" bytes...");
                Serial.flush();
            }
            break;
        default:
            Serial.print("Got HTTP response ");
            Serial.println(response);
            Serial.flush();
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
        Serial.flush();
    }
}
static TaskAction s_download_state_task(download_state_task_fn, 100, INFINITE_TICKS);

void http_handler_loop()
{
    s_download_state_task.tick();
}
