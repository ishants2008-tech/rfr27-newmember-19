Temp changes



* diode voltage changes
* multiplexer chooses a diode
* Arduino measures the voltage and checked the heat
* reports the result in I^2C
* led blinks if over 60 degrees Celsius





Diodes



* Voltage drops when current goes through the diode
* as the diode gets hotter, the voltage drop decreases
* Temp sensing diode has two different terminals 

  * anode (losing electrons)
  * cathode (gaining electrons)
* Even though the current is the same on all of the diodes, the placement matters. (If the power source is closer to one diode it would be hotter than the others because the power source is emitting heat onto the surrounding area.)
* **ALWAYS KEEP CURRENT THE SAME ACROSS ALL DIODES**



Multiplexer



* Nothing actually measured through this (only sends signals to the Arduino)
* Arduino tells it what diode to connect to









