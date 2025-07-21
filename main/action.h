#ifndef action_h
#define action_h

#include <Arduino.h>
#include "hands.h"
#include "neck.h"
#include "eyes.h"

class Action {
public:
  Action(int leftHandPin = 2, int rightHandPin = 15, int neckPin = 4, int screen_width = 128, int screen_height = 64);
  void begin();
  void idle();
  void answer();
  void sad();
  void happy();
  void mad();
  void dance();
  void standBy();
  void detectWakeword();
  void hearingQuestion();
  void thinking();
  void update(); // Handle ongoing animations for hands, neck, and eyes

private:
  Hands hands;
  Neck neck;
  eyes myEyes; // Eyes object
  char currentAction; // Tracks current action for update
};

#endif