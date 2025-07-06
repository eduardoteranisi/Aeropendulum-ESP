#include <Arduino.h>

//LIMITS TO MAINTAIN THE AEROPENDULUM
#define UPPER_LIMIT 109
#define LOWER_LIMIT 77

//DELAY
#define DELAY_TIME 50

//CONSTANTS
const int motor_ena = 25; // ENA pin PWM velocity control
const int motor_in1 = 26; // IN1 pin
const int motor_in2 = 27; // IN2 
const int led_pin = 2;
const int potentiometer_pin = 32;


//PWM SETTINGS
const int pwm_channel = 0;
const int pwm_frequency = 5000;
const int pwm_resolution = 8; // 8 bits (0-255)

//GLOBAL VARIABLE
int current_pwm_value = 0;
bool motor_running = false;

//FUNCTION DECLARATIONS
float convertToDegrees(int analog_result);
int checkLimit(float degree_to_check);
void setPWM(int pwm_value);
void setMotor(int target_pwm); 

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  ledcSetup(pwm_channel, pwm_frequency, pwm_resolution);

  ledcAttachPin(motor_ena, pwm_channel);

  pinMode(potentiometer_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(motor_in1, OUTPUT);
  pinMode(motor_in2, OUTPUT);

  digitalWrite(motor_in1, HIGH);
  digitalWrite(motor_in2, LOW);
}

void loop() {
  digitalWrite(led_pin, HIGH);

  int potentiometer_value = analogRead(potentiometer_pin);
  if(potentiometer_value < 0 || potentiometer_value > 4095) {
    Serial.println("Invalid analog value!");
    return;
  }

  float degree = convertToDegrees(potentiometer_value);
  if(degree < 0 || degree > 180) {
    Serial.println("Invalid degree value!");
    return;
  }

  int checker = checkLimit(degree);

  int motor_speed = map(degree, 0, 180, 0, 255);

  // VELOCITY CONTROL LOGIC
  if (checker == 1) {
    Serial.println("Upper limit reached! Slowing down...");

    for(int i = motor_speed; i >= 125; i--) {
      setPWM(i);
      delay(10);
    }

    motor_speed = 125;
  } else if (checker == -1) {
    Serial.println("Lower limit reached! Speeding up...");

    for(int i = motor_speed; i <= 125; i++) {
      setPWM(i);
      delay(10);
    }

    motor_speed = 125;
  } else {
    if (motor_speed < 50) {
      setPWM(0);
      motor_running = false;
    } else {
      setMotor(motor_speed);
    }
  }

  Serial.print("Degree: ");
  Serial.print(degree);
  Serial.print("  |  Velocidade do Motor: ");
  Serial.println(motor_speed);

  delay(DELAY_TIME);

  digitalWrite(led_pin, LOW);
  delay(DELAY_TIME);
}

//FUNCTION LOGICS
float convertToDegrees(int analog_result){
  float degrees = map(analog_result, 0, 4095, 0, 180);  // 0° to 180°

  return degrees;
}

int checkLimit(float degree_to_check){
  if(degree_to_check >= UPPER_LIMIT) return 1;
  if(degree_to_check <= LOWER_LIMIT) return -1;

  return 0;
}

void setPWM(int pwm_value) {
  ledcWrite(pwm_channel, pwm_value);
  current_pwm_value = pwm_value;
  
  if (pwm_value > 0) {
    motor_running = true;
  } else {
    motor_running = false;
  }
}

void setMotor(int target_pwm) {
  if (!motor_running && target_pwm > 0) {
    Serial.println("Starting motor with soft start...");
    
    setPWM(190);
    delay(100);
    
    for (int i = 190; i >= target_pwm; i--) {
      setPWM(i);
      delay(10);
    }
    
    Serial.print("Motor started, now running at PWM: ");
    Serial.println(target_pwm);
  } else {
    setPWM(target_pwm);
  }
}