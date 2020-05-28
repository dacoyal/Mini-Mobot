#include <SparkFun_TB6612.h>
#include <HCSR04.h>

#define AIN1  2
#define BIN1  7
#define AIN2  4
#define BIN2  8
#define PWMA  5
#define PWMB  6
#define STBY  9
#define lightSamplingTimes 10
#define distanceSamplingTimes 5

const int right_Sensor = A1;
const int left_Sensor = A0;
const int middle_Sensor = A2;
const int trigPin = 11;
const int echoPin = 10;

long long int iterations = 0;
double duration;
double distance = 16;
int left_speed = 0;
int right_speed = 0;
int left_Val = 0;
int right_Val = 0;
int middle_Val = 0;
bool done = false;


Motor motor1 = Motor(AIN1, AIN2, PWMA, 1, STBY); //fill in inputs
Motor motor2 = Motor(BIN1, BIN2, PWMB, 1, STBY); //fill in inputs

void setup() {
  pinMode(right_Sensor, INPUT);
  pinMode(left_Sensor, INPUT);
  pinMode(middle_Sensor, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  iterations ++;
  // Light Sensors
  left_Val = 0;
  right_Val = 0;
  middle_Val = 0;
  for (int i = 0; i < lightSamplingTimes; i++) {
    left_Val += analogRead(left_Sensor);
    right_Val += analogRead(right_Sensor);
    middle_Val += analogRead(middle_Sensor);
  }
  // Distance Sensor
  if (iterations % 50 == 0 or distance < 100) {
    distance = 0;
    for (int i = 0; i < distanceSamplingTimes; i++) {
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW); 
      duration = pulseIn(echoPin, HIGH);
      distance += duration * 0.034 / 2;
      delayMicroseconds(625); // 1000 -> 500 -> 750 -> 625 & then we stopped bc diminishing returns
    }
    distance /= distanceSamplingTimes;
  }

  Serial.print(left_Val);
  Serial.print(" ");
  Serial.print(middle_Val);
  Serial.print(" ");
  Serial.print(right_Val);
  Serial.print("  ");
  Serial.print(done);
  Serial.print("  ");
  Serial.println(distance);

  if (left_Val > max(right_Val, middle_Val)) {
    right_speed = 100;
    left_speed = 0;
  }
  if (right_Val > max(left_Val, middle_Val)) {
    right_speed = 0;
    left_speed = 100;
  }
  if (middle_Val > max(right_Val, left_Val)) {
    right_speed = 100;
    left_speed = 100;
  }
  if (distance <= 15) {
    done = true;
  }
  if (done) {
    right_speed = 0;
    left_speed = 0;
  }
  motor1.drive(right_speed);
  motor2.drive(left_speed);
}
