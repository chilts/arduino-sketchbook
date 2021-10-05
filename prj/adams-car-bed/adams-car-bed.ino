// ----------------------------------------------------------------------------
//
// # Adam's Car Bed #
//
// Adam has a racing car for his bed. He'd like some lights that he can turn on
// and off. After some discussion we decided on the following requirements:
//
// 1. there is an on/off switch which turns on/off the lights (it's not in the
//    5V line, just allows/disallows all other lights).
// 2. 
//
// ## Bill of Materials ##
//
// 1. Arduino Nano (but of course you can use any other board or mcu you like).
// 2. 14 LEDs with typical current limiting resistors
//   * 2 white for front headlights
//   * 2 red for back lights
//   * 2 yellow to get brighter as we pretend to accelerate (using a POT)
//   * 6 orange for 3 left indicators and 3 right (front, side, and back)
//   * 2 red for brake lights
// 3. a POT to allow analogue input to pretend to accelerate
// 4. a slide switch to "turn the engine on" (using LED_BUILTIN to show "on")
// 5. a 3 way switch for left/off/right indicator
// 6. a momentary push switch for the brake lights
// 7. a slide switch to turn on/off the front/back headlights
//
// ## Circuit ##
//
// For this
//
// Note: when running multiple LEDs using one output pin (such as the
// indicators, front/back headlights, accelerator lights) we'll drive them
// using a transistor instead of directly. 
//
//
//
//
//
// (Ends)
//
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// setup

// 
int loopDelayMs = 10;

// digital pins
int engineSwitchPin = 2;
int headlightsSwitchPin = 3;
int indicatorLeftSwitchPin = 4;
int indicatorRightSwitchPin = 5;
int brakeSwitchPin = 6;

// LEDs
int engineLedPin = LED_BUILTIN;
int headlightLedsPin = 12;
int indicatorLeftLedsPin = 11;
int indicatorRightLedsPin = 10;
int acceleratorLedsPin = 9;
int brakeLedsPin = 8;

// state machines
bool isEngineOn = false;
bool isHeadlightsOn = false;
bool isIndicatorLeftOn = false;
bool isIndicatorRightOn = false;
bool indicatorFlashOn = 0;
bool isBrakeOn = false;

// indicator timing
long indicatorOnInMs = 0;
long indicatorPeriodInMs = 1000;

void setup() {
  // start the serial port
  Serial.begin(9600);
  // Leonardo: wait for serial monitor
  while (!Serial) {
    delay(10);
  }
  Serial.println(F("\nAdam's Car Bed"));

  // input switches (all with pull-up resistors)
  pinMode(engineSwitchPin, INPUT_PULLUP);
  pinMode(headlightsSwitchPin, INPUT_PULLUP);
  pinMode(indicatorLeftSwitchPin, INPUT_PULLUP);
  pinMode(indicatorRightSwitchPin, INPUT_PULLUP);
  pinMode(brakeSwitchPin, INPUT_PULLUP);

  // output LEDs (or transistors to LEDs)
  pinMode(engineLedPin, OUTPUT);
  pinMode(headlightLedsPin, OUTPUT);
  pinMode(indicatorLeftLedsPin, OUTPUT);
  pinMode(indicatorRightLedsPin, OUTPUT);
  pinMode(brakeLedsPin, OUTPUT);

  // output PWM

  // engine on/off switch
  
}

// ----------------------------------------------------------------------------
// loop

void loop() {
  // always get the current ms
  unsigned long currentMs = millis();

  // Note: all switch readings are inverted due to being pull-ups.

  // read all switches
  isEngineOn = !digitalRead(engineSwitchPin);
  isIndicatorLeftOn = !digitalRead(indicatorLeftSwitchPin);
  isIndicatorRightOn = !digitalRead(indicatorRightSwitchPin);
  isHeadlightsOn = !digitalRead(headlightsSwitchPin);
  isBrakeOn = !digitalRead(brakeSwitchPin);

  // if the engine is off, then make sure all other lights are off
  digitalWrite(engineLedPin, isEngineOn);
  if ( !isEngineOn ) {
    indicatorOnInMs = 0;
    digitalWrite(headlightLedsPin, LOW);
    digitalWrite(indicatorLeftLedsPin, LOW);
    digitalWrite(indicatorRightLedsPin, LOW);
    digitalWrite(brakeLedsPin, LOW);
    delay(loopDelayMs);
    return;
  }

  // engine is on, so allow any and all lights to be on
  // ToDo: ... !!!

  // turn on the headlights (for now)
  digitalWrite(headlightLedsPin, isHeadlightsOn);

  // check if either indicator is on
  if ( isIndicatorLeftOn || isIndicatorRightOn ) {
    // start the indicator timer if not already on
    if ( !indicatorOnInMs ) {
      indicatorOnInMs = currentMs;
    }

    // time since indicator was on
    long timeSinceOnMs = currentMs - indicatorOnInMs;
    long indicatorWhereabouts = timeSinceOnMs % indicatorPeriodInMs;
    if ( indicatorWhereabouts < indicatorPeriodInMs / 2 ) {
      // on (but only if that particular indicator is on)
      digitalWrite(indicatorLeftLedsPin, isIndicatorLeftOn);
      digitalWrite(indicatorRightLedsPin, isIndicatorRightOn);
    }
    else {
      // off
      digitalWrite(indicatorLeftLedsPin, LOW);
      digitalWrite(indicatorRightLedsPin, LOW);
    }
  }
  else {
    // both indicators are off, so reset the timer
    indicatorOnInMs = 0;
    digitalWrite(indicatorLeftLedsPin, LOW);
    digitalWrite(indicatorRightLedsPin, LOW);
  }

  // turn on the brakelights
  digitalWrite(brakeLedsPin, isBrakeOn);

  delay(loopDelayMs);
}

// ----------------------------------------------------------------------------
