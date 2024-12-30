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

#include <Arduino.h>
#include <XInput.h>

/* Configuration */
#define DEBOUNCE_MS 50
#define PEDAL_MAX 825

/* Type definitions */
enum control_mode {
  GRAN_TURISMO,
  FORZA,
  X,
  N_MODES
};

struct control { /* 3-pos switch */
  uint8_t pin1;
  uint8_t pin2;
  enum control_mode mode;
};

enum analog_type {
  TRIGGER,
  JOYSTICK
};

struct analog {
  uint8_t pin;
  enum XInputControl control;
  int val;
  enum analog_type type;
};

struct digital {
  uint8_t pin;
  enum XInputControl control;
  bool pressed;
  unsigned long t_pressed;
  bool rising;
  unsigned long t_rising;
};

/* Globals */
struct control ctrl = {8, 9, GRAN_TURISMO};

struct analog aio[] = {
  {A0, JOY_LEFT,      0,  JOYSTICK},
  {A1, TRIGGER_LEFT,  0,  TRIGGER},
  {A2, TRIGGER_RIGHT, 0,  TRIGGER}
};
uint8_t n_aio = sizeof(aio) / sizeof(struct analog);

struct digital dio[N_MODES][4] = {
  /* GRAN_TURISMO */
  {{3, BUTTON_B,     false, 0, false, 0},
   {4, BUTTON_X,     false, 0, false, 0},
   {5, BUTTON_START, false, 0, false, 0},
   {6, BUTTON_BACK,  false, 0, false, 0}
  },
  /* FORZA */
  {{3, BUTTON_B,     false, 0, false, 0},
   {4, BUTTON_X,     false, 0, false, 0},
   {5, BUTTON_START, false, 0, false, 0},
   {6, BUTTON_BACK,  false, 0, false, 0}
  },
  /* X */
  {}
};
uint8_t n_dio = sizeof(dio[0]) / sizeof(dio[0][0]);


void setup() {
  /* Analog */
  XInput.setTriggerRange(42, PEDAL_MAX);
  XInput.setJoystickRange(-350, 350);

  /* Digital */
  for (int i = 0; i < n_dio; i++) {
    pinMode(dio[ctrl.mode][i].pin, INPUT_PULLUP);
  }
  pinMode(ctrl.pin1, INPUT_PULLUP);
  pinMode(ctrl.pin2, INPUT_PULLUP);

  XInput.begin();
}


void loop() {
  /* Control */
  bool left = !digitalRead(ctrl.pin1);
  bool right = !digitalRead(ctrl.pin2);
  bool center = !(left || right);
  if (left)
    ctrl.mode = GRAN_TURISMO;
  else if (center)
    ctrl.mode = FORZA;
  else
    ctrl.mode = X;


  /* Analog */
  for (int i = 0; i < n_aio; i++) {
    struct analog &input = aio[i];
    input.val = analogRead(input.pin);

    switch(input.type) {
      case TRIGGER:
      XInput.setTrigger(input.control, input.val);
      break;

      case JOYSTICK:
      input.val = map(input.val, 0, 1023, -500, 500);
      input.val -= 12.5;  //Error correction
      if (input.val >= 0) {
        input.val = map(input.val, 0, 500, 60, 500);
      } else {
        input.val = map(input.val, -500, 0, -500, -60);
      }
      XInput.setJoystick(input.control, input.val, 0);
      break;
    }
  }


  /* Digital */
  for (int i = 0; i < n_dio; i++) {
    struct digital &btn = dio[ctrl.mode][i];
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