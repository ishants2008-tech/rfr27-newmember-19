Temp changes



* diode voltage changes
* multiplexer chooses a diode
* Arduino measures the voltage and checked the heat
* reports the result in I^2C
* led blinks if over 60 degrees Celsius





Diodes



* Voltage drops when current goes through the diode
* as the diode gets hotter, the voltage drop decreases



Multiplexer



* Nothing actually measuered through this (only sends signals to the adruino)
* Arduino tells it what diode to connect to

