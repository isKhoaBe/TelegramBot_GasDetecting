#ifndef FSM_BUTTONS_H
#define FSM_BUTTONS_H

enum ScreenState {
  SCREEN_MONITOR,      
  SCREEN_SET_WARN,     
  SCREEN_SET_CRITICAL,
  SCREEN_CALIBRATE
};

extern ScreenState currentScreen;

void handleButtons();

#endif // FSM_BUTTONS_H