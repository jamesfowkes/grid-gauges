#ifndef _GRID_POWER_H_
#define _GRID_POWER_H_

typedef enum _ePowerType
{
    ePowerType_Demand,
    ePowerType_Coal,
    ePowerType_Nuclear,
    ePowerType_Wind,
    ePowerType_CCGT,
    ePowerType_Other,
} ePowerType;

class GridPower
{

public:
    GridPower();
    GridPower(XMLParser& parser);
    void update(XMLParser& parser);
    int32_t get_powertype_mw(ePowerType category);
    unsigned long time();
    void print(Stream& printer);
private:
    int32_t m_power_mw[6];
    unsigned long m_time;
};

#endif
