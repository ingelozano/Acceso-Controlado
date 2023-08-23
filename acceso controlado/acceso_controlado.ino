#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Cambia la dirección 0x27 según tu módulo

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int relayPin = 10;
const int greenLedPin = 11;
const int redLedPin = 12;
const int obstacleSensorPin = A0;

const String correctPIN = "1234";
String enteredPIN = "";

boolean scrollingWelcome = true;
boolean accessGranted = false;
boolean obstacleDetected = false;
boolean relayActivated = false;
unsigned long relayActivationTime = 0;
int scrollPositionWelcome = 16;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(relayPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(obstacleSensorPin, INPUT);
  digitalWrite(relayPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
}

void loop() {
  if (scrollingWelcome) {
    lcd.clear();
    lcd.setCursor(scrollPositionWelcome, 0);
    lcd.print("Bienvenido!");

    scrollPositionWelcome -= 2;
    if (scrollPositionWelcome < -15) {
      scrollingWelcome = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ingrese el PIN:");
      lcd.setCursor(0, 1);
      lcd.print("PIN: ");
    }
    delay(150);
  }

  char key = keypad.getKey();

  if (!scrollingWelcome && key) {
    if (key == 'A') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ingrese el PIN:");
      lcd.setCursor(0, 1);
      lcd.print("PIN: ");
      enteredPIN = "";
    }

    if (key == 'B' && accessGranted) {
      scrollingWelcome = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bienvenido!");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      accessGranted = false;
    }

    if (key == '#') {
      if (enteredPIN == correctPIN) {
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("ABRIENDO");
        lcd.setCursor(5, 1);
        lcd.print("PUERTA");
        digitalWrite(relayPin, HIGH);
        digitalWrite(greenLedPin, HIGH);
        relayActivated = true;
        relayActivationTime = millis();
        accessGranted = true;
      } else {
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("INTENTE");
        lcd.setCursor(3, 1);
        lcd.print("NUEVAMENTE");
        digitalWrite(redLedPin, HIGH);
        delay(2000);
        digitalWrite(redLedPin, LOW);
      }
      lcd.clear();
      scrollingWelcome = true;
      scrollPositionWelcome = 16;
      enteredPIN = "";
    } else if (key != NO_KEY && enteredPIN.length() < 4) {
      enteredPIN += key;
      lcd.setCursor(8, 1);
      lcd.print(getHiddenCode());
    }
  }

  if (relayActivated) {
    if (!digitalRead(obstacleSensorPin)) {
      relayActivated = false;
      digitalWrite(relayPin, LOW);
      digitalWrite(greenLedPin, LOW);
    } else {
      if (millis() - relayActivationTime >= 10000) {
        relayActivated = false;
        digitalWrite(relayPin, LOW);
        digitalWrite(greenLedPin, LOW);
      }
    }
  }
}

String getHiddenCode() {
  String hiddenCode = "";
  for (int i = 0; i < enteredPIN.length(); i++) {
    hiddenCode += "*";
  }
  return hiddenCode;
}
