#ifndef eyes_h
#define eyes_h

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

class eyes {
public:
  eyes(int screen_width, int screen_height, int oled_reset = -1, int screen_address = 0x3C);

  void begin();
  void drawEyes(bool update = true);
  void centerEyes(bool update = true);
  void sleep();
  void wakeup();
  void questioning();
  void startBlink(int speed = 10);
  void updateBlink();
  void startHappy();
  void updateHappy();
  void startMad();
  void updateMad();
  void startSad();
  void updateSad();
  void startBored();
  void updateBored();
  void startExcited();
  void updateExcited();
  void startQuestion();
  void updateQuestion();
  void startMoveBigEye(int direction);
  void updateMoveBigEye();
  bool isBlinking() const {
    return blinking;
  }
  bool isDuringBlink() const {
    return closing || (blinking && !closing);
  }
  void cancelBlink();
  void startIdle();
  void updateIdle();
  bool isExpressionActive() const {
    return expressionActive;
  }
  void cancelExpression();

private:
  Adafruit_SH1106G display;
  int screen_width;
  int screen_height;

  // Reference state
  int ref_eye_height = 40;
  int ref_eye_width = 45;
  int ref_space_between_eye = 10;
  int ref_corner_radius = 15;

  // Tambahan untuk non-blocking blink
  bool blinking = false;
  bool closing = true;  // fase: true = mengecil, false = membuka
  unsigned long blinkPrevMillis = 0;
  int blinkSpeed = 3;
  int blinkStep = 5;

  // Untuk idle animation
  bool idleActive = false;
  unsigned long idleLastBlink = 0;
  unsigned long nextIdleTime = 0;
  int plannedBlinks = 0;
  int completedBlinks = 0;
  unsigned long nextBlinkTime = 0;
  unsigned long blinkPauseDuration = 0;

  // Untuk happy animation
  bool happyActive = false;
  int happyOffset = 0;
  unsigned long happyPrevMillis = 0;
  int happyStepDelay = 1;
  int happyPhase = 0;  // 0 = animasi segitiga, 1 = delay akhir

  bool madRunning = false;
  unsigned long madLastUpdate = 0;
  int madYOffset = -13;

  bool sadRunning = false;
  unsigned long sadLastUpdate = 0;
  int sadYOffset = -13;

  bool boredRunning = false;
  unsigned long boredLastUpdate = 0;
  int boredYOffset = -15;

  bool excitedRunning = false;
  unsigned long excitedLastUpdate = 0;
  int excitedY = 78;  // posisi awal lingkaran

  // Untuk question animation
  bool questionRunning = false;
  unsigned long questionLastUpdate = 0;
  int questionStep = 0;
  const int totalQuestionSteps = 20;  // Number of animation steps

  unsigned long previousEyeUpdate = 0;
  int eyeAnimationPhase = 0;
  int eyeAnimationDirection = 0;
  bool eyeReturning = false;
  int eyeAnimationCounter = 0;
  bool waitingToReturn = false;
  unsigned long waitStartTime = 0;

  bool expressionActive = false;
  int currentExpression = 0;  // 0=none, 1=happy, 2=bored, 3=excited, 4=moveBigEye, 5=question, 6=mad, 7=sad
  unsigned long expressionStartTime = 0;
  unsigned long expressionDuration = 0;

  // Current state
  int left_eye_height;
  int left_eye_width;
  int left_eye_x;
  int left_eye_y;
  int right_eye_x;
  int right_eye_y;
  int right_eye_height;
  int right_eye_width;
};

#endif