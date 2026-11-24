
#include <Arduino.h>

constexpr int PIN_BUTTON = D5;
constexpr int PIN_BUZZER = D3;
constexpr int PIN_JOYSTICK_X      = D0;
constexpr int PIN_JOYSTICK_Y      = D1;
constexpr int PIN_JOYSTICK_SELECT = D2;

// Configuration bouton
constexpr int PRESSED_LEVEL = LOW;
constexpr unsigned long DEBOUNCE_MS = 30;

// Configuration buzzer
constexpr unsigned long BIP_DURATION = 100;
constexpr int BIP_FREQUENCY = 1000;

unsigned long bipStart = 0;
bool bipActive = false;

// Structure du bouton
struct Button {
    int pin;
    int stableLevel;
    int lastRead;
    unsigned long lastChangeMs;
};

// Initialisation du bouton
Button button = {
    PIN_BUTTON,
    HIGH,
    HIGH,
    0
};

// Détection d'un appui avec anti-rebond
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

// Gestion du buzzer
void bip(bool trigger = false) {

    if (trigger && !bipActive) {
        tone(PIN_BUZZER, BIP_FREQUENCY);
        bipStart = millis();
        bipActive = true;
    }

    if (bipActive && millis() - bipStart >= BIP_DURATION) {
        noTone(PIN_BUZZER);
        bipActive = false;
    }
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
    Serial.begin(115200);

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_BUZZER, OUTPUT);
    
    pinMode(PIN_JOYSTICK_X, INPUT);
    pinMode(PIN_JOYSTICK_Y, INPUT);
    pinMode(PIN_JOYSTICK_SELECT, INPUT);
}

void loop() {

    if (justPressed(button)) {
        Serial.println("Bouton appuyé !");
        bip(true);
    }

    bip();
    
    float x = normalizeJoystick(analogRead(PIN_JOYSTICK_X), joystickCenterX);
    float y = normalizeJoystick(analogRead(PIN_JOYSTICK_Y), joystickCenterY);
    Serial.println(digitalRead(PIN_JOYSTICK_SELECT) == HIGH ? "HIGH" : "LOW");

    Serial.print("> X : ");
    Serial.println(x);

    Serial.print("> Y : ");
    Serial.println(y);
}