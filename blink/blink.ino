#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LiquidCrystal.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define START_X 0
#define X_INC 0.1f
#define COLOR WHITE
#define PADDING 1

#define LED_BUILTIN_AUX 3
#define BUTTONS_PIN 0
#define INTERRUPTION_PIN 2

#define INTENSITY 1

#define ZOOM_FACTOR 10.0f
#define MAX_ZOOM 9
#define MIN_ZOOM 1

#define MAX_FREQUENCY_LOWER 1.0f
#define MAX_FREQUENCY_UPPER 20.0f
#define MIN_FREQUENCY_LOWER 0.5f
#define FREQUENCY_FACTOR 0.5f

#define LCD_WIDTH 16
#define LCD_HEIGHT 2

float freqMax = 5.0f;
float freqMin = 1.0f;

float angle;
float frequency = freqMin;
unsigned int periodMillis = (1 / frequency) * 1000 / 2;
unsigned long timer = 0;

bool didToogleBefore = false;

int zoom = 1;

bool isOLedAvailable = false;
float x = START_X;
float previousY = SCREEN_HEIGHT;

enum Buttons {
  SELECT = 750,
  LEFT = 550,
  RIGHT = 50,
  UP = 150,
  DOWN = 350
};

void setup() {
  if (oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    isOLedAvailable = true;
    oled.clearDisplay();
  }

  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  lcdDisplay();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_BUILTIN_AUX, OUTPUT);

  pinMode(INTERRUPTION_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPTION_PIN), onButtonPressed, CHANGE);
}

void loop() {
  unsigned long currentMillis = millis();

  oLedDisplay();

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
      analogWrite(LED_BUILTIN_AUX, INTENSITY);
      break;
    case HIGH:
      digitalWrite(LED_BUILTIN, LOW);
      analogWrite(LED_BUILTIN_AUX, 0);
      break;
  }
}

void oLedDisplay() {
  int y = map(frequency, MIN_FREQUENCY_LOWER, MAX_FREQUENCY_UPPER, SCREEN_HEIGHT - PADDING, PADDING);

  oled.drawLine(x, previousY, x + X_INC, y, COLOR);
  
  previousY = y;
  x += zoom / ZOOM_FACTOR;
  
  if (x >= SCREEN_WIDTH) {
    x = START_X;
    oled.clearDisplay();
  }
  
  oled.display();
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
    freqMax = max(max(freqMax - FREQUENCY_FACTOR, MAX_FREQUENCY_LOWER), freqMin + FREQUENCY_FACTOR);
    lcdDisplay();
    return;
  }
  if (pressedButton <= UP && pressedButton > RIGHT) {
    pinMode(BUTTONS_PIN, INPUT_PULLUP);
    analogWrite(BUTTONS_PIN, 1024);
    freqMax = max(min(freqMax + FREQUENCY_FACTOR, MAX_FREQUENCY_UPPER), freqMin + FREQUENCY_FACTOR);
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
