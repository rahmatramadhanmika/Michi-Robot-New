#include "action.h"

Action::Action(int leftHandPin, int rightHandPin, int neckPin, int screen_width, int screen_height)
  : hands(leftHandPin, rightHandPin), neck(neckPin), myEyes(screen_width, screen_height), currentAction('s') {
}

void Action::begin() {
  hands.begin();
  neck.begin();
  myEyes.begin();
  myEyes.centerEyes();  // Initialize eyes to normal state
}

void Action::idle() {
  myEyes.cancelExpression();  // Cancel any ongoing expression
  myEyes.centerEyes();        // Immediately open eyes to default state
  currentAction = '1';
  hands.idle();
  neck.idle();
  myEyes.startIdle();
}

void Action::answer() {
  myEyes.cancelExpression();  // Cancel any ongoing expression
  myEyes.centerEyes();        // Immediately open eyes to default state
  currentAction = '2';
  hands.answer();
  neck.answer();
  myEyes.startExcited();
}

void Action::sad() {
  myEyes.cancelExpression();  // Cancel any ongoing expression
  myEyes.centerEyes();        // Immediately open eyes to default state
  currentAction = '5';
  hands.sad();
  neck.sad();
  myEyes.startSad();
}

void Action::happy() {
  myEyes.cancelExpression();  // Cancel any ongoing expression
  myEyes.centerEyes();        // Immediately open eyes to default state
  currentAction = '3';
  hands.happy();
  neck.happy();
  myEyes.startExcited();
}

void Action::mad() {
  myEyes.cancelExpression();  // Cancel any ongoing expression
  myEyes.centerEyes();        // Immediately open eyes to default state
  currentAction = '4';
  hands.mad();
  myEyes.startMad();
}

void Action::dance() {
  myEyes.cancelExpression();  // Cancel any ongoing expression
  myEyes.centerEyes();        // Immediately open eyes to default state
  currentAction = '6';
  hands.dance();
  myEyes.startExcited();
}

void Action::standBy() {
  myEyes.cancelExpression();  // Cancel any ongoing expression
  currentAction = 's';
  hands.standBy();
  neck.standBy();
  myEyes.sleep();
}

void Action::detectWakeword() {
  myEyes.cancelExpression();  // Cancel any ongoing expression
  myEyes.centerEyes();        // Immediately open eyes to default state
  currentAction = '7';
  hands.standBy();            // Keep hands in standby position
  neck.detectWakeword();      // Call neck's detectWakeword function
  myEyes.startQuestion();
}

void Action::hearingQuestion() {
  myEyes.cancelExpression();
  myEyes.centerEyes();
  currentAction = '8';
  hands.hearingQuestion();
  neck.standBy();
  myEyes.startHappy();
}

void Action::thinking() {
  myEyes.cancelExpression();  // Cancel any ongoing expression
  myEyes.centerEyes();        // Immediately open eyes to default state
  currentAction = '9';
  hands.standBy();
  neck.thinking();
  myEyes.startQuestion();
}

void Action::update() {
  switch (currentAction) {
    case '1':
      hands.idle();
      neck.idle();
      myEyes.updateIdle();
      myEyes.updateBlink();  // Handle any ongoing blinks
      break;
    case '2':
      hands.answer();
      neck.answer();
      myEyes.updateExcited();
      myEyes.updateBlink();
      break;
    case '5':
      hands.sad();
      neck.sad();
      myEyes.updateSad();
      myEyes.updateBlink();
      break;
    case '3':
      hands.happy();
      neck.happy();
      myEyes.updateExcited();
      myEyes.updateBlink();
      break;
    case '4':
      hands.mad();
      myEyes.updateMad();
      myEyes.updateBlink();
      break;
    case '6':
      hands.dance();
      myEyes.updateExcited();
      myEyes.updateBlink();
      break;
    case '7':
      hands.standBy();        // Keep hands in standby during wakeword
      neck.detectWakeword();  // Continue neck's wakeword animation
      myEyes.updateQuestion();
      myEyes.updateBlink();
      break;
    case '8':
      hands.hearingQuestion();
      neck.standBy();
      myEyes.updateHappy();
      myEyes.updateBlink();
      break;
    case '9':
      hands.standBy();
      neck.thinking();
      myEyes.updateQuestion();
      myEyes.updateBlink();
      break;
    case 's':
      hands.standBy();
      neck.standBy();
      myEyes.updateBlink();  // Handle any blinks during standby
      break;
  }
}