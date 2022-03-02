/*
    Simple proof-of-concept sketch to show two independent NeoPixel arrays being controlled by
    individual GPIO pins.

    Just for fun, put the code for one of the NeoPixel arrays into an independent task.
*/
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// GPIOd the LEDs are connected to
#define CIRCLE 25 
#define STRIP 27

// The number of LEDs per NeoPixel array
#define CIRCLE_LEDS 12
#define STRIP_LEDS 16

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 75 

// Declare TWO independent objects to control the NeoPixels
Adafruit_NeoPixel circle(CIRCLE_LEDS, CIRCLE, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip(STRIP_LEDS, STRIP, NEO_GRB + NEO_KHZ800);

// Control the 16-LED strip array in a separate task
void stripLedTask(void *parameter);
TaskHandle_t Task0;

// -----------------------------------------------------------
// SETUP    SETUP    SETUP    SETUP    SETUP    SETUP    SETUP
// -----------------------------------------------------------
void setup() {
    Serial.begin(115200);

    // Ensure all LED arrays are clear when we start
    circle.begin();
    circle.clear();
    circle.setBrightness(BRIGHTNESS);

    strip.begin();
    strip.clear();
    strip.setBrightness(BRIGHTNESS);

    // Create the LED strip to a separate task (note: starts immediately)
    xTaskCreatePinnedToCore(
        stripLedTask, /* Function to implement the task */
        "Task0",      /* Name of the task */
        1000,         /* Stack size in words */
        NULL,         /* Task input parameter */
        1,            /* Priority of the task */
        &Task0,       /* Task handle. */
        1);           /* Core where the task should run */

    Serial.println("Setup completed");
}

// This is the code that causes the Darth Vader/Knight Rider effect
void stripLedTask(void *parameter) {
    static uint8_t red;
    static uint8_t green;
    static uint8_t blue;
    static uint8_t cycle = 0;

    // Do forever (the equivalent of a loop())
    for (;;) {
        switch (cycle) {
        case 0:
            red   = 255;
            green = 0;
            blue  = 0;
            cycle = 1;
            break;

        case 1:
            red   = 0;
            green = 255;
            blue  = 0;
            cycle = 2;
            break;

        case 2:
            red   = 0;
            green = 0;
            blue  = 255;
            cycle = 0;
            break;

        default:
            break;
        }

        // Forwards
        for (int i = 0; i < STRIP_LEDS; i++) {
            strip.setPixelColor(i, red, green, blue);
            strip.show();
            delay(40);
            // Clear the lit LED after a small delay
            strip.setPixelColor(i, 0, 0, 0);
            strip.show();
        }

        // Backwards
        for (int i = STRIP_LEDS - 1; i >= 0; i--) {
            strip.setPixelColor(i, red, green, blue);
            strip.show();
            delay(40);
            strip.setPixelColor(i, 0, 0, 0);
            strip.show();
        }
    }
}

// ----------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// ----------------------------------------------------------
void loop() {
    uint8_t red   = 255;
    uint8_t green = 0;
    uint8_t blue  = 0;

    // Make the colours change as we go round the circle
    for (int i = 0; i < CIRCLE_LEDS; i++) {
        circle.setPixelColor(i, red, green, blue);
        circle.show();
        delay(40);
        blue = blue + 12;
        red  = red - 12;
    }

    // Change colours and go round the circle the other way
    red   = 0;
    green = 255;
    blue  = 0;
    circle.clear();

    for (int i = CIRCLE_LEDS - 1; i >= 0; i--) {
        circle.setPixelColor(i, red, green, blue);
        circle.show();
        delay(50);
    }

    // And again
    green = 0;
    blue  = 255;
    circle.clear();

    for (int i = 0; i < CIRCLE_LEDS; i++) {
        circle.setPixelColor(i, red, green, blue);
        circle.show();
        delay(30);
        green += 8;
    }

    // Clear down the circle LEDs and start again
    circle.clear();
    circle.show();
}