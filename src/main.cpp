#include <Arduino.h>

//CONSTANTS
#define DELAY_TIME 50
#define LED 2
#define POTENCIOMETER 32

//LIMITS TO MAINTAIN THE AEROPENDULUM
#define UPPER_LIMIT -25
#define LOWER_LIMIT 13

//FUNCTION DECLARATIONS
float convertToDegrees(int analog_result);
int checkLimit(float degree_to_check);

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  pinMode(POTENCIOMETER, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(LED, HIGH);

  int potenciometer_value = analogRead(POTENCIOMETER);
  if(potenciometer_value < 0 || potenciometer_value > 4095) {
    Serial.println("Invalid analog value!");
    return;
  }

  float degree = convertToDegrees(potenciometer_value);
  if(degree < -90 || degree > 90) {
    Serial.println("Invalid degree value!");
    return;
  }

  Serial.println(degree);

  int checker = checkLimit(degree);
  if(checker == 1) Serial.println("Upper limit reached!");
  if(checker == -1) Serial.println("Lower limit reached!");

  delay(DELAY_TIME);

  digitalWrite(LED, LOW);
  delay(DELAY_TIME);
}

//FUNCTION LOGICS
float convertToDegrees(int analog_result){
  float degrees = map(analog_result, 0, 4095, -90, 90);  // -90° to +90°

  return degrees;
}

int checkLimit(float degree_to_check){
  if(degree_to_check <= UPPER_LIMIT) return 1;
  if(degree_to_check >= LOWER_LIMIT) return -1;

  return 0;
}