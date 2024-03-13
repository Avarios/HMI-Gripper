#include <SoftwareSerial.h>
#include "ESP8266_ISR_Servo.h"
#include <EEPROM.h>

#define SERVO_PIN D7 // Direction
#define PIN_POTENTIOMETER A0
#define BUTTON_PIN D1
#define MIN_MICROS 544 // 800
#define MAX_MICROS 2450
#define NUM_SERVOS 1
#define EEPROM_SIZE 8
#define TIMER_INTERRUPT_DEBUG       0
#define ISR_SERVO_DEBUG             0

int pressCounter = 0;
int angleLow;
int angleHigh;
bool buttonPressed = false;
bool tooglePos = false;

typedef struct
{
  int servoIndex;
  uint8_t servoPin;
} ISR_servo_t;

ISR_servo_t ISR_servo[NUM_SERVOS] =
    {
        {-1, D7},
};

IRAM_ATTR void button_press()
{
  buttonPressed = true;
  int analogValue = analogRead(PIN_POTENTIOMETER);
  Serial.print("Button Press Counter:");
  Serial.print(pressCounter);
  Serial.println("");
  // scales it to use it with the servo (value between 0 and 180)
  int angle = map(analogValue, 7, 1024, 0, 90);

  switch (pressCounter)
  {
  case 0:
    pressCounter = 1;
    break;
  case 1:
    Serial.print("Saveing Wide Angle Position:");
    Serial.print(angle);
    Serial.println("");
    EEPROM.write(0, angle);
    angleHigh = angle;
    pressCounter = 2;
    break;
  case 2:
    Serial.print("Saveing Low Angle Position:");
    Serial.print(angle);
    Serial.println("");
    EEPROM.write(1, angle);
    angleLow = angle;
    buttonPressed = false;
    pressCounter = 0;
    EEPROM.commit();
    break;
  }
}

void setup()
{
  EEPROM.begin(EEPROM_SIZE);
  angleLow = EEPROM.read(0);
  angleHigh = EEPROM.read(1);
  Serial.begin(9600);
  Serial.println("\nStarting");
  pinMode(BUTTON_PIN, INPUT);

  for (int index = 0; index < 1; index++)
  {
    ISR_servo[index].servoIndex = ISR_Servo.setupServo(ISR_servo[index].servoPin, MIN_MICROS, MAX_MICROS);

    if (ISR_servo[index].servoIndex != -1)
      Serial.println("Setup OK Servo index = " + String(ISR_servo[index].servoIndex));
    else
      Serial.println("Setup Failed Servo index = " + String(ISR_servo[index].servoIndex));
  }
  Serial.print(" Angle low: ");
  Serial.print(angleLow);
  Serial.print(" Angle high: ");
  Serial.print(angleHigh);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_press, FALLING);
}

void loop()
{
  if (buttonPressed)
  {
    int angle = map(analogRead(PIN_POTENTIOMETER), 7, 1024, 0, 90);
    ISR_Servo.setPosition(ISR_servo[0].servoIndex, angle);
  }
  if (!buttonPressed)
  {
    int pos = ISR_Servo.getPosition(ISR_servo[0].servoIndex);
    Serial.print(" Angle low: ");
    Serial.print(angleLow);
    Serial.print(" Angle high: ");
    Serial.print(angleHigh);
    Serial.print(" Position: ");
    Serial.print(pos);
    Serial.println("");
    if (tooglePos)
    {
      ISR_Servo.setPosition(ISR_servo[0].servoIndex, angleHigh);
    } else {
      ISR_Servo.setPosition(ISR_servo[0].servoIndex, angleLow);
    }
    tooglePos = !tooglePos;
    delay(3000);
  }

  delay(100);
}