#include "Menu.h"
#include "Config.h"
#include "DisplayUI.h"
#include "Stats.h"
#include "Games.h"
#include "Sound.h"
#include "LED.h"
#include <Arduino.h>

// Access latch variables from Input.cpp
extern unsigned long lastCenter;

// extern stats
extern int energy;
extern int happiness;

void handleMainMenuAction(int index) {

    if (index == 0) {
        // CLEAR CENTER LATCH BEFORE ENTERING GAMES MENU
        lastCenter = 0;

        // Enter Games Menu
        appState = STATE_GAMES_MENU;
        menuIndex = 0;
        drawScreen();
    }

    else if (index == 1) {
        // Replenish happiness only
        happiness = clamp(happiness + 10);

        ledSetMood(happiness);
        soundMenuSelect();

        // Update only happiness bar + face
        updateBarFill(58, 10, 65, happiness, YELLOW);
        drawFace();
    }

    else if (index == 2) {
        // CLEAR CENTER LATCH BEFORE ENTERING SETTINGS
        lastCenter = 0;

        // Settings Menu
        appState = STATE_SETTINGS_MENU;
        menuIndex = 0;
        drawScreen();
    }
}
