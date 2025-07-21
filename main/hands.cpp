#include "hands.h"

Hands::Hands(int leftPin, int rightPin) : leftPin(leftPin), rightPin(rightPin) {
}

void Hands::begin() {
  servoLeft.attach(leftPin);
  servoRight.attach(rightPin);
  servoLeft.write(90);
  servoRight.write(90);
  randomSeed(analogRead(0));
}

void Hands::idle() {
  if (!isTransitioning && millis() - lastComboChange >= currentComboDuration) {
    isTransitioning = true;
    targetLeft = posDegrees;
    targetRight = (idleCombo == 1) ? 180 - posDegrees : posDegrees;
    lastComboChange = millis();
  }

  if (isTransitioning) {
    if (millis() - previousMillis >= idleInterval) {
      previousMillis = millis();
      if (targetLeft < 90) {
        targetLeft++;
      } else if (targetLeft > 90) {
        targetLeft--;
      }
      if (targetRight < 90) {
        targetRight++;
      } else if (targetRight > 90) {
        targetRight--;
      }
      servoLeft.write(targetLeft);
      servoRight.write(targetRight);
      if (targetLeft == 90 && targetRight == 90) {
        if (transitionStartTime == 0) {
          transitionStartTime = millis();
        }
        if (millis() - transitionStartTime >= transitionPause) {
          isTransitioning = false;
          transitionStartTime = 0;
          idleCombo = random(1, 3);
          currentComboDuration = random(minComboDuration, maxComboDuration + 1);
          lastComboChange = millis();
          posDegrees = 90;
        }
      }
    }
  } else {
    if (millis() - previousMillis >= idleInterval) {
      previousMillis = millis();
      servoLeft.write(posDegrees);
      if (idleCombo == 1) {
        servoRight.write(180 - posDegrees);
      } else {
        servoRight.write(posDegrees);
      }
      if (increasing) {
        posDegrees++;
        if (posDegrees >= 100) {
          increasing = false;
        }
      } else {
        posDegrees--;
        if (posDegrees <= 70) {
          increasing = true;
        }
      }
    }
  }
}

void Hands::happy() {
  if (!isTransitioningHappy && millis() - lastHappyComboChange >= currentHappyComboDuration) {
    isTransitioningHappy = true;
    targetLeft = posDegrees;
    targetRight = (happyCombo == 1) ? 180 - posDegrees : 150 + posDegrees;
    lastHappyComboChange = millis();
  }

  if (isTransitioningHappy) {
    if (millis() - previousMillis >= interval) {
      previousMillis = millis();
      if (targetLeft < 30) {
        targetLeft++;
      } else if (targetLeft > 30) {
        targetLeft--;
      }
      if (targetRight < 150) {
        targetRight++;
      } else if (targetRight > 150) {
        targetRight--;
      }
      servoLeft.write(targetLeft);
      servoRight.write(targetRight);
      if (targetLeft == 30 && targetRight == 150) {
        if (transitionStartTimeHappy == 0) {
          transitionStartTimeHappy = millis();
        }
        if (millis() - transitionStartTimeHappy >= currentHappyTransitionPause) {
          isTransitioningHappy = false;
          transitionStartTimeHappy = 0;
          happyCombo = random(1, 3);
          currentHappyComboDuration = random(minHappyComboDuration, maxHappyComboDuration + 1);
          currentHappyTransitionPause = random(minHappyTransitionPause, maxHappyTransitionPause + 1);
          lastHappyComboChange = millis();
          posDegrees = 30;
        }
      }
    }
  } else {
    if (millis() - previousMillis >= interval) {
      previousMillis = millis();
      servoLeft.write(posDegrees);
      if (happyCombo == 1) {
        servoRight.write(180 - posDegrees);
      } else {
        servoRight.write(150 + posDegrees);
      }
      if (increasing) {
        posDegrees++;
        if (posDegrees >= 30) {
          increasing = false;
        }
      } else {
        posDegrees--;
        if (posDegrees <= 0) {
          increasing = true;
        }
      }
    }
  }
}

void Hands::dance() {
  if (millis() - previousMillis >= danceInterval) {
    previousMillis = millis();
    servoLeft.write(posDegrees);
    servoRight.write(posDegrees);
    if (increasing) {
      posDegrees++;
      if (posDegrees >= 180) {
        increasing = false;
      }
    } else {
      posDegrees--;
      if (posDegrees <= 0) {
        increasing = true;
      }
    }
  }
}

void Hands::mad() {
  if (millis() - previousMillis >= danceInterval) {
    previousMillis = millis();
    servoLeft.write(posDegrees);
    servoRight.write(170 + (posDegrees - 85));
    if (increasing) {
      posDegrees++;
      if (posDegrees >= 95) {
        increasing = false;
      }
    } else {
      posDegrees--;
      if (posDegrees <= 85) {
        increasing = true;
      }
    }
  }
}

void Hands::sad() {
  if (millis() - previousMillis >= madInterval) {
    previousMillis = millis();
    servoLeft.write(posDegrees);
    servoRight.write(180 - posDegrees);
    if (increasing) {
      posDegrees++;
      if (posDegrees >= 120) {
        increasing = false;
      }
    } else {
      posDegrees--;
      if (posDegrees <= 80) {
        increasing = true;
      }
    }
  }
}

void Hands::answer() {
  if (!isTransitioningAnswer && millis() - lastAnswerComboChange >= currentAnswerComboDuration) {
    isTransitioningAnswer = true;
    targetLeft = posDegrees;
    targetRight = (answerCombo == 1) ? 180 - posDegrees : 120 + posDegrees;
    lastAnswerComboChange = millis();
  }

  if (isTransitioningAnswer) {
    if (millis() - previousMillis >= interval) {
      previousMillis = millis();
      if (targetLeft < 60) {
        targetLeft++;
      } else if (targetLeft > 60) {
        targetLeft--;
      }
      if (targetRight < 120) {
        targetRight++;
      } else if (targetRight > 120) {
        targetRight--;
      }
      servoLeft.write(targetLeft);
      servoRight.write(targetRight);
      if (targetLeft == 60 && targetRight == 120) {
        if (transitionStartTimeAnswer == 0) {
          transitionStartTimeAnswer = millis();
        }
        if (millis() - transitionStartTimeAnswer >= currentAnswerTransitionPause) {
          isTransitioningAnswer = false;
          transitionStartTimeAnswer = 0;
          answerCombo = random(1, 3);
          currentAnswerComboDuration = random(minAnswerComboDuration, maxAnswerComboDuration + 1);
          currentAnswerTransitionPause = random(minAnswerTransitionPause, maxAnswerTransitionPause + 1);
          lastAnswerComboChange = millis();
          posDegrees = 60;
        }
      }
    }
  } else {
    if (millis() - previousMillis >= interval) {
      previousMillis = millis();
      servoLeft.write(posDegrees);
      if (answerCombo == 1) {
        servoRight.write(180 - posDegrees);
      } else {
        servoRight.write(120 + posDegrees);
      }
      if (increasing) {
        posDegrees++;
        if (posDegrees >= 60) {
          increasing = false;
        }
      } else {
        posDegrees--;
        if (posDegrees <= 0) {
          increasing = true;
        }
      }
    }
  }
}

void Hands::standBy() {
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    if (targetLeft < 90) {
      targetLeft++;
    } else if (targetLeft > 90) {
      targetLeft--;
    }
    if (targetRight < 90) {
      targetRight++;
    } else if (targetRight > 90) {
      targetRight--;
    }
    servoLeft.write(targetLeft);
    servoRight.write(targetRight);
    if (targetLeft == 90 && targetRight == 90) {
      posDegrees = 90;
    }
  }
}

void Hands::hearingQuestion() {
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    if (targetLeft < 30) {
      targetLeft++;
    } else if (targetLeft > 30) {
      targetLeft--;
    }
    if (targetRight < 150) {
      targetRight++;
    } else if (targetRight > 150) {
      targetRight--;
    }
    servoLeft.write(targetLeft);
    servoRight.write(targetRight);
    if (targetLeft == 30 && targetRight == 150) {
      posDegrees = 30;
    }
  }
}

void Hands::testHands() {
  unsigned long startTime = millis();
  unsigned long testDuration = 5000; // 5 seconds per mode

  // Test idle mode
  while (millis() - startTime < testDuration) {
    idle();
  }
  standBy(); // Return to neutral position
  delay(1000); // Brief pause between modes

  // Test dance mode
  startTime = millis();
  while (millis() - startTime < testDuration) {
    dance();
  }
  standBy();
  delay(1000);

  // Test happy mode
  startTime = millis();
  while (millis() - startTime < testDuration) {
    happy();
  }
  standBy();
  delay(1000);

  // Test mad mode
  startTime = millis();
  while (millis() - startTime < testDuration) {
    mad();
  }
  standBy();
  delay(1000);

  // Test sad mode
  startTime = millis();
  while (millis() - startTime < testDuration) {
    sad();
  }
  standBy();
  delay(1000);

  // Test answer mode
  startTime = millis();
  while (millis() - startTime < testDuration) {
    answer();
  }
  standBy();
  delay(1000);

  // Test hearingQuestion mode
  startTime = millis();
  while (millis() - startTime < testDuration) {
    hearingQuestion();
  }
  standBy();
}