#ifndef ENGINE_INPUT
#define ENGINE_INPUT

struct Stick {
  float x;
  float y;
};

struct Button {
  bool is_pressed;
};

struct Controller {
  bool is_connected;

  Stick stick;

  Button move_up;
  Button move_down;
  Button move_left;
  Button move_right;

  Button action_up;
  Button action_down;
  Button action_left;
  Button action_right;

  Button start;
  Button back;
};

struct Input {
  Controller controllers[4];
};

#endif
