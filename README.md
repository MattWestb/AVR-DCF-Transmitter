# AVR-DCF-Transmitter
Using one ATmega328P with 16 MHz crystal for sending / modulating  DCF77 


 Based on Udo Kleins code.

 Stripped out all things not needed and added D2 as input for modulation.
 
 Need an ATmega328P with 16 MHz crystal for working and a resistor for current limiting in 
 serie with D3 and antenna for not frying the ATMega(max 40.0 mA) or one transitor stage.
 (U = R * I) 16MHz 328 is normally 5 V thats giving a 125 Ω then driving antenna without buffert.
 Generating a stady 77500 Hz on D3 and modulation it then aplaying keying on input D2.
 Primerry for use with ESP-01S (Or other ESP8266) doing transmitting and modultion of DCF77.
 If you want one chip sulotion go for ESP32 that can do the DCF codeing and PWM in hardware.
 One working ESP32 you can find on Andreas Spiess git: https://github.com/SensorsIot/DCF77-Transmitter-for-ESP32.
 If want to inject the DCF77 time signal in a clock (cuting away the reciver and inject the keying) 
 go for a ESP01S and use ESP-NTP-DCF77-Encoder.
 
 Copyright 2020 Mattias Westerberg of my parts as Udo above.
