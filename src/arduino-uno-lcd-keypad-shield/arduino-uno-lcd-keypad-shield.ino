// LCD Keypad Shield
//
// Based on the program below and modified a fair bit.
//
// More info on here : https://dronebotworkshop.com/lcd-displays-arduino/
//
// * Shield Layout : https://i2.wp.com/dronebotworkshop.com/wp-content/uploads/2018/03/LCD-Keypad-Shield-Layout.jpg
// * A0 buttons    : https://i0.wp.com/dronebotworkshop.com/wp-content/uploads/2018/03/LCD-Keypad-Shield-Buttons.jpg
// * Pinout        : https://i1.wp.com/dronebotworkshop.com/wp-content/uploads/2018/03/LCD-Keypad-Shield-Pinouts.jpg
//
// Contains:
//
// * 1 x 16x2 col/row LCD display
// * 1 x Reset button connected directly to the Arduino Uno's Reset button
// * 5 x select/left/right/up/down buttons connected to A0
//

#include <LiquidCrystal.h>
 
// Setup lcd object with display pinouts
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
char line1[17];

// Define button constants (from left to right on the shield)
#define btnNONE   0
#define btnSELECT 1
#define btnUP     2
#define btnDOWN   3
#define btnRIGHT  4
#define btnLEFT   5

// Define button values, from low to high, as shown by this program.
// You should run this program, check each button's value and enter them here.
#define btnRightVal 0
#define btnUpVal 133
#define btnDownVal 310
#define btnLeftVal 483
#define btnSelectVal 724

// We're going to check about 50 above each reading.
#define btnTolerance 50

// Convert the ADC analogue value into a key constant.
int read_LCD_buttons(int val) {
  if (val < btnRightVal + btnTolerance)
    return btnRIGHT;

  if (val < btnUpVal + btnTolerance)
    return btnUP;

  if (val < btnDownVal + btnTolerance)
    return btnDOWN;

  if (val < btnLeftVal + btnTolerance)
    return btnLEFT;

  if (val < btnSelectVal + btnTolerance)
    return btnSELECT;

  // no button currently pressed
  return btnNONE;
} 

void setup() {
  // Setup the LCD display
  lcd.begin(16, 2); 

  // Print instructions on first line
  lcd.setCursor(0, 0);
  lcd.print("Push a Button ..");
}
  
void loop() {
  // Move to the begining of the second line
  lcd.setCursor(0,1);

  // Call the read buttons function           
  // read the value from the sensor
  int adc_key_in = analogRead(0);
  int lcd_key = read_LCD_buttons(adc_key_in);

  // Print button value on second line 
  switch (lcd_key) {
    case btnRIGHT: {
      lcd.print(F("RIGHT "));
      break;
    }
    case btnLEFT: {
      lcd.print(F("LEFT   "));
      break;
    }
    case btnUP: {
      lcd.print(F("UP    "));
      break;
    }
    case btnDOWN: {
      lcd.print(F("DOWN  "));
      break;
    }
    case btnSELECT: {
      lcd.print(F("SELECT"));
      break;
    }
    case btnNONE: {
      lcd.print(F("NONE  "));
      break;
    }
  }

  // Display A0 reading
  lcd.setCursor(7, 1);
  sprintf(line1, "(A0=%4d)", adc_key_in);
  lcd.print(line1);

  // small delay so we're not spinning much
  delay(250);
}
