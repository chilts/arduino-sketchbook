// ----------------------------------------------------------------------------
//
// Temperature sensor (DHT11) to show on an OLED 0.91" 128x32 display.
//
// References:
//
// * the `ssd1306_128x32_i2c` sketch
// * the `DHTtester` sketch
// * https://www.arduino.cc/en/Tutorial/BuiltInExamples/BlinkWithoutDelay
//
// ----------------------------------------------------------------------------

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

// === Setup ===

// --- Button ---
int btnPin = 5;

// --- OLED ---
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an Arduino UNO:       A4(SDA), A5(SCL)
// On an Arduino Nano:       A4(SDA), A5(SCL)
// On an Arduino MEGA 2560: 20(SDA), 21(SCL)
// On an Arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- DHT11 ---
// Digital Pin "DATA" connected to the DHT11 sensor
#define DHT_PIN 2
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);
float hum = 0, tmp = 0, hic = 0;

// state machines - button, display, sensor read
long btnPressedMs = 0;
long dhtLastReadInMs = 0;
const long dhtStabiliseInMs = 2000;
const long dhtReadIntervalInMs = 2000;
bool showDisplay;
const long displayedInMs = 10000;

void setup() {
  Serial.begin(9600);
  // Leonardo: wait for serial monitor
  while (!Serial) {
    delay(10);
  }
  Serial.println(F("\nDHT11 Temperature to OLED Display"));

  // Set up the input button
  pinMode(btnPin, INPUT_PULLUP);

  // Set up the SSD1306

  // SSD1306_SWITCHCAPVCC means to generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Normal 1:1 pixel scale, does 21x4 chars across/down
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // and show some initial text
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Loading ...");
  display.display();

  // setup the DHT11 and let it stabilise
  dht.begin();
  delay(dhtStabiliseInMs);
}

void loop() {
  // always get the current ms
  unsigned long currentMs = millis();

  // read the button (check for LOW since it has a pull-up resistor)
  int btnVal = digitalRead(btnPin);
  if ( btnVal == LOW ) {
    // only re-read the sensor and show the display if we haven't already
    if ( !showDisplay ) {
      Serial.println(F("Button Pressed"));
      btnPressedMs = currentMs;
      showDisplay = true;
    }
  }
  else {
    // if the display is on, see if it has timed out
    if ( showDisplay && currentMs > btnPressedMs + displayedInMs ) {
      Serial.println(F("Turning off Display"));
      btnPressedMs = 0;
      showDisplay = false;
      display.clearDisplay();
      display.display();
    }
    // else, nothing to do yet - still showing the display
  }

  // read the DHT and write our readings to the OLED
  if ( showDisplay ) {
    // We only need to read the DHT if we are displaying something.
    // This will change if we want to send to a data server using MQTT.
    perhapsReadDht(currentMs);

    // write to the display
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("------- TOMMY -------"));
    display.print(F(" Humidity    : "));
    display.println(hum);
    display.print(F(" Temperature : "));
    display.println(tmp);
    display.print(F(" Heat Index  : "));
    display.println(hic);
    display.display();
  }
  else {
    display.clearDisplay();
    display.display();
  }

  delay(10);
}

void perhapsReadDht(long currentMs) {
  // check if we have waited at least `dhtReadIntervalInMs`
  if ( dhtLastReadInMs > currentMs - dhtReadIntervalInMs ) {
    return;
  }

  // remember the current time for the sensor read
  dhtLastReadInMs = currentMs;

  // Reading temperature or humidity takes about 250 milliseconds!

  // Sensor readings may also be up to 2 seconds old (it's a very slow sensor).
  hum = dht.readHumidity();
  // Read temperature as Celsius (the default).
  tmp = dht.readTemperature();

  // Check if any reads failed and exit early (to try again next time).
  if (isnan(hum) || isnan(tmp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(tmp, hum, false);

  // Write these values out to the serial port so we can check
  // them against what we put onto the OLED.
  Serial.print(F("Humidity    : "));
  Serial.print(hum);
  Serial.println(F("%"));
  Serial.print(F("Temperature : "));
  Serial.print(tmp);
  Serial.println(F("C"));
  Serial.print(F("Heat index  : "));
  Serial.print(hic);
  Serial.println(F("C"));
}
