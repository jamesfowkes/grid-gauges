# grid-gauges
An ESP32 based analog gauge driver showing National Grid generation 

There are three types of gauges used:

 - DC Voltmeters that draw 0.4mA. These are driven using PWM and RC filters.
 - DC ammeter with a 300R coil, which has a FSR of 500uA. This is driven using PWM, and RC filter and a 9K7 resistor in series.
 