This is PC and Arduino SW of a spectrum analyzer
The HW is build mostly from modules found on ebay with some rather simple filters
The VFO is based on a SI5351 module with max frequency of 250MHz so first IF should be between 100MHz and 120MHz

HW path:
1: 1st Mixer AD831 module (8$)

2: 1st IF filter, between 100-120MHz build or buy. 4MHz bandwith is sufficient.

3: 2nd Mixer AD831 module (8$)

4: 20dB wide band ampplifier module (8$)

5: 2nd IF filter at 10.7MHz. buy some 3 pole crystals filters (5$) or ready made filter module (23$)

6: AD8307 module (8$)

7: Arduino nano module analog input (3$)

8: SI5351 module for generation of LO1 and LO2 (6$)


Connect everything with cheap SMA Male - SMA Male pigtail cables 

7810 Voltage regulator per AD831

12 Volt supply for the modules

SI5351 powered from Arduino

Communication with PC via USB from Arduino.

Carefull select frequency of first IF to minimize spurs (from SI5351 crosstalk and xtal).
With 1st IF at 110MHz I selected 110.7MHz

