const float FREQ_MAX = 10.0f;
const float FREQ_MIN = 1.0f;
const float ANGLE_INC = 0.5f;

float angle = 0.0f;
float frequency = 0.0f;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  frequency = fmap(sin(angle), -1.0f, 1.0f, FREQ_MIN, FREQ_MAX); // Frequency in Hz
  int periodMillis = (1 / frequency) * 1000; // Period in seconds and converted to ms
  digitalWrite(LED_BUILTIN, HIGH);
  delay(periodMillis);
  digitalWrite(LED_BUILTIN, LOW);
  delay(periodMillis);
  angle += ANGLE_INC;
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
