#include <Arduino.h>
#include "Config.h"
#include "DisplayUI.h"
#include "Input.h"
#include "Menu.h"
#include "Games.h"
#include "Stats.h"
#include "Settings.h"
#include "Snake.h"
#include "LED.h"
#include "Sound.h"
#include "TimeSystem.h"
#include "RemoteHappiness.h"
#include "MicVoice.h"


// ----- Globals -----
int energy = 70;
int happiness = 0;

int menuIndex = 0;

// Main Menu
const int MENU_ITEMS = 3;
const char* menuOptions[] = {
    "Games",
    "Replenish Stats",
    "Settings"
};

// Settings Menu
const int SETTINGS_ITEMS = 2;
const char* settingsOptions[] = {
    "Enter Low Power",
    "Back"
};

// Games Menu
const int GAMES_ITEMS = 2;
const char* gamesOptions[] = {
    "Snake",
    "Back"
};

// Current app state
AppState appState = STATE_MAIN_MENU;

// Display objects
HardwareSerial DisplaySerial(1);
Goldelox_Serial_4DLib Display(&DisplaySerial);


unsigned long lastTimeUpdate = 0;

void initDisplay() {
    pinMode(LCD_RESET, OUTPUT);
    digitalWrite(LCD_RESET, LOW);
    delay(100);
    digitalWrite(LCD_RESET, HIGH);
    delay(5000);  // give the Goldelox time to boot and show splash

    DisplaySerial.begin(9600, SERIAL_8N1, LCD_RX, LCD_TX);
    Display.TimeLimit4D = 5000;

    // Clear any leftover splash pixels
    Display.gfx_Cls();
}

void setup() {
    initStats();
    initDisplay();
    initLED();
    initSound();
    initRTC();

    pinMode(NAV_UP,     INPUT_PULLUP);
    pinMode(NAV_DOWN,   INPUT_PULLUP);
    pinMode(NAV_LEFT,   INPUT_PULLUP);
    pinMode(NAV_RIGHT,  INPUT_PULLUP);
    pinMode(NAV_CENTER, INPUT_PULLUP);

    // Initial LED mood based on starting happiness
    ledSetMood(happiness);
    initRemoteHappiness();
    initMicVoice(); 

    drawScreen();
}

void loop() {
    bool up, down, left, right, center;
    readNavLatched(up, down, left, right, center);

    // If we are in a game, handle that FIRST and return
    if (appState == STATE_GAME_SNAKE) {
        snakeLoop();
        return;
    }

    // How many items based on current screen
    int maxItems =
        (appState == STATE_MAIN_MENU)     ? MENU_ITEMS :
        (appState == STATE_SETTINGS_MENU) ? SETTINGS_ITEMS :
        (appState == STATE_GAMES_MENU)    ? GAMES_ITEMS :
        0;

    // -------- MENU MOVEMENT --------
    if (up && menuIndex > 0) {
        clearChat();
        menuIndex--;
        soundMenuMove();
        updateMenuCursor();   // was drawScreen();
    }

    if (down && menuIndex < maxItems - 1) {
        clearChat();
        menuIndex++;
        soundMenuMove();
        updateMenuCursor();   // was drawScreen();
    }

    // -------- SELECT --------
    if (center) {
        clearChat();
        soundMenuSelect();

        if (appState == STATE_MAIN_MENU)
            handleMainMenuAction(menuIndex);
        else if (appState == STATE_SETTINGS_MENU)
            handleSettingsAction(menuIndex);
        else if (appState == STATE_GAMES_MENU)
            handleGamesMenuAction(menuIndex);
    }

    // Voice capture (only when not in Snake)
    micVoiceLoop();


// Simple chat scroll controls (example)
if (appState == STATE_MAIN_MENU) {
    if (left)  scrollChatUp();
    if (right) scrollChatDown();
}


    // Update time once per minute, only on BMO UI (no Snake)
    unsigned long now = millis();
    if (now - lastTimeUpdate >= 60000) {   // 60,000 ms = 1 minute
        lastTimeUpdate = now;
        updateTime();
    }

    // Decay stats (also updates LED mood)
    updateStats();
    delay(25);
}
