#include <Arduino.h>

#include <fixed-length-accumulator.h>

#include "elexon.h"

static void handle_complete_buffer(FixedLengthAccumulator& buffer)
{
    char * command = buffer.c_str();
    if (buffer.strncmp("APIKEY:",7) == 0)
    {
        elexon_set_api_key(buffer.c_str()+7);
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
        }
    }
}
