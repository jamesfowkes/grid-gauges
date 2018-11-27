#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "xml-processor.h"

char dbg_buf[16];

#ifdef UNIT_TEST

#include <iostream>
#define DBG(x) if (m_debug) { memcpy(dbg_buf, x, 15); dbg_buf[15] =  '\0'; std::cout << dbg_buf << std::endl; }

#else

#include <Arduino.h>
#define DBG(x) if (m_debug) { memcpy(dbg_buf, x, 15); dbg_buf[15] =  '\0'; Serial.println(dbg_buf); }

#endif

static char TEST_XML[] = "<?xml version=\"1.0\"?>\r"
"<GENERATION_BY_FUEL_TYPE_TABLE><INST AT=\"2018-11-25 21:40:00\" TOTAL=\"34223\">\r"
"<FUEL TYPE=\"CCGT\" IC=\"N\" VAL=\"23105\" PCT=\"52.5\"/>"
"<FUEL TYPE=\"OCGT\" IC=\"N\" VAL=\"0\" PCT=\"0.0\"/>"
"<FUEL TYPE=\"OIL\" IC=\"N\" VAL=\"0\" PCT=\"0.0\"/>"
"<FUEL TYPE=\"COAL\" IC=\"N\" VAL=\"7356\" PCT=\"16.7\"/>"
"<FUEL TYPE=\"NUCLEAR\" IC=\"N\" VAL=\"6975\" PCT=\"15.8\"/>"
"<FUEL TYPE=\"WIND\" IC=\"N\" VAL=\"2338\" PCT=\"5.3\"/>"
"<FUEL TYPE=\"PS\" IC=\"N\" VAL=\"1055\" PCT=\"2.4\"/>"
"<FUEL TYPE=\"NPSHYD\" IC=\"N\" VAL=\"581\" PCT=\"1.3\"/>"
"<FUEL TYPE=\"OTHER\" IC=\"N\" VAL=\"72\" PCT=\"0.2\"/>"
"<FUEL TYPE=\"INTFR\" IC=\"Y\" VAL=\"0\" PCT=\"0.0\"/>"
"<FUEL TYPE=\"INTIRL\" IC=\"Y\" VAL=\"0\" PCT=\"0.0\"/>"
"<FUEL TYPE=\"INTNED\" IC=\"Y\" VAL=\"0\" PCT=\"0.0\"/>"
"<FUEL TYPE=\"INTEW\" IC=\"Y\" VAL=\"0\" PCT=\"0.0\"/>"
"<FUEL TYPE=\"BIOMASS\" IC=\"N\" VAL=\"2563\" PCT=\"5.8\"/>"
"<FUEL TYPE=\"INTNEM\" IC=\"Y\" VAL=\"0\" PCT=\"0.0\"/>"
"</INST>";

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

static char * find_string(char * haystack, char const * const needle, char * pEnd)
{
    size_t needle_size = strlen(needle);

    while((strncmp(haystack, needle, needle_size) != 0) && (haystack < pEnd))
    {
        haystack++; 
    }
    return (haystack < pEnd) ? haystack : NULL;
}

static char * find_string_after(char * haystack, char const * const needle, char * pEnd)
{
    char * found = find_string(haystack, needle, pEnd);
    return found ? found + strlen(needle) : NULL;
}

static void copy_quoted_string(char * dst, char * start)
{
    char * end = strchr(start, '\"');
    if (end>start)
    {
        size_t length = end-start;
        memcpy(dst, start, length);
        dst[length] = '\0';
    }
}

static void copy_attr(char const * const to_copy, char * dst, char * xml, char * end)
{
    char attr_start[12];
    sprintf(attr_start, "%s=\"", to_copy);
    char * attr = find_string_after(xml, attr_start, end);
    if (attr)
    {
        copy_quoted_string(dst, attr);
    }
}

static void copy_time(char * dst, char * xml, char * end)
{
    copy_attr("AT", dst, xml, end);
}

static void copy_total(char * dst, char * xml, char * end)
{
    copy_attr("TOTAL", dst, xml, end);
}

XMLProcessor::XMLProcessor()
{
    this->reset();
    m_debug = false;
}

void XMLProcessor::reset()
{
    for (uint8_t i=0; i<32; i++)
    {
        m_fuel_types[i][0] = '\0';
    }
    m_fuel_type_count = 0;
}

void XMLProcessor::process(char * xml, size_t length)
{
    this->reset();
     
    char * p_XML_end = xml + length - 1;

    xml = find_string(xml, "<INST ", p_XML_end);
    DBG(xml);

    copy_time(m_time, xml, p_XML_end);
    copy_total(m_total, xml, p_XML_end);   

    char buffer[16];
    int32_t i32Temp;
    float fTemp;

    while (xml)
    {
        xml = find_string_after(xml, "<FUEL ", p_XML_end);
        if (xml)
        {
            DBG(xml);
            copy_attr("TYPE", m_fuel_types[m_fuel_type_count], xml, p_XML_end);
            
            copy_attr("VAL", buffer, xml, p_XML_end);
            if (parse_i32(buffer, i32Temp, NULL))
            {
                m_generation[m_fuel_type_count] = (uint32_t)i32Temp;
            }

            copy_attr("PCT", buffer, xml, p_XML_end);
            if (parse_float(buffer, fTemp, NULL))
            {
                m_generation_pct[m_fuel_type_count] = (uint8_t)(fTemp + 0.5f);   
            }
            m_fuel_type_count++;
        }
    }
}

void XMLProcessor::test(bool debug)
{
    this->m_debug = debug;
    this->process(TEST_XML, strlen(TEST_XML));
    this->m_debug = false;
}

char const * XMLProcessor::time()
{
    return m_time;
}

char const * XMLProcessor::total()
{
    return m_total;
}

uint8_t XMLProcessor::fuel_type_count()
{
    return m_fuel_type_count;
}

char * XMLProcessor::get_fuel_type(uint8_t i)
{
    return m_fuel_types[i];
}

uint32_t XMLProcessor::get_fuel_generation(uint8_t i)
{
    return m_generation[i];
}

uint8_t XMLProcessor::get_fuel_percent(uint8_t i)
{
    return m_generation_pct[i];
}

#ifdef UNIT_TEST

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

class XMLProcessorTest : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE(XMLProcessorTest);

    CPPUNIT_TEST(test_copy_time);
    CPPUNIT_TEST(test_processing);

    CPPUNIT_TEST_SUITE_END();

    void test_copy_time()
    {
        char actual[32];
        char source[] = "<INST AT=\"2018-11-25 21:40:00\" TOTAL=\"34223\">";
        copy_time(actual, source, source+strlen(source));
        CPPUNIT_ASSERT_EQUAL(std::string("2018-11-25 21:40:00"), std::string(actual));
    }

    void test_processing()
    {
        XMLProcessor processor;
        processor.test(true);

        CPPUNIT_ASSERT_EQUAL(std::string("2018-11-25 21:40:00"), std::string(processor.time()));
        CPPUNIT_ASSERT_EQUAL(std::string("34223"), std::string(processor.total()));

        CPPUNIT_ASSERT_EQUAL(15, (int)processor.fuel_type_count());

        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(0)), std::string("CCGT"));
        CPPUNIT_ASSERT_EQUAL(23105U, processor.get_fuel_generation(0));
        CPPUNIT_ASSERT_EQUAL(53U, (unsigned int)processor.get_fuel_percent(0));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(1)), std::string("OCGT"));
        CPPUNIT_ASSERT_EQUAL(0U, processor.get_fuel_generation(1));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)processor.get_fuel_percent(1));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(2)), std::string("OIL"));
        CPPUNIT_ASSERT_EQUAL(0U, processor.get_fuel_generation(2));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)processor.get_fuel_percent(2));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(3)), std::string("COAL"));
        CPPUNIT_ASSERT_EQUAL(7356U, processor.get_fuel_generation(3));
        CPPUNIT_ASSERT_EQUAL(17U, (unsigned int)processor.get_fuel_percent(3));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(4)), std::string("NUCLEAR"));
        CPPUNIT_ASSERT_EQUAL(6975U, processor.get_fuel_generation(4));
        CPPUNIT_ASSERT_EQUAL(16U, (unsigned int)processor.get_fuel_percent(4));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(5)), std::string("WIND"));
        CPPUNIT_ASSERT_EQUAL(2338U, processor.get_fuel_generation(5));
        CPPUNIT_ASSERT_EQUAL(5U, (unsigned int)processor.get_fuel_percent(5));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(6)), std::string("PS"));
        CPPUNIT_ASSERT_EQUAL(1055U, processor.get_fuel_generation(6));
        CPPUNIT_ASSERT_EQUAL(2U, (unsigned int)processor.get_fuel_percent(6));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(7)), std::string("NPSHYD"));
        CPPUNIT_ASSERT_EQUAL(581U, processor.get_fuel_generation(7));
        CPPUNIT_ASSERT_EQUAL(1U, (unsigned int)processor.get_fuel_percent(7));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(8)), std::string("OTHER"));
        CPPUNIT_ASSERT_EQUAL(72U, processor.get_fuel_generation(8));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)processor.get_fuel_percent(8));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(9)), std::string("INTFR"));
        CPPUNIT_ASSERT_EQUAL(0U, processor.get_fuel_generation(9));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)processor.get_fuel_percent(9));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(10)), std::string("INTIRL"));
        CPPUNIT_ASSERT_EQUAL(0U, processor.get_fuel_generation(10));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)processor.get_fuel_percent(10));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(11)), std::string("INTNED"));
        CPPUNIT_ASSERT_EQUAL(0U, processor.get_fuel_generation(11));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)processor.get_fuel_percent(11));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(12)), std::string("INTEW"));
        CPPUNIT_ASSERT_EQUAL(0U, processor.get_fuel_generation(12));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)processor.get_fuel_percent(12));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(13)), std::string("BIOMASS"));
        CPPUNIT_ASSERT_EQUAL(2563U, processor.get_fuel_generation(13));
        CPPUNIT_ASSERT_EQUAL(6U, (unsigned int)processor.get_fuel_percent(13));
        CPPUNIT_ASSERT_EQUAL(std::string(processor.get_fuel_type(14)), std::string("INTNEM"));
        CPPUNIT_ASSERT_EQUAL(0U, processor.get_fuel_generation(14));
        CPPUNIT_ASSERT_EQUAL(0U, (unsigned int)processor.get_fuel_percent(14));
    }

};

int main()
{
   CppUnit::TextUi::TestRunner runner;
   
   CPPUNIT_TEST_SUITE_REGISTRATION( XMLProcessorTest );

   CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

   runner.addTest( registry.makeTest() );
   runner.run();

   return 0;
}

#endif
