/*
From http://www.instructables.com/id/Arduino-Button-Tutorial/
*/

#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(int p, int lp)
: pin(p), longpress_len(lp)
{
}

void ButtonHandler::init()
{
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH); // pull-up
  was_pressed = false;
  pressed_counter = 0;
}

int ButtonHandler::handle()
{
  int event;
  int now_pressed = !digitalRead(pin);

  if (!now_pressed && was_pressed) {
    // handle release event
    if (pressed_counter < longpress_len)
      event = EV_SHORTPRESS;
    else
      event = EV_LONGPRESS;
  }
  else
    event = EV_NONE;

  // update press running duration
  if (now_pressed)
    ++pressed_counter;
  else
    pressed_counter = 0;

  // remember state, and we're done
  was_pressed = now_pressed;
  return event;
}

// handle pressed, and still pressed
int ButtonHandler::handlePressed()
{
  int event;
  int now_pressed = !digitalRead(pin);

  if (now_pressed)
  {
    if (was_pressed)
      event = EV_STILLPRESS;
    else
      event = EV_SHORTPRESS;
  }
  else
    event = EV_NONE;

  if (now_pressed)
    ++pressed_counter;
  else
    pressed_counter = 0;

  // remember state, and we're done
  was_pressed = now_pressed;
  return event;
}
