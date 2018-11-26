#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "xml-processor.h"

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
    
    if (end>dst)
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
    char * p_inst_end = find_string(xml, "</INST>", p_XML_end);

    xml = find_string(xml, "<INST ", p_XML_end);
    copy_time(m_time, xml, p_XML_end);
    copy_total(m_total, xml, p_XML_end);

    /*while (xml < p_inst_end)
    {
        xml = find_string(xml, "<FUEL_TYPE='", p_XML_end);
        if (xml < p_inst_end)
        {

        }
    }*/
}

char const * XMLProcessor::time()
{
    return m_time;
}

char const * XMLProcessor::total()
{
    return m_total;
}

#ifdef UNIT_TEST

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

class XMLProcessorTest : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE(XMLProcessorTest);
    CPPUNIT_TEST(test_time_processing);
    CPPUNIT_TEST(test_total_processing);
    CPPUNIT_TEST_SUITE_END();

    void test_time_processing()
    {
        char xml[] = "<GENERATION_BY_FUEL_TYPE_TABLE><INST AT=\"2018-11-25 21:40:00\" TOTAL=\"34223\">";
        XMLProcessor processor;
        processor.process(xml, strlen(xml));
        CPPUNIT_ASSERT_EQUAL(std::string("2018-11-25 21:40:00"), std::string(processor.time()));
    }

    void test_total_processing()
    {
        char xml[] = "<GENERATION_BY_FUEL_TYPE_TABLE><INST AT=\"2018-11-25 21:40:00\" TOTAL=\"34223\">";
        XMLProcessor processor;
        processor.process(xml, strlen(xml));
        CPPUNIT_ASSERT_EQUAL(std::string("34223"), std::string(processor.total()));
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
