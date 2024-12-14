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

/* Configuration */
#define DEBOUNCE_MS 50;
#define PEDAL_MAX 825;

/* Type definitions */
enum analog_type = {
  TRIGGER,
  JOYSTICK
}

struct analog {
  uint8_t pin;
  enum XInputControl control;
  int val;
  enum analog_type type;
}

struct digital {
  uint8_t pin;
  enum XInputControl control;
  bool pressed = false;
  time_t t_pressed = 0;
  bool rising = false;
  time_t t_rising = 0;
}

struct analog aio[] = {
  {A0, JOY_LEFT,      0,  JOYSTICK},
  {A1, TRIGGER_LEFT,  0,  TRIGGER},
  {A2, TRIGGER_RIGHT, 0,  TRIGGER}
}
uint8_t n_aio = sizeof(aio) / sizeof(struct analog);

struct digital dio[] = { /* TODO: conditional mapping via 3-pos switch */
  {3, BUTTON_B,     false, 0, false, 0},
  {4, BUTTON_X,     false, 0, false, 0},
  {5, BUTTON_START, false, 0, false, 0},
  {6, BUTTON_BACK,  false, 0, false, 0}
}
uint8_t n_dio = sizeof(dio) / sizeof(struct digital);



void setup() {
  /* Analog */
  XInput.setTriggerRange(0, PEDAL_MAX);
  XInput.setJoystickRange(-350, 350);

  /* Digital */
  for (int i = 0; i < n_dio; i++) {
    pinMode(dio[i].pin, INPUT_PULLUP);
  }

  XInput.begin();
}



void loop() {
  /* Analog */
  for (int i = 0; i < n_aio; i++) {
    struct analog input = aio[i];
    input.val = analogRead(input.pin);

    switch(input.type) {
      case TRIGGER:
      XInput.setTrigger(input.control, input.val);
      break;

      case JOYSTICK:
      input.val = map(input.val, 0, 1023, -500, 500);
      input.val -= 12.5;  //Error correction
      if (input.val >= 0) {
        input.val = map(input.val, 0, 500, 120, 500);
      } else {
        input.val = map(input.val, -500, 0, -500, -120);
      }
      XInput.setJoystick(input.control, input.val, 0);
      break;
    }
  }


  /* Digital */
  for (int i = 0; i < n_dio; i++) {
    struct digital btn = dio[i];
    btn.pressed = !digitalRead(btn.pin);

    if (btn.pressed && !btn.rising) {
      /* rising edge */
      btn.t_pressed = millis();
      btn.rising = true;
    }
    else if (btn.pressed && (millis() - btn.t_rising >= DEBOUNCE_MS)) {
      /* active */
      XInput.setButton(btn.control, true);
    }
    else {
      /* falling edge */
      XInput.setButton(btn.control, false);
      btn.rising = false;
    }
  }


  /* Rumble control
  uint16_t rumble = XInput.getRumble();
  if (rumble > 0)
  	digitalWrite(Pin_LED, HIGH);
  else
  	digitalWrite(Pin_LED, LOW);
  */
}