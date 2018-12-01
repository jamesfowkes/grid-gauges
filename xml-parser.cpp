#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "xml-parser.h"

char dbg_buf[16];

#ifdef UNIT_TEST

#include <iostream>
#define DBG(x) if (m_debug) { memcpy(dbg_buf, x, 15); dbg_buf[15] =  '\0'; std::cout << dbg_buf << std::endl; }

#else

#include <Arduino.h>
#define DBG(x) if (m_debug) { memcpy(dbg_buf, x, 15); dbg_buf[15] =  '\0'; Serial.println(dbg_buf); }

#endif

static bool parse_i32(char const * const numeric, int32_t& parsed, char** p_end)
{
    if (!numeric) { return false; }

    char * p_local_end;
    int32_t local = strtol(numeric, &p_local_end, 10);

    if (p_local_end > numeric)
    {
        parsed = local;
        if (p_end)
        {
            *p_end = p_local_end;
        }
    }
    return (p_local_end > numeric);
}

static bool parse_float(char const * const numeric, float& parsed, char** p_end)
{
    if (!numeric) { return false; }

    char * p_local_end;
    float local = strtof(numeric, &p_local_end);

    if (p_local_end > numeric)
    {
        parsed = local;
        if (p_end)
        {
            *p_end = p_local_end;
        }
    }
    return (p_local_end > numeric);
}

static char const * find_string(char const * haystack, char const * const needle, char const * const pEnd)
{
    size_t needle_size = strlen(needle);

    while((strncmp(haystack, needle, needle_size) != 0) && (haystack < pEnd))
    {
        haystack++; 
    }
    return (haystack < pEnd) ? haystack : NULL;
}

static char const * find_string_after(char const * haystack, char const * const needle, char const * const pEnd)
{
    char const * found = find_string(haystack, needle, pEnd);
    return found ? found + strlen(needle) : NULL;
}

static void copy_quoted_string(char * const dst, char const * const start)
{
    char const * const end = strchr(start, '\"');
    if (end>start)
    {
        size_t length = end-start;
        memcpy(dst, start, length);
        dst[length] = '\0';
    }
}

static void copy_attr(char const * const to_copy, char * const dst, char const * const xml, char const * const end)
{
    char attr_start[12];
    sprintf(attr_start, "%s=\"", to_copy);
    char const * attr = find_string_after(xml, attr_start, end);
    if (attr)
    {
        copy_quoted_string(dst, attr);
    }
}

static void copy_time(char * dst, char const * const xml, char const * const end)
{
    copy_attr("AT", dst, xml, end);
}

static void copy_total(char * dst, char const * const xml, char const * const end)
{
    copy_attr("TOTAL", dst, xml, end);
}

XMLParser::XMLParser()
{
    this->reset();
    m_debug = false;
}

void XMLParser::reset()
{
    for (uint8_t i=0; i<32; i++)
    {
        m_fuel_types[i][0] = '\0';
    }
    m_fuel_type_count = 0;
}

void XMLParser::parse(char const * const xml_start, size_t length, bool debug)
{
    char buffer[16];
    int32_t i32Temp;

    this->reset();
    this->m_debug = debug;

    char const * p_XML_end = xml_start + length - 1;

    char const * xml = find_string(xml_start, "<INST ", p_XML_end);
    char const * xml_end = find_string(xml_start, "</INST>", p_XML_end);
    DBG(xml);

    copy_time(m_time, xml, xml_end);
    copy_total(buffer, xml, xml_end);   

    if(parse_i32(buffer, i32Temp, NULL))
    {
        m_total = i32Temp;
    }
    
    float fTemp;

    while (xml)
    {
        xml = find_string_after(xml, "<FUEL ", xml_end);
        if (xml)
        {
            DBG(xml);
            copy_attr("TYPE", m_fuel_types[m_fuel_type_count], xml, xml_end);
            
            copy_attr("VAL", buffer, xml, xml_end);
            if (parse_i32(buffer, i32Temp, NULL))
            {
                m_generation[m_fuel_type_count] = i32Temp;
            }

            copy_attr("PCT", buffer, xml, xml_end);
            if (parse_float(buffer, fTemp, NULL))
            {
                m_generation_pct[m_fuel_type_count] = (uint8_t)(fTemp + 0.5f);   
            }
            m_fuel_type_count++;
        }
    }
}

char const * XMLParser::time()
{
    return m_time;
}

int32_t XMLParser::total()
{
    return m_total;
}

uint8_t XMLParser::fuel_type_count()
{
    return m_fuel_type_count;
}

char * XMLParser::get_fuel_type(uint8_t i)
{
    return m_fuel_types[i];
}

int32_t XMLParser::get_fuel_generation(uint8_t i)
{
    return m_generation[i];
}

uint8_t XMLParser::get_fuel_percent(uint8_t i)
{
    return m_generation_pct[i];
}

#ifdef UNIT_TEST

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

std::string slurp(std::ifstream& in) {
    std::stringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

class XMLParserTest : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE(XMLParserTest);

    CPPUNIT_TEST(test_parsing);

    CPPUNIT_TEST_SUITE_END();

    void test_parsing()
    {
        XMLParser parser;
        char const * pXML;
        std::string xml_string;
        std::ifstream test_file ("test.xml");
        if (test_file.is_open())
        {
            xml_string = slurp(test_file);
            pXML = xml_string.c_str();
        }
        test_file.close();
        
        parser.parse(pXML, strlen(pXML), false);

        CPPUNIT_ASSERT_EQUAL(std::string("2018-11-27 06:20:00"), std::string(parser.time()));
        CPPUNIT_ASSERT_EQUAL(33962, parser.total());

        CPPUNIT_ASSERT_EQUAL(15, (int)parser.fuel_type_count());

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(0)), std::string("CCGT"));
        CPPUNIT_ASSERT_EQUAL(14847, parser.get_fuel_generation(0));
        CPPUNIT_ASSERT_EQUAL(44U, (unsigned int)parser.get_fuel_percent(0));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(1)), std::string("OCGT"));
        CPPUNIT_ASSERT_EQUAL(0, parser.get_fuel_generation(1));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)parser.get_fuel_percent(1));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(2)), std::string("OIL"));
        CPPUNIT_ASSERT_EQUAL(0, parser.get_fuel_generation(2));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)parser.get_fuel_percent(2));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(3)), std::string("COAL"));
        CPPUNIT_ASSERT_EQUAL(2514, parser.get_fuel_generation(3));
        CPPUNIT_ASSERT_EQUAL(7U, (unsigned int)parser.get_fuel_percent(3));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(4)), std::string("NUCLEAR"));
        CPPUNIT_ASSERT_EQUAL(6344, parser.get_fuel_generation(4));
        CPPUNIT_ASSERT_EQUAL(19U, (unsigned int)parser.get_fuel_percent(4));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(5)), std::string("WIND"));
        CPPUNIT_ASSERT_EQUAL(6675, parser.get_fuel_generation(5));
        CPPUNIT_ASSERT_EQUAL(20U, (unsigned int)parser.get_fuel_percent(5));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(6)), std::string("PS"));
        CPPUNIT_ASSERT_EQUAL(0, parser.get_fuel_generation(6));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)parser.get_fuel_percent(6));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(7)), std::string("NPSHYD"));
        CPPUNIT_ASSERT_EQUAL(353, parser.get_fuel_generation(7));
        CPPUNIT_ASSERT_EQUAL(1U, (unsigned int)parser.get_fuel_percent(7));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(8)), std::string("OTHER"));
        CPPUNIT_ASSERT_EQUAL(27, parser.get_fuel_generation(8));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)parser.get_fuel_percent(8));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(9)), std::string("INTFR"));
        CPPUNIT_ASSERT_EQUAL(0, parser.get_fuel_generation(9));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)parser.get_fuel_percent(9));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(10)), std::string("INTIRL"));
        CPPUNIT_ASSERT_EQUAL(93, parser.get_fuel_generation(10));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)parser.get_fuel_percent(10));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(11)), std::string("INTNED"));
        CPPUNIT_ASSERT_EQUAL(0, parser.get_fuel_generation(11));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)parser.get_fuel_percent(11));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(12)), std::string("INTEW"));
        CPPUNIT_ASSERT_EQUAL(504, parser.get_fuel_generation(12));
        CPPUNIT_ASSERT_EQUAL(2U, (unsigned int)parser.get_fuel_percent(12));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(13)), std::string("BIOMASS"));
        CPPUNIT_ASSERT_EQUAL(2605, parser.get_fuel_generation(13));
        CPPUNIT_ASSERT_EQUAL(8U, (unsigned int)parser.get_fuel_percent(13));

        CPPUNIT_ASSERT_EQUAL(std::string(parser.get_fuel_type(14)), std::string("INTNEM"));
        CPPUNIT_ASSERT_EQUAL(0, parser.get_fuel_generation(14));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)parser.get_fuel_percent(14));

    }

};

int main()
{
   CppUnit::TextUi::TestRunner runner;
   
   CPPUNIT_TEST_SUITE_REGISTRATION( XMLParserTest );

   CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

   runner.addTest( registry.makeTest() );
   runner.run();

   return 0;
}

#endif
