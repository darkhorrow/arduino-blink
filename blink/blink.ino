//################# Para la ampliación del proyecto ####################
// SCL/SCK = A5
// SDA = A4
// Lectura de pulsadores = A0
// Led Réplica del pin 13 = 2

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include  <LiquidCrystal.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Valores iniciales de X e Y del display OLED
int startX = 1;
float x = startX;
int y, prev_y;

const int LED_BUILTIN_AUX = 2;
const int BUTTONS_PIN = 0;

const float ZOOM_FACTOR = 10.0f;
const int MAX_ZOOM = 9;
const int MIN_ZOOM = 1;

const float MAX_FREQUENCY_LOWER = 1.0f;
const float MAX_FREQUENCY_UPPER = 20.0f;
const float FREQUENCY_FACTOR = 0.5f;

const int LCD_WIDTH = 16;
const int LCD_HEIGHT = 2;

float freqMax = 5.0f;
float freqMin = 1.0f;

float angle;
float frequency = freqMin;
unsigned int periodMillis = (1 / frequency) * 1000 / 2;
unsigned long timer = 0;

bool didToogleBefore = false;

int zoom = 1;

bool isOLedAvailable = false;

enum Buttons {
  SELECT = 750,
  LEFT = 550,
  RIGHT = 50,
  UP = 150,
  DOWN = 350
};

void setup() {
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    isOLedAvailable = true;
    display.clearDisplay();
    axis();
  }

  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  lcdDisplay();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_BUILTIN_AUX, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  onButtonPressed();

  // ############################ Parte de impresion en OLED y pulsadores #####################
  y = map(frequency, freqMin, freqMax, SCREEN_HEIGHT / 2 + 20, SCREEN_HEIGHT / 2 - 20);

  display.drawLine(x, prev_y, x + 0.5, y, WHITE);// El 0.5 hay que retocarlo quizás
  prev_y = y;
  x += zoom / ZOOM_FACTOR; // Aumentando el valor hace zoom (Como el factor de ampliación visual de la onda)
  if (x >= (SCREEN_WIDTH - startX)) {
    x = startX;// Esta linea
    axis();
    //    display.startscrollleft(0x01, 0x07);// Hace scroll horizontal de las filas desde la primera dirección hasta la segunda(El display tiene 8 filas)
    //    delay(9000);// Este tiempo es el que está haciendo el scroll
    //    display.stopscroll();
  }
  display.display();

  // --------------------------------------------------
  // #############################################################################################

  if (currentMillis - timer >= periodMillis) {
    toogleLedBuiltin();
    if (didToogleBefore) {
      angle = currentMillis * 2.f * PI / 12000.f;
      frequency = fmap(sin(angle), -1.0f, 1.0f, freqMin, freqMax); // Frequency in Hz
      periodMillis = (1 / frequency) * 1000 / 2; // Period in seconds and converted to ms
    }
    timer = currentMillis;
    didToogleBefore = !didToogleBefore;
  }
}

/*
    @description: Maps the given from a range [a1, a2] to a range [b1, b2].
    @param s: The value to map.
    @param a1: Lower bound of the current range of 's'.
    @param a2: Upper bound of the current range of 's'.
    @param b1: Lower bound of the new range of 's'.
    @param b2: Upper bound of the new range of 's'.
    @return: A float value in the range [b1, b2] from the given value s and its range [a1, a2].
*/
float fmap(float s, float a1, float a2, float b1, float b2) {
  return b1 + (((s - a1) * (b2 - b1)) / (a2 - a1));
}

/*
   @description: Changes the builtin led state
*/
void toogleLedBuiltin() {
  switch (digitalRead(LED_BUILTIN)) {
    case LOW:
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(LED_BUILTIN_AUX, HIGH);
      break;
    case HIGH:
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(LED_BUILTIN_AUX, LOW);
      break;
  }
}

void axis() {
  display.clearDisplay();
  display.drawLine(startX, SCREEN_HEIGHT, startX, 10, WHITE); // Dibuja el eje Y
  display.drawLine(startX, SCREEN_HEIGHT - startX, SCREEN_WIDTH - startX, SCREEN_HEIGHT - startX, WHITE);// Dibuja el eje X
  display.display();
}

void lcdDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("F.Max F.Min Zoom");
  lcd.setCursor(0, 1);
  lcd.print(freqMax);
  lcd.print("  ");
  lcd.print(freqMin);
  lcd.print("  X");
  lcd.print(zoom);
}

void onButtonPressed() {
  pinMode(BUTTONS_PIN, OUTPUT);
  int pressedButton = analogRead(BUTTONS_PIN);

  if (pressedButton <= SELECT && pressedButton > LEFT) {

  }
  if (pressedButton <= LEFT && pressedButton > DOWN) {
    pinMode(BUTTONS_PIN, INPUT_PULLUP);
    analogWrite(BUTTONS_PIN, 1024);
    zoom = min(zoom + 1, MAX_ZOOM);
    lcdDisplay();
    return;
  }
  if (pressedButton <= DOWN && pressedButton > UP) {
    pinMode(BUTTONS_PIN, INPUT_PULLUP);
    analogWrite(BUTTONS_PIN, 1024);
    freqMax = max(freqMax - FREQUENCY_FACTOR, MAX_FREQUENCY_LOWER);
    lcdDisplay();
    return;
  }
  if (pressedButton <= UP && pressedButton > RIGHT) {
    pinMode(BUTTONS_PIN, INPUT_PULLUP);
    analogWrite(BUTTONS_PIN, 1024);
    freqMax = min(freqMax + FREQUENCY_FACTOR, MAX_FREQUENCY_UPPER);
    lcdDisplay();
    return;
  }
  if (pressedButton <= RIGHT) {
    pinMode(BUTTONS_PIN, INPUT_PULLUP);
    analogWrite(BUTTONS_PIN, 1024);
    zoom = max(zoom - 1, MIN_ZOOM);
    lcdDisplay();
  }
}
