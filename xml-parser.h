#ifndef _XML_PARSER_H_
#define _XML_PARSER_H_

static const uint8_t INVALID_FUEL_TYPE = 0xFF;

class XMLParser
{
public:
    XMLParser();

    void parse(char const * xml, size_t length, bool debug);
    unsigned long epoch_time();
    int32_t total();
    uint8_t fuel_type_count();
    char * get_fuel_type(uint8_t i);
    int32_t get_fuel_generation(char const * const id);
    uint8_t get_fuel_percent(uint8_t i);
    void test(bool debug);
    
private:
    void reset();
    uint8_t get_index_for_fuel(char const * const id);
    unsigned long m_time;
    int32_t m_total = 0;
    char m_fuel_types[32][12];
    int32_t m_generation[32] = {0};
    uint8_t m_generation_pct[32] = {0};
    uint8_t m_fuel_type_count = 0;
    bool m_debug;
};

#endif
