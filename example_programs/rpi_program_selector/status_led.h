#ifndef STATUS_LED_H
#define STATUS_LED_H

// Let the onboard LED blink at 1Hz.
// Works on Pico (GPIO LED) and Pico W (LED on the CYW43 chip).
void status_led_start(void);

#endif
