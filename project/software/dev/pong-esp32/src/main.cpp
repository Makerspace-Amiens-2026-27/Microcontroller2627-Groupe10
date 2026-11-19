#include <Arduino.h>

// Définition des broches du joystick
#define PIN_JOYSTICK_X      1
#define PIN_JOYSTICK_Y      3
#define PIN_JOYSTICK_SELECT 5

void setup() {
    // Initialisation du moniteur série
    Serial.begin(115200);

    // Configuration des broches
    pinMode(PIN_JOYSTICK_X, INPUT);
    pinMode(PIN_JOYSTICK_Y, INPUT);
    pinMode(PIN_JOYSTICK_SELECT, INPUT_PULLDOWN);

    // Configuration de la résolution analogique
    analogReadResolution(12); // Valeurs entre 0 et 4095
}

void loop() {
    // Lecture des axes analogiques
    int x = analogRead(PIN_JOYSTICK_X);
    int y = analogRead(PIN_JOYSTICK_Y);


    // Affichage des valeurs
    Serial.print(">X:"); 
    Serial.println(x);
    
    Serial.print(">Y:");
    Serial.println(y);

    delay(100);
}