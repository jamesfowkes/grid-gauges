#ifndef _XML_PARSER_H_
#define _XML_PARSER_H_

class XMLParser
{
public:
    XMLParser();

    void parse(char const * xml, size_t length, bool debug);
    char const * time();
    uint32_t total();
    uint8_t fuel_type_count();
    char * get_fuel_type(uint8_t i);
    uint32_t get_fuel_generation(uint8_t i);
    uint8_t get_fuel_percent(uint8_t i);
    void test(bool debug);

private:
    void reset();
    char m_time[32] = "";
    uint32_t m_total = 0;
    char m_fuel_types[32][12];
    uint32_t m_generation[32] = {0};
    uint8_t m_generation_pct[32] = {0};
    uint8_t m_fuel_type_count = 0;
    bool m_debug;
};

#endif
