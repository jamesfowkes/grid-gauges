#include <Arduino.h>

#include <stdint.h>
#include <stdlib.h>

#include "xml-parser.h"
#include "grid-power.h"

GridPower::GridPower() : m_time(0)
{

}

GridPower::GridPower(XMLParser& parser) : m_time(0)
{
	this->update(parser);
}

void GridPower::update(XMLParser& parser)
{
	m_power_mw[ePowerType_Demand] = parser.total();
    m_time = parser.epoch_time();
    m_power_mw[ePowerType_Coal] = parser.get_fuel_generation("COAL");
    m_power_mw[ePowerType_Nuclear] = parser.get_fuel_generation("NUCLEAR");
    m_power_mw[ePowerType_Wind] = parser.get_fuel_generation("WIND");
    m_power_mw[ePowerType_CCGT] =  parser.get_fuel_generation("CCGT");
    m_power_mw[ePowerType_Other] = m_power_mw[ePowerType_Demand] - 
    	(m_power_mw[ePowerType_Coal] + m_power_mw[ePowerType_Nuclear] + 
    	m_power_mw[ePowerType_Wind] + m_power_mw[ePowerType_CCGT]);
}

int32_t GridPower::get_powertype_mw(ePowerType powertype)
{
	return m_power_mw[powertype];
}

unsigned long GridPower::time()
{
    return m_time;
}

void GridPower::print(Stream& printer)
{
    printer.print("Demand: "); printer.print(m_power_mw[ePowerType_Demand]); printer.println("MW");
    printer.print("Coal: "); printer.print(m_power_mw[ePowerType_Coal]); printer.println("MW");
    printer.print("Nuclear: "); printer.print(m_power_mw[ePowerType_Nuclear]); printer.println("MW");
    printer.print("Wind: "); printer.print(m_power_mw[ePowerType_Wind]); printer.println("MW");
    printer.print("CCGT: "); printer.print(m_power_mw[ePowerType_CCGT]); printer.println("MW");
    printer.print("Other: "); printer.print(m_power_mw[ePowerType_Other]); printer.println("MW");
}
