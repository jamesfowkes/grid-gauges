#ifndef _XML_PROCESSOR_H_
#define _XML_PROCESSOR_H_

class XMLProcessor
{
public:
    XMLProcessor();

    void process(char const * xml, size_t length, bool debug);
    char const * time();
    char const * total();
    uint8_t fuel_type_count();
    char * get_fuel_type(uint8_t i);
    uint32_t get_fuel_generation(uint8_t i);
    uint8_t get_fuel_percent(uint8_t i);
    void test(bool debug);

private:
    void reset();
    char m_time[32] = "";
    char m_total[16] = "";
    char m_fuel_types[32][12];
    uint32_t m_generation[32] = {0};
    uint8_t m_generation_pct[32] = {0};
    uint8_t m_fuel_type_count = 0;
    bool m_debug;
};

#endif
