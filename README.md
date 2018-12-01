# grid-gauges
An ESP32 based analog gauge driver showing National Grid generation 

There are three types of gauges used:

 - DC Voltmeters that draw 0.4mA. These are driven using PWM and RC filters.
 - DC ammeter with a 300R coil, which has a FSR of 500uA. This is driven using PWM, an RC filter and a 8K2 resistor in series.
 - AC (moving iron) ammeter (DC is fine) with a 150mA FSR. This is driven using PWM, an RC filter and a emitter-follower transistor buffer [Falstad circuit](http://www.falstad.com/circuit/circuitjs.html?cct=$+1+0.000005+63.79968419005069+50+5+50%0AR+96+64+48+64+0+0+40+5+0+0+0.5%0Ar+432+304+432+208+0+18%0A370+432+64+432+208+1+0%0Ag+432+304+432+336+0%0At+336+144+336+64+1+1+-1.5168725933340292+0.7254530318026973+100%0Aw+320+64+96+64+0%0Aw+352+64+432+64+0%0Ac+336+144+336+224+0+0.00001+3.4831274066659708%0Ag+336+224+336+288+0%0Ar+336+144+192+144+0+1000%0AR+192+144+96+144+0+2+3000+2.5+2.5+0+1%0Ao+2+64+0+4353+0.0001+0.46736693281189107+0+2+2+3%0A)