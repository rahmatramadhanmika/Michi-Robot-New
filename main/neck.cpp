#include "neck.h"

Neck::Neck(int servoPin)
  : servoPin(servoPin) {
}

void Neck::begin() {
  servo1.attach(servoPin);
  servo1.write(90);
  randomSeed(analogRead(0));
}

void Neck::idle() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateMillis < interval) {
    return;
  }
  lastUpdateMillis = currentMillis;

  if (!waitingAtTarget && !returningToCenter && currentMillis - lastMoveMillis >= random(5000, 10001)) {
    targetDegrees = random(45, 136);
    lastMoveMillis = currentMillis;
  }

  if (posDegrees != targetDegrees && !waitingAtTarget && !returningToCenter) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      waitingAtTarget = true;
      waitStartMillis = currentMillis;
    }
  } else if (waitingAtTarget && currentMillis - waitStartMillis >= random(2000, 3001)) {
    waitingAtTarget = false;
    returningToCenter = true;
    targetDegrees = 90;
  } else if (returningToCenter && posDegrees != targetDegrees) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      returningToCenter = false;
      lastMoveMillis = currentMillis;
    }
  }
}

void Neck::answer() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateMillis < answerInterval) {
    return;
  }
  lastUpdateMillis = currentMillis;

  if (!waitingAtTarget && !returningToCenter && currentMillis - lastMoveMillis >= random(7000, 10001)) {
    targetDegrees = random(60, 121);
    lastMoveMillis = currentMillis;
  }

  if (posDegrees != targetDegrees && !waitingAtTarget && !returningToCenter) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      waitingAtTarget = true;
      waitStartMillis = currentMillis;
    }
  } else if (waitingAtTarget && currentMillis - waitStartMillis >= random(1000, 2001)) {
    waitingAtTarget = false;
    returningToCenter = true;
    targetDegrees = 90;
  } else if (returningToCenter && posDegrees != targetDegrees) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      returningToCenter = false;
      lastMoveMillis = currentMillis;
    }
  }
}

void Neck::sad() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateMillis < sadInterval) {
    return;
  }
  lastUpdateMillis = currentMillis;

  if (!waitingAtTarget && !returningToCenter && currentMillis - lastMoveMillis >= random(2000, 3001)) {
    targetDegrees = movingTo110 ? 110 : 80;
    movingTo110 = !movingTo110;
    lastMoveMillis = currentMillis;
  }

  if (posDegrees != targetDegrees && !waitingAtTarget && !returningToCenter) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      waitingAtTarget = true;
      waitStartMillis = currentMillis;
    }
  } else if (waitingAtTarget && currentMillis - waitStartMillis >= random(1000, 2001)) {
    waitingAtTarget = false;
    returningToCenter = true;
    targetDegrees = 90;
  } else if (returningToCenter && posDegrees != targetDegrees) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      returningToCenter = false;
      lastMoveMillis = currentMillis;
    }
  }
}

void Neck::happy() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateMillis < answerInterval) {
    return;
  }
  lastUpdateMillis = currentMillis;

  if (!waitingAtTarget && !returningToCenter && currentMillis - lastMoveMillis >= random(2000, 3001)) {
    targetDegrees = random(60, 121);
    lastMoveMillis = currentMillis;
  }

  if (posDegrees != targetDegrees && !waitingAtTarget && !returningToCenter) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      waitingAtTarget = true;
      waitStartMillis = currentMillis;
    }
  } else if (waitingAtTarget && currentMillis - waitStartMillis >= random(1000, 2001)) {
    waitingAtTarget = false;
    returningToCenter = true;
    targetDegrees = 90;
  } else if (returningToCenter && posDegrees != targetDegrees) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      returningToCenter = false;
      lastMoveMillis = currentMillis;
    }
  }
}

void Neck::standBy() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateMillis < interval) {
    return;
  }
  lastUpdateMillis = currentMillis;

  if (posDegrees != 90) {
    if (posDegrees > 90) {
      posDegrees--;
    } else if (posDegrees < 90) {
      posDegrees++;
    }
    servo1.write(posDegrees);
  }
}

void Neck::detectWakeword() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateMillis < interval) {
    return;
  }
  lastUpdateMillis = currentMillis;

  // If at 90 degrees and no target set, start new cycle after random delay (2-3 seconds)
  if (!waitingAtTarget && !returningToCenter && posDegrees == 90 && targetDegrees == 90 && currentMillis - lastMoveMillis >= random(2000, 3001)) {
    targetDegrees = 120; // Start by moving to 120 degrees
    lastMoveMillis = currentMillis;
  }

  // Move towards target
  if (posDegrees != targetDegrees && !waitingAtTarget && !returningToCenter) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      waitingAtTarget = true;
      waitStartMillis = currentMillis;
    }
  }
  // Wait at target (120 or 60 degrees) for 1 second
  else if (waitingAtTarget && currentMillis - waitStartMillis >= 1000) {
    waitingAtTarget = false;
    if (targetDegrees == 120) {
      targetDegrees = 60; // Move to 60 degrees
    } else if (targetDegrees == 60) {
      targetDegrees = 90; // Return to 90 degrees
      returningToCenter = true;
    }
    lastMoveMillis = currentMillis;
  }
  // Return to center (90 degrees)
  else if (returningToCenter && posDegrees != targetDegrees) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      returningToCenter = false;
      lastMoveMillis = currentMillis; // Reset timer for random delay
    }
  }
}

void Neck::thinking() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateMillis < interval) {
    return;
  }
  lastUpdateMillis = currentMillis;

  // If at target and waiting, check if wait time (3-5 seconds) has elapsed
  if (waitingAtTarget && currentMillis - waitStartMillis >= random(3000, 5001)) {
    waitingAtTarget = false;
    targetDegrees = random(30, 151); // Select new random target between 30 and 150 degrees
    lastMoveMillis = currentMillis;
  }

  // If not waiting and at target, start waiting
  if (!waitingAtTarget && posDegrees == targetDegrees) {
    waitingAtTarget = true;
    waitStartMillis = currentMillis;
  }

  // Move towards target
  if (posDegrees != targetDegrees && !waitingAtTarget) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      waitingAtTarget = true;
      waitStartMillis = currentMillis;
      lastMoveMillis = currentMillis; // Reset timer for next delay
    }
  }
}

void Neck::testNeck() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdateMillis < interval) {
    return;
  }
  lastUpdateMillis = currentMillis;

  // If at target and waiting, check if wait time (3-5 seconds) has elapsed
  if (waitingAtTarget && currentMillis - waitStartMillis >= random(3000, 5001)) {
    waitingAtTarget = false;
    targetDegrees = random(0, 181); // Select new random target between 30 and 150 degrees
    lastMoveMillis = currentMillis;
  }

  // If not waiting and at target, start waiting
  if (!waitingAtTarget && posDegrees == targetDegrees) {
    waitingAtTarget = true;
    waitStartMillis = currentMillis;
  }

  // Move towards target
  if (posDegrees != targetDegrees && !waitingAtTarget) {
    if (posDegrees < targetDegrees) {
      posDegrees++;
    } else {
      posDegrees--;
    }
    servo1.write(posDegrees);
    if (posDegrees == targetDegrees) {
      waitingAtTarget = true;
      waitStartMillis = currentMillis;
      lastMoveMillis = currentMillis; // Reset timer for next delay
    }
  }
}