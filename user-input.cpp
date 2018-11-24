#include <TaskAction.h>

#include "user-input.h"

static const uint8_t RESET_PIN = 21;

static uint8_t s_debouncer = 0;
static bool s_pressed = false;

static void debounce_task_fn(TaskAction * pThisTask)
{
  (void)pThisTask;
  bool pressed = digitalRead(RESET_PIN) == LOW;
  if (pressed && (s_debouncer < 5))
  {
    s_debouncer++;
    s_pressed = s_debouncer == 5;
  }
  else if (!pressed && (s_debouncer > 0))
  {
    s_debouncer--;
  }
}
static TaskAction s_debounce_task(debounce_task_fn, 20, INFINITE_TICKS);

void user_input_setup()
{
  pinMode(RESET_PIN, INPUT_PULLUP);
}

void user_input_loop()
{
  s_debounce_task.tick();
}

bool user_input_check_and_clear()
{
  bool pressed = s_pressed;
  s_pressed = false;
  return pressed;
}

