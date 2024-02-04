/*
 *  Project                    Arcade Machine -> Arduino Leo Mapping
 *  @XInput Library Author     David Madison
 *  @link       partsnotincluded.com/tutorials/how-to-emulate-an-xbox-controller-with-arduino-xinput
 *  @license    MIT - Copyright (c) 2019 David Madison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <XInput.h>
// Input Pins
const uint8_t a0 = A0;
const uint8_t a1 = A1;
const uint8_t a2 = A2;

const uint8_t d3  = 3;
const uint8_t d4  = 4;
const uint8_t d5  = 5;
const uint8_t d6  = 6;

// Analog Input Range
const int AnalogRead_Max = 825;
void setup() {
	// Set input pin modes
	pinMode(d3, INPUT_PULLUP);
	pinMode(d4, INPUT_PULLUP);
  pinMode(d5, INPUT_PULLUP);
  pinMode(d6, INPUT_PULLUP);

	// Setup library
  XInput.setTriggerRange(0, AnalogRead_Max);
  XInput.setJoystickRange(-350, 350);
	XInput.begin();
}

unsigned long initialPress = 0;
boolean depressed = false;

void loop() {
	// Read pin states
	boolean press_d3 = !digitalRead(d3);
  boolean press_d4 = !digitalRead(d4);
  boolean press_d5 = !digitalRead(d5);
  boolean press_d6 = !digitalRead(d6);

	int joy_a0 = analogRead(a0);
  int joy_a1 = analogRead(a1);
  int joy_a2 = analogRead(a2);
  
	// Set button and trigger states
	XInput.setButton(BUTTON_B, press_d3);

  //TODO: button debouncing on d4
  XInput.setButton(BUTTON_X, press_d4);
  XInput.setButton(BUTTON_START, press_d5);
  XInput.setButton(BUTTON_BACK, press_d6);

  XInput.setTrigger(TRIGGER_LEFT, joy_a1);
  XInput.setTrigger(TRIGGER_RIGHT, joy_a2);

  int scaledValue = map(joy_a0, 0, 1023, -500, 500);
  scaledValue -= 12.5; //Error correction
  if (scaledValue >= 0) {
    scaledValue = map(scaledValue, 0, 500, 120, 500);
  }
  else {
    scaledValue = map(scaledValue, -500, 0, -500, -120);
  }
	XInput.setJoystick(JOY_LEFT, scaledValue, 0);  // move x, leave y centered



  // Time-based condition
  if (press_d4) {
    if (!depressed) {
      initialPress = millis(); // Record the start time
      depressed = true;
    }

    // Check if the time duration has passed (0.2 seconds = 200 milliseconds)
    else if (millis() - initialPress >= 300) {
      //XInput.setButton(BUTTON_A, true);
      XInput.setButton(BUTTON_A, true);
    }
  }
  else {
    XInput.setButton(BUTTON_A, false);
    depressed = false;
  }



  
	// // Get rumble value
	// uint16_t rumble = XInput.getRumble();
	// // If controller is rumbling, turn on LED
	// if (rumble > 0) {
	// 	digitalWrite(Pin_LED, HIGH);
	// }
	// else {
	// 	digitalWrite(Pin_LED, LOW);
	// }
}