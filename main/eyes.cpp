#include "eyes.h"

eyes::eyes(int screen_width, int screen_height, int oled_reset, int screen_address)
  : display(screen_width, screen_height, &Wire, oled_reset),
    screen_width(screen_width),
    screen_height(screen_height) {
  // Initialize member variables
  left_eye_height = ref_eye_height;
  left_eye_width = ref_eye_width;
  right_eye_height = ref_eye_height;
  right_eye_width = ref_eye_width;

  left_eye_x = screen_width / 2 - ref_eye_width / 2 - ref_space_between_eye / 2;
  left_eye_y = screen_height / 2;
  right_eye_x = screen_width / 2 + ref_eye_width / 2 + ref_space_between_eye / 2;
  right_eye_y = screen_height / 2;
}

void eyes::begin() {
  display.begin(0x3C, true);
  display.clearDisplay();
}

void eyes::drawEyes(bool update) {
  display.clearDisplay();
  // Draw left eye
  int x = int(left_eye_x - left_eye_width / 2);
  int y = int(left_eye_y - left_eye_height / 2);
  display.fillRoundRect(x, y, left_eye_width, left_eye_height, ref_corner_radius, SH110X_WHITE);

  // Draw right eye
  x = int(right_eye_x - right_eye_width / 2);
  y = int(right_eye_y - right_eye_height / 2);
  display.fillRoundRect(x, y, right_eye_width, right_eye_height, ref_corner_radius, SH110X_WHITE);

  if (update) {
    display.display();
  }
}

void eyes::centerEyes(bool update) {
  left_eye_height = ref_eye_height;
  left_eye_width = ref_eye_width;
  right_eye_height = ref_eye_height;
  right_eye_width = ref_eye_width;

  left_eye_x = screen_width / 2 - ref_eye_width / 2 - ref_space_between_eye / 2;
  left_eye_y = screen_height / 2;
  right_eye_x = screen_width / 2 + ref_eye_width / 2 + ref_space_between_eye / 2;
  right_eye_y = screen_height / 2;

  drawEyes(update);
}

void eyes::questioning() {
  display.clearDisplay();
  display.fillRoundRect(95, 1, 7, 15, 3, SH110X_WHITE);     // left block
  display.fillRoundRect(110, 1, 7, 25, 3, SH110X_WHITE);    // right long block
  display.fillRoundRect(95, 1, 20, 7, 3, SH110X_WHITE);     // middle top block
  display.fillRoundRect(100, 20, 15, 7, 3, SH110X_WHITE);   // middle mid block
  display.fillRoundRect(100, 20, 7, 15, 3, SH110X_WHITE);   // middle bottom block
  display.fillRoundRect(100, 45, 7, 7, 3, SH110X_WHITE);    // the dot
  display.fillRoundRect(7, 25, 35, 30, 10, SH110X_WHITE);   // left eye
  display.fillRoundRect(48, 18, 37, 32, 10, SH110X_WHITE);  // right eye
  if (!questionRunning) {                                   // Only display if not in animation
    display.display();
  }
}

void eyes::startQuestion() {
  if (expressionActive) return;  // Prevent starting if another expression is active
  questionRunning = true;
  questionLastUpdate = millis();
  questionStep = 0;
  currentExpression = 5;  // Set expression to question
  expressionActive = true;
  expressionStartTime = millis();
  expressionDuration = random(2000, 5000);  // 2-5 seconds
}

void eyes::updateQuestion() {
  if (!questionRunning) return;

  unsigned long now = millis();
  if (now - questionLastUpdate < 50) return;  // 50ms per frame, ~1s total for 20 steps
  questionLastUpdate = now;

  // Initial state (from drawEyes)
  float start_left_x = left_eye_x - left_eye_width / 2;   // 36 - 45/2 = 13.5
  float start_left_y = left_eye_y - left_eye_height / 2;  // 32 - 40/2 = 12
  float start_left_width = 45;
  float start_left_height = 40;
  float start_right_x = right_eye_x - right_eye_width / 2;   // 91 - 45/2 = 68.5
  float start_right_y = right_eye_y - right_eye_height / 2;  // 12
  float start_right_width = 45;
  float start_right_height = 40;
  float start_radius = 15;

  // Final state (from questioning)
  float end_left_x = 7;
  float end_left_y = 25;
  float end_left_width = 35;
  float end_left_height = 30;
  float end_right_x = 48;
  float end_right_y = 18;
  float end_right_width = 37;
  float end_right_height = 32;
  float end_radius = 10;

  // Calculate interpolation factor
  float t = (float)questionStep / totalQuestionSteps;

  // Interpolate parameters
  float left_x = start_left_x + (end_left_x - start_left_x) * t;
  float left_y = start_left_y + (end_left_y - start_left_y) * t;
  float left_width = start_left_width + (end_left_width - start_left_width) * t;
  float left_height = start_left_height + (end_left_height - start_left_height) * t;
  float right_x = start_right_x + (end_right_x - start_right_x) * t;
  float right_y = start_right_y + (end_right_y - start_right_y) * t;
  float right_width = start_right_width + (end_right_width - start_right_width) * t;
  float right_height = start_right_height + (end_right_height - start_right_height) * t;
  float radius = start_radius + (end_radius - start_radius) * t;

  // Draw the frame
  display.clearDisplay();
  display.fillRoundRect((int)left_x, (int)left_y, (int)left_width, (int)left_height, (int)radius, SH110X_WHITE);
  display.fillRoundRect((int)right_x, (int)right_y, (int)right_width, (int)right_height, (int)radius, SH110X_WHITE);

  // Gradually introduce question mark elements in the second half of the animation
  if (t >= 0.5) {
    float alpha = (t - 0.5) * 2;                                              // Scale from 0 to 1 for last half
    if (alpha > 0) display.fillRoundRect(95, 1, 7, 15, 3, SH110X_WHITE);      // left block
    if (alpha > 0.2) display.fillRoundRect(110, 1, 7, 25, 3, SH110X_WHITE);   // right long block
    if (alpha > 0.4) display.fillRoundRect(95, 1, 20, 7, 3, SH110X_WHITE);    // middle top block
    if (alpha > 0.6) display.fillRoundRect(100, 20, 15, 7, 3, SH110X_WHITE);  // middle mid block
    if (alpha > 0.8) display.fillRoundRect(100, 20, 7, 15, 3, SH110X_WHITE);  // middle bottom block
    if (alpha > 0.9) display.fillRoundRect(100, 45, 7, 7, 3, SH110X_WHITE);   // the dot
  }

  display.display();

  // Update animation state
  questionStep++;
  if (questionStep > totalQuestionSteps) {
    questionRunning = false;
    questionStep = 0;
    questioning();  // Draw final state
  }

  // Check if expression duration has elapsed
  if (now - expressionStartTime >= expressionDuration) {
    expressionActive = false;
    questionRunning = false;
    centerEyes();  // Return to normal eyes
  }
}

void eyes::startBlink(int speed) {
  if (!blinking) {
    blinkSpeed = speed;
    blinking = true;
    closing = true;  // mulai dengan menutup
    blinkPrevMillis = millis();
  }
}

void eyes::updateBlink() {
  if (!blinking) return;

  unsigned long now = millis();
  if (now - blinkPrevMillis >= blinkSpeed) {
    blinkPrevMillis = now;

    if (closing) {
      left_eye_height -= blinkStep;
      right_eye_height -= blinkStep;

      if (left_eye_height <= 0) {
        left_eye_height = 0;
        right_eye_height = 0;
        closing = false;  // mulai buka
      }
    } else {
      left_eye_height += blinkStep;
      right_eye_height += blinkStep;

      if (left_eye_height >= ref_eye_height) {
        left_eye_height = ref_eye_height;
        right_eye_height = ref_eye_height;
        blinking = false;  // blink selesai
      }
    }

    drawEyes();
  }
}

void eyes::cancelBlink() {
  if (blinking) {
    blinking = false;
    closing = false;
    left_eye_height = ref_eye_height;
    right_eye_height = ref_eye_height;
    drawEyes();  // Immediately update display
  }
}

void eyes::sleep() {
  left_eye_height = 2;
  right_eye_height = 2;
  drawEyes(true);
}

void eyes::wakeup() {
  sleep();

  for (int h = 0; h <= ref_eye_height; h += 2) {
    left_eye_height = h;
    right_eye_height = h;
    drawEyes(true);
  }
}

void eyes::startHappy() {
  centerEyes(false);
  happyOffset = ref_eye_height / 2;
  happyPrevMillis = millis();
  happyActive = true;
  happyPhase = 0;
  currentExpression = 1;
  expressionActive = true;
  expressionStartTime = millis();
  expressionDuration = random(2000, 5000);  // 2-5 seconds
}

void eyes::updateHappy() {
  if (!happyActive) return;

  unsigned long now = millis();

  if (happyPhase == 0) {
    if (now - happyPrevMillis >= happyStepDelay && happyOffset >= -1) {
      happyPrevMillis = now;

      display.fillTriangle(left_eye_x - left_eye_width / 2 - 1, left_eye_y + happyOffset,
                           left_eye_x + left_eye_width / 2 + 1, left_eye_y + 5 + happyOffset,
                           left_eye_x - left_eye_width / 2 - 1, left_eye_y + ref_eye_height + happyOffset,
                           SH110X_BLACK);

      display.fillTriangle(right_eye_x + right_eye_width / 2 + 1, right_eye_y + happyOffset,
                           right_eye_x - right_eye_width / 2 - 1, right_eye_y + 5 + happyOffset,
                           right_eye_x + right_eye_width / 2 + 1, right_eye_y + ref_eye_height + happyOffset,
                           SH110X_BLACK);

      display.display();
      happyOffset -= 2;

      if (happyOffset < -1) {
        happyPhase = 1;  // Masuk delay 1 detik
        happyPrevMillis = now;
      }
    }
  } else if (happyPhase == 1) {
    if (now - happyPrevMillis >= 1000) {
      happyActive = false;
      if (now - expressionStartTime >= expressionDuration) {
        expressionActive = false;
        centerEyes();
      }
    }
  }
}

void eyes::startMad() {
  madRunning = true;
  madLastUpdate = millis();
  madYOffset = -13;
  currentExpression = 6;
  expressionActive = true;
  expressionStartTime = millis();
  expressionDuration = random(2000, 5000);  // 2-5 seconds
}

void eyes::updateMad() {
  if (!madRunning) return;

  unsigned long now = millis();
  if (now - madLastUpdate < 20) return;
  madLastUpdate = now;

  display.clearDisplay();
  drawEyes(false);

  int ax = 0, ay = 10;
  int bx = 65, by = 10;
  int cx = 65, cy = 40;

  int maxX = screen_width - 1;
  int ax_r = maxX - ax;
  int bx_r = maxX - bx;
  int cx_r = maxX - cx;

  display.fillTriangle(ax, ay + madYOffset, bx, by + madYOffset, cx, cy + madYOffset, SH110X_BLACK);
  display.fillTriangle(ax_r, ay + madYOffset, bx_r, by + madYOffset, cx_r, cy + madYOffset, SH110X_BLACK);

  display.display();

  madYOffset++;
  if (madYOffset > 0) {
    madRunning = false;
    if (now - expressionStartTime >= expressionDuration) {
      expressionActive = false;
      centerEyes();
    }
  }
}

void eyes::startSad() {
  sadRunning = true;
  sadLastUpdate = millis();
  sadYOffset = -13;
  currentExpression = 7;
  expressionActive = true;
  expressionStartTime = millis();
  expressionDuration = random(2000, 5000);  // 2-5 seconds
}

void eyes::updateSad() {
  if (!sadRunning) return;

  unsigned long now = millis();
  if (now - sadLastUpdate < 20) return;
  sadLastUpdate = now;

  display.clearDisplay();
  drawEyes(false);

  int ax = 5, ay = 10;
  int bx = 65, by = 10;
  int cx = 5, cy = 40;

  int maxX = screen_width - 1;
  int ax_r = maxX - ax;
  int bx_r = maxX - bx;
  int cx_r = maxX - cx;

  display.fillTriangle(ax, ay + sadYOffset, bx, by + sadYOffset, cx, cy + sadYOffset, SH110X_BLACK);
  display.fillTriangle(ax_r, ay + sadYOffset, bx_r, by + sadYOffset, cx_r, cy + sadYOffset, SH110X_BLACK);

  display.display();

  sadYOffset++;
  if (sadYOffset > 0) {
    sadRunning = false;
    if (now - expressionStartTime >= expressionDuration) {
      expressionActive = false;
      centerEyes();
    }
  }
}

void eyes::startBored() {
  boredRunning = true;
  boredLastUpdate = millis();
  boredYOffset = -15;
  currentExpression = 2;
  expressionActive = true;
  expressionStartTime = millis();
  expressionDuration = random(2000, 5000);  // 2-5 seconds
}

void eyes::updateBored() {
  if (!boredRunning) return;

  unsigned long now = millis();
  if (now - boredLastUpdate < 20) return;
  boredLastUpdate = now;

  display.clearDisplay();
  drawEyes(false);

  int finalX = 5;
  int finalY = 5;
  int width = 65;
  int height = 25;

  display.fillRect(finalX, finalY + boredYOffset, width, height, SH110X_BLACK);
  int mirrorX = screen_width - 1 - (finalX + width - 1);
  display.fillRect(mirrorX, finalY + boredYOffset, width, height, SH110X_BLACK);

  display.display();

  boredYOffset++;
  if (boredYOffset > 0) {
    boredRunning = false;
    if (now - expressionStartTime >= expressionDuration) {
      expressionActive = false;
      centerEyes();
    }
  }
}

void eyes::startExcited() {
  excitedRunning = true;
  excitedLastUpdate = millis();
  excitedY = 78;
  currentExpression = 3;
  expressionActive = true;
  expressionStartTime = millis();
  expressionDuration = random(2000, 5000);  // 2-5 seconds
}

void eyes::updateExcited() {
  if (!excitedRunning) return;

  unsigned long now = millis();
  if (now - excitedLastUpdate < 20) return;
  excitedLastUpdate = now;

  display.clearDisplay();
  drawEyes(false);

  display.fillCircle(35, excitedY, 35, SH110X_BLACK);
  int mirrored_x = (screen_width - 1) - 35;
  display.fillCircle(mirrored_x, excitedY, 35, SH110X_BLACK);

  display.display();

  excitedY--;
  if (excitedY < 65) {
    excitedRunning = false;
    if (now - expressionStartTime >= expressionDuration) {
      expressionActive = false;
      centerEyes();
    }
  }
}

void eyes::startMoveBigEye(int direction) {
  eyeAnimationDirection = direction;
  eyeAnimationPhase = 0;
  eyeAnimationCounter = 0;
  eyeReturning = false;
  previousEyeUpdate = millis();
  currentExpression = 4;
  expressionActive = true;
  expressionStartTime = millis();
  expressionDuration = random(2000, 5000);  // 2-5 seconds
}

void eyes::updateMoveBigEye() {
  if (eyeAnimationDirection == 0) return;

  unsigned long currentTime = millis();
  if (currentTime - previousEyeUpdate < 5) return;
  previousEyeUpdate = currentTime;

  int direction_movement_amplitude = 2;
  int blink_amplitude = 5;
  int direction_oversize = 1;

  if (waitingToReturn) {
    if (currentTime - waitStartTime >= 1600) {
      waitingToReturn = false;
      eyeReturning = true;
      eyeAnimationPhase = 0;
    }
    return;
  }

  if (!eyeReturning) {
    left_eye_x += direction_movement_amplitude * eyeAnimationDirection;
    right_eye_x += direction_movement_amplitude * eyeAnimationDirection;

    if (eyeAnimationPhase < 3) {
      right_eye_height -= blink_amplitude;
      left_eye_height -= blink_amplitude;
    } else {
      right_eye_height += blink_amplitude;
      left_eye_height += blink_amplitude;
    }

    if (eyeAnimationDirection > 0) {
      right_eye_height += direction_oversize;
      right_eye_width += direction_oversize;
    } else {
      left_eye_height += direction_oversize;
      left_eye_width += direction_oversize;
    }

    eyeAnimationPhase++;
    if (eyeAnimationPhase >= 6) {
      eyeAnimationPhase = 0;
      waitingToReturn = true;
      waitStartTime = currentTime;
    }
  } else {
    left_eye_x -= direction_movement_amplitude * eyeAnimationDirection;
    right_eye_x -= direction_movement_amplitude * eyeAnimationDirection;

    if (eyeAnimationPhase < 3) {
      right_eye_height -= blink_amplitude;
      left_eye_height -= blink_amplitude;
    } else {
      right_eye_height += blink_amplitude;
      left_eye_height += blink_amplitude;
    }

    if (eyeAnimationDirection > 0) {
      right_eye_height -= direction_oversize;
      right_eye_width -= direction_oversize;
    } else {
      left_eye_height -= direction_oversize;
      left_eye_width -= direction_oversize;
    }

    eyeAnimationPhase++;
    if (eyeAnimationPhase >= 6) {
      eyeAnimationDirection = 0;
      eyeReturning = false;
      if (currentTime - expressionStartTime >= expressionDuration) {
        expressionActive = false;
        centerEyes();
      } else {
        drawEyes();
      }
    }
  }

  drawEyes();
}

void eyes::startIdle() {
  if (expressionActive || idleActive) return;
  idleActive = true;
  idleLastBlink = millis();
  nextIdleTime = idleLastBlink + random(3000, 6000);  // Schedule first blink sequence
  plannedBlinks = 0;
  completedBlinks = 0;
}

void eyes::updateIdle() {
  if (!idleActive) return;

  unsigned long now = millis();

  // Update any ongoing blink first
  if (isBlinking()) {
    updateBlink();
    return;  // Keluar jika masih blinking
  }

  // Handle scheduled blink sequences
  if (plannedBlinks == 0 && now >= nextIdleTime) {
    // Time for a new blink sequence
    plannedBlinks = random(1, 4);  // 1, 2, or 3 blinks
    completedBlinks = 0;
    nextBlinkTime = now;  // Start immediately
  }

  // Handle blink sequence execution
  if (plannedBlinks > 0 && completedBlinks < plannedBlinks) {
    if (now >= nextBlinkTime) {
      // Start a new blink
      startBlink(random(8, 15));
      completedBlinks++;

      // Schedule next blink or next sequence
      if (completedBlinks < plannedBlinks) {
        nextBlinkTime = now + random(100, 300);  // Pause between blinks
      } else {
        // Sequence completed
        plannedBlinks = 0;
        nextIdleTime = now + random(3000, 6000);  // Schedule next sequence
      }
    }
  }
}

void eyes::cancelExpression() {
  if (expressionActive) {
    expressionActive = false;
    questionRunning = false;
    madRunning = false;
    sadRunning = false;
    boredRunning = false;
    excitedRunning = false;
    happyActive = false;
    eyeAnimationDirection = 0;
    eyeReturning = false;
    centerEyes();
  }
}