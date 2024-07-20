#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3d0SvzOoE"
#define BLYNK_TEMPLATE_NAME "Petfeeder"
#define BLYNK_AUTH_TOKEN "sPJIlxg1MrbtD9BCTNRoBmpp4_fcwtxu"



#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

Servo servo1; // Standard servo
Servo servo2; // MG995 servo

int lastPosition1 = 0;
int lastPosition2 = 0;
int minPos1 = 0;   // Minimum position for servo1 (0 degrees)
int maxPos1 = 180; // Maximum position for servo1 (180 degrees)
int minPos2 = 0;   // Minimum position for servo2 (0 degrees)
int maxPos2 = 180; // Maximum position for servo2 (180 degrees)

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "POCOM3";
char pass[] = "wifihotspot";

#define TRIGGER_PIN1 D1 // Digital pin connected to ultrasonic sensor 1 trigger
#define ECHO_PIN1 D2    // Digital pin connected to ultrasonic sensor 1 echo

#define TRIGGER_PIN2 D3 // Digital pin connected to ultrasonic sensor 2 trigger
#define ECHO_PIN2 D4    // Digital pin connected to ultrasonic sensor 2 echo

const float tankHeight1 = 16; // Height of the first tank in centimeters
const float tankCapacity1 = 1; // Capacity of the first tank in liters
const float maxSensorRange1 = 16; // Maximum range of the ultrasonic sensor 1 in centimeters

const float tankHeight2 = 30; // Height of the second tank in centimeters
const float tankCapacity2 = .8; // Capacity of the second tank in liters
const float maxSensorRange2 = 30; // Maximum range of the ultrasonic sensor 2 in centimeters

float previousWaterLevel1 = 0; // Variable to store previous water level for tank 1
float previousWaterLevel2 = 0; // Variable to store previous water level for tank 2

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  servo1.attach(D5); // Attach standard servo to pin D5
  servo2.attach(D6); // Attach MG995 servo to pin D6
  pinMode(TRIGGER_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIGGER_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
}

void loop() {
  Blynk.run();

  // Read water level for tank 1
  long duration1, distance1;
  digitalWrite(TRIGGER_PIN1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN1, LOW);
  duration1 = pulseIn(ECHO_PIN1, HIGH);
  distance1 = duration1 * 0.034 / 2; // Convert echo time to distance in centimeters

  float waterLevel1 = tankHeight1 - distance1;
  float waterPercentage1 = (waterLevel1 / tankHeight1) * 100;

  // Read water level for tank 2
  long duration2, distance2;
  digitalWrite(TRIGGER_PIN2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN2, LOW);
  duration2 = pulseIn(ECHO_PIN2, HIGH);
  distance2 = duration2 * 0.034 / 2; // Convert echo time to distance in centimeters

  float waterLevel2 = tankHeight2 - distance2;
  float waterPercentage2 = (waterLevel2 / tankHeight2) * 100;

  Serial.print("Tank 1 - Water level: ");
  Serial.print(waterLevel1);
  Serial.print(" cm, Percentage: ");
  Serial.print(waterPercentage1);
  Serial.println("%");

  Serial.print("Tank 2 - Water level: ");
  Serial.print(waterLevel2);
  Serial.print(" cm, Percentage: ");
  Serial.print(waterPercentage2);
  Serial.println("%");

  // Send updates to Blynk app if water level changes significantly
  if (abs(waterLevel1 - previousWaterLevel1) > 2) {
    Blynk.virtualWrite(V0, waterPercentage1); // Send water percentage for tank 1 to Blynk app
    previousWaterLevel1 = waterLevel1;
  }

  if (abs(waterLevel2 - previousWaterLevel2) > 2) {
    Blynk.virtualWrite(V1, waterPercentage2); // Send water percentage for tank 2 to Blynk app
    previousWaterLevel2 = waterLevel2;
  }
}

BLYNK_WRITE(V2) { // Control standard servo (0 to 180 degrees)
  int value = param.asInt();
  Serial.println(value);

  if (value == 1 && lastPosition1 != maxPos1) {
    servo1.write(maxPos1);
    lastPosition1 = maxPos1;
  } else if (value == 0 && lastPosition1 != minPos1) {
    servo1.write(minPos1);
    lastPosition1 = minPos1;
  }
}

BLYNK_WRITE(V3) { // Control MG995 servo (0 to 180 degrees)
  int value = param.asInt();
  Serial.println(value);

  if (value == 1 && lastPosition2 != maxPos2) {
    servo2.write(maxPos2);
    lastPosition2 = maxPos2;
  } else if (value == 0 && lastPosition2 != minPos2) {
    servo2.write(minPos2);
    lastPosition2 = minPos2;
  }
}
