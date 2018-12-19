#include <Arduino.h>

#include <fixed-length-accumulator.h>

#include "grid-gauges.h"
#include "xml-parser.h"
#include "grid-power.h"
#include "elexon.h"

static void handle_complete_buffer(FixedLengthAccumulator& buffer)
{
    if (buffer.strncmp("APIKEY:", 7) == 0)
    {
        elexon_set_api_key(buffer.c_str()+7);
    }
    else if (buffer.strncmp("FORCE_DOWNLOAD", 14) == 0)
    {
        application_set_flag(eApplicationFlag_Download);
    }
    else if (buffer.strncmp("PRINT_LATEST", 12) == 0)
    {
        application_set_flag(eApplicationFlag_Print);
    }
}

void serial_loop()
{
    static char s_incoming[128];
    static FixedLengthAccumulator s_in_buffer(s_incoming, 128);

    char c;
    while(Serial.available())
    {
        c = Serial.read();
        if (c != '\n')
        {
            s_in_buffer.writeChar(c);
        }
        else
        {
            handle_complete_buffer(s_in_buffer);
            s_in_buffer.reset();
        }
    }
}
