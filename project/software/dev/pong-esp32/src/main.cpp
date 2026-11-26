#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "config.h"

Adafruit_ST7789 tft(&SPI, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RESET);

constexpr uint32_t FRAME_TIME_MS = 16;
constexpr uint32_t DEBOUNCE_MS   = 25;
constexpr int      PRESSED_LEVEL = LOW; // Bouton relié à GND avec INPUT_PULLUP

struct Button {
    int pin;
    int lastRead;
    uint32_t lastChangeMs;
    int stableLevel;
};

Button button = { PIN_BUTTON, HIGH, 0, HIGH };
Button joyButton   = { PIN_JOYSTICK_SELECT, HIGH, 0, HIGH };

float dotX = 120, dotY = 160;   // position calculée
int drawnX = 120, drawnY = 160; // position affichée

bool justPressed(Button& b) {
    int level = digitalRead(b.pin);

    if (level != b.lastRead) {
        b.lastRead = level;
        b.lastChangeMs = millis();
    }

    if (millis() - b.lastChangeMs > DEBOUNCE_MS
        && level != b.stableLevel) {

        b.stableLevel = level;
        return level == PRESSED_LEVEL;
    }

    return false;
}

constexpr int JOYSTICK_DEADZONE = 300;
int joystickCenterX = 2048;
int joystickCenterY = 2048;

void calibrateJoystick() {
    long sumX = 0;
    long sumY = 0;

    for (int i = 0; i < 16; i++) {
        sumX += analogRead(PIN_JOYSTICK_X);
        sumY += analogRead(PIN_JOYSTICK_Y);
        delay(2);
    }
    joystickCenterX = sumX / 16;
    joystickCenterY = sumY / 16;
}

// Convertit une lecture brute (0..4095) en valeur entre -1.0 et +1.0
float normalizeJoystick(int raw, int center) {
    int offset = raw - center;
    if (abs(offset) < JOYSTICK_DEADZONE) {
        return 0.0f;
    }
    int range = (offset > 0) ? (4095 - center) : center;
    return constrain((float)offset / range, -1.0f, 1.0f);
}

void setup() {
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_BUZZER, OUTPUT);

    calibrateJoystick();

    SPI.begin(PIN_TFT_SCK, -1, PIN_TFT_MOSI, PIN_TFT_CS); // pas de MISO
    tft.init(240, 320, SPI_MODE3);
    //tft.invertDisplay(false);
    tft.setRotation(2);                                   // portrait
    tft.fillScreen(ST77XX_BLACK);
}

void loop() {
    static uint32_t lastFrameMs = 0;
    uint32_t nowMs = millis();
    if (nowMs - lastFrameMs < FRAME_TIME_MS) {
        return;
    }
    lastFrameMs = nowMs;

    // 1. Lire
    float x = normalizeJoystick(analogRead(PIN_JOYSTICK_X), joystickCenterX);
    bool beep = justPressed(button);

    // 2. Calculer
    dotX = constrain(dotX + x * 4.0f, 0.0f, 235.0f);
    if (beep) {
        tone(PIN_BUZZER, 880, 50);
    }

    // 3. Afficher : effacer l'ancien point, dessiner le nouveau
    if ((int)dotX != drawnX) {
        tft.fillRect(drawnX, drawnY, 5, 5, ST77XX_BLACK);
        drawnX = (int)dotX;
        tft.fillRect(drawnX, drawnY, 5, 5, ST77XX_WHITE);
    }
}