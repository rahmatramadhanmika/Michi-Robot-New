#ifndef hands_h
#define hands_h

#include <Arduino.h>
#include <ESP32Servo.h>

class Hands {
public:
  Hands(int leftPin = 2, int rightPin = 15);
  void begin();
  void idle();
  void dance();
  void happy();
  void mad();
  void sad();
  void answer();
  void standBy();
  void hearingQuestion();
  void testHands(); // New function to test all hand functionalities

private:
  Servo servoLeft;
  Servo servoRight;
  int leftPin;
  int rightPin;

  int posDegrees = 90;
  bool increasing = true;
  unsigned long previousMillis = 0;
  const long interval = 15;
  const long danceInterval = 10;
  const long idleInterval = 10;
  const long madInterval = 50;

  int targetLeft = 90;
  int targetRight = 90;

  // Idle mode variables
  int idleCombo = 1;
  unsigned long lastComboChange = 0;
  const long minComboDuration = 3000;
  const long maxComboDuration = 10000;
  unsigned long currentComboDuration = minComboDuration;
  bool isTransitioning = false;
  unsigned long transitionStartTime = 0;
  const long transitionPause = 1000;

  // Happy mode variables
  int happyCombo = 1;
  unsigned long lastHappyComboChange = 0;
  const long minHappyComboDuration = 5000;
  const long maxHappyComboDuration = 10000;
  unsigned long currentHappyComboDuration = minHappyComboDuration;
  bool isTransitioningHappy = false;
  unsigned long transitionStartTimeHappy = 0;
  const long minHappyTransitionPause = 3000;
  const long maxHappyTransitionPause = 5000;
  unsigned long currentHappyTransitionPause = minHappyTransitionPause;

  // Answer mode variables
  int answerCombo = 1;
  unsigned long lastAnswerComboChange = 0;
  const long minAnswerComboDuration = 5000;
  const long maxAnswerComboDuration = 10000;
  unsigned long currentAnswerComboDuration = minAnswerComboDuration;
  bool isTransitioningAnswer = false;
  unsigned long transitionStartTimeAnswer = 0;
  const long minAnswerTransitionPause = 3000;
  const long maxAnswerTransitionPause = 5000;
  unsigned long currentAnswerTransitionPause = minAnswerTransitionPause;
};

#endif