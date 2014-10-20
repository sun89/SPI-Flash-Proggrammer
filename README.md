SPI-Flash-Proggrammer
=====================
I write this project for make opensource spi flash programmer

Requirement
===========
1. Arduino board(I use arduino Duemilanove)
2. Some Resistor for current limit between Arduino(5v) to spi flash(3.3v)
3. 3.3 Volt regulator for power flash ic.

Connection
==========
Connection between Arduino and Flash 
Arduno Pin  8 --> R 68E --> Flash HOLD 
Arduno Pin  9 --> R 68E --> Flash WP
Arduno Pin 10 --> R 68E --> Flash CS
Arduno Pin 11 --> R 68E --> Flash DI
Arduno Pin 12 --> R 68E --> Flash DO --> R 10k --> 3.3 volt
Arduno Pin 13 --> R 68E --> Flash CLK
Flash VCC is 3.3 volt
