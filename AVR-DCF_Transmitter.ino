/*
*  AVR-DCF-Transmitter
* 
//  www.blinkenlight.net
//
//  Copyright 2014 Udo Klein
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program. If not, see http://www.gnu.org/licenses/
 
// Please take care to NOT amplify the output of pin 3.
// Also do not connect pin 3 to a tuned antenna.
 
// Stay within in legal bounds. You have been warned.
 
// http://www.mikrocontroller.net/articles/Allgemeinzuteilung
// http://www.bundesnetzagentur.de/SharedDocs/Downloads/DE/Sachgebiete/Telekommunikation/Unternehmen_Institutionen/Frequenzen/Allgemeinzuteilungen/2014_04_InduktiveFunkanwendungen_pdf.pdf?__blob=publicationFile&v=5
 
// In Germany this implies that you must not exceed
// 42 dBμA/m in a distance of 10m of your transmitter
// If you connect a 1k resistor to the output pin then
// the current will stay below 5 mA which will keep
// the output well below regulatory limits.


*
* Based on Udo Kleins code.
*
* Stripped out all things not needed and added D2 as input for modulation.
* Need an ATmega328P with 16 MHz crystall for working and a resistor for current limiting in 
* serie with D3 and antenna for not frying the ATMega(max 40.0 mA) or one transitor stage.
* U = R + I.  16MHz 328 is normally 5 V thats giving a 125 Ω then driving antenna without buffert.
* Generating a stady 77500 Hz on D3 and modulation it then aplaying keying on input D2.
* Primerry for use with ESP01S (Or other ESP8266) doing transmitting and modultion of DCF77.
* If you want one chip sulotion go for ESP32 that can do the DCF codeing and PWM in hardware.
* One working ESP32 you can find on Andreas Spiess git: https://github.com/SensorsIot/DCF77-Transmitter-for-ESP32.
* If want to inject the DCF77 time signal in a clock (cuting away the reciver and inject the keying) 
* go for a ESP01S and use ESP-NTP-DCF77-Encoder.
* 
* Copyright 2020 Mattias Westerberg of my parts as Udo above.
* 
*/

const byte LED = 13;
const byte KeyInput = 2;

namespace timer_2 {
    // 16000000 / 77500 = 206 + 70/155 = 206 + 14/31   213 = 75.1 khz // 267 = 60khz 320 = 50Khz
    const uint8_t ticks_per_period = 206;
    // fractional ticks
    const uint8_t nominator = 14;
    const uint8_t denominator = 31;
    const uint8_t pwm_full_carrier      = ticks_per_period / 2;  // ~50  %   duty cycle
    const uint8_t pwm_modulated_carrier = ticks_per_period / 32;  // 1/4 Amplitude = 1/16 Power
 
    void setup() {
         // disable timer2 interrupts during setup
        TIMSK2 = 0;
        // enable OC2B pin for output (digital pin 3)
        DDRD |= 1<<3;
 
        // The fast Pulse Width Modulation or fast PWM mode (WGM22:0 = 3 or 7) provides a high fre-
        // quency PWM waveform generation option. The fast PWM differs from the other PWM option by
        // its single-slope operation. The counter counts from BOTTOM to TOP then restarts from BOT-
        // TOM. TOP is defined as 0xFF when WGM2:0 = 3, and OCR2A when MGM2:0 = 7. In non-
        // inverting Compare Output mode, the Output Compare (OC2x) is cleared on the compare match
        // between TCNT2 and OCR2x, and set at BOTTOM. In inverting Compare Output mode, the out-
        // put is set on compare match and cleared at BOTTOM.
 
        TCCR2A = 1<< WGM20 | 1<<WGM21 | // Fast PWM
                 1<<COM2B1;             // Clear OC2B on Compare Match
        TCCR2B = 1<<CS20 | // no Prescaler
                 1<<WGM22;  // Fast PWM
        OCR2A = ticks_per_period - 1;  // period length
        OCR2B = pwm_full_carrier;  // duty cycle
        TIMSK2 = 1<<OCIE2A;  // enable match interrupts
    }
}
 
ISR(TIMER2_COMPA_vect) {
    static uint8_t accumulated_fractional_ticks = 0;
    accumulated_fractional_ticks += timer_2::nominator;
    if (accumulated_fractional_ticks < timer_2::denominator) {
        OCR2A = timer_2::ticks_per_period - 1;
    } else {
        OCR2A = timer_2::ticks_per_period;
        accumulated_fractional_ticks -= timer_2::denominator;
    }
}

// Interrupt Service Routine for the KEYING input
void KeySignal () {
  if (digitalRead (KeyInput) == HIGH){
    digitalWrite (LED, HIGH);
    OCR2B = timer_2::pwm_modulated_carrier;
  }else{
    digitalWrite (LED, LOW);
    OCR2B = timer_2::pwm_full_carrier;
  }
}

void setup() {
    timer_2::setup();
    Serial.begin(115200);
    Serial.println(F("Start sending at 77500 Hz"));
    Serial.println(F("Keying input pin: D2"));
    Serial.println(F("Antenna output pin: D3"));
    pinMode (LED, OUTPUT);  // so we can see the keying.
    digitalWrite (KeyInput, HIGH);  // internal pull-up resistor
    attachInterrupt (digitalPinToInterrupt (KeyInput), KeySignal, CHANGE);  // attach interrupt handler
}

void loop() {
// Do nothing, just let the interrupts handel it !!
}
