SPI-Flash-Proggrammer
=====================
I write this project for make opensource spi flash programmer

Requirement
===========
1. Arduino board(I use arduino Duemilanove)<br>
2. Some Resistor for current limit between Arduino(5v) to spi flash(3.3v)<br>
3. 3.3 Volt regulator for power flash ic.<br>

Connection
==========
Connection between Arduino and Flash<br> 
Arduno Pin  8 --> R 68E --> Flash HOLD<br>
Arduno Pin  9 --> R 68E --> Flash WP<br>
Arduno Pin 10 --> R 68E --> Flash CS<br>
Arduno Pin 11 --> R 68E --> Flash DI<br>
Arduno Pin 12 --> R 68E --> Flash DO --> R 10k --> 3.3 volt<br>
Arduno Pin 13 --> R 68E --> Flash CLK<br>
Flash VCC is 3.3 volt<br>
