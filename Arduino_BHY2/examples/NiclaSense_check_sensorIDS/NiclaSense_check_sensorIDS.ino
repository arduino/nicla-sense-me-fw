/*
Sketch that checks all the sensors avaialble, requesting 100 samples of each
With Putty, log the Serial output as a .txt
@author: Pablo Marquínez
*/

#include "Arduino_BHY2.h"

/*
Sensor
SensorXYZ 
SensorQuaternion
SensorOrientation
SensorBSEC
SensorActivity
*/
int sensors[] = {112, 113,114, 116, 117, 120, 121, 122, 123, 124};
Sensor* ptrSensor;

int sensorsBSEC[] = {112, 113,114, 116, 117, 120, 121, 122, 123, 124};
SensorBSEC* ptrSensorBSEC;

int sensorsBSEC2[] = {112, 113,114, 116, 117, 120, 121, 122, 123, 124};
SensorBSEC2* ptrSensorBSEC2;

int sensorsBSEC2Collector[] = {112, 113,114, 116, 117, 120, 121, 122, 123, 124};
SensorBSEC2Collector* ptrSensorBSEC2Collector;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial)
    ;

  delay(10000);
  BHY2.begin();

  delay(1000);

  checkSensors();
  checkSensorsBSEC();
  checkSensorsBSEC2();
  checkSensorsBSEC2Collector();

  Serial.println("--------");
  Serial.println("END");
}

void loop() {
  // put your main code here, to run repeatedly:
}

void checkSensors() {
  Serial.println("-------");
  Serial.println("Checking type Sensor");
  int listLength = sizeof(sensors) / sizeof(sensors[0]);
  for (int checkID = 0; checkID < listLength; checkID++) {
    ptrSensor = new Sensor(sensors[checkID]);
    ptrSensor->begin();

    Serial.print("\tChecking ");
    Serial.println(sensors[checkID]);
    for (int i = 0; i < 3; i++) {
      BHY2.update();
      Serial.println(String("\t\tSample n") + String(i) + String(" ") + ptrSensor->toString());
      delay(20);
    }

    ptrSensor->end();
  }
}

void checkSensorsBSEC() {
  Serial.println("-------");
  Serial.println("Checking type SensorBSEC");
  int listLength = sizeof(sensorsBSEC) / sizeof(sensorsBSEC[0]);
  for (int checkID = 0; checkID < listLength; checkID++) {
    ptrSensorBSEC = new SensorBSEC(sensorsBSEC[checkID]);
    ptrSensorBSEC->begin();

    Serial.print("\tChecking ");
    Serial.println(sensorsBSEC[checkID]);
    for (int i = 0; i < 3; i++) {
      BHY2.update();
      Serial.println(String("\t\tSample n") + String(i) + String(" ") + ptrSensorBSEC->toString());
      delay(20);
    }

    ptrSensorBSEC->end();
  }
}

void checkSensorsBSEC2() {
  Serial.println("-------");
  Serial.println("Checking type SensorBSEC2");
  int listLength = sizeof(sensorsBSEC2) / sizeof(sensorsBSEC2[0]);
  for (int checkID = 0; checkID < listLength; checkID++) {
    ptrSensorBSEC2 = new SensorBSEC2(sensorsBSEC2[checkID]);
    ptrSensorBSEC2->begin();

    Serial.print("\tChecking ");
    Serial.println(sensorsBSEC2[checkID]);
    for (int i = 0; i < 3; i++) {
      BHY2.update();
      Serial.println(String("\t\tSample n") + String(i) + String(" ") + ptrSensorBSEC2->toString());
      delay(20);
    }

    ptrSensorBSEC2->end();
  }
}

void checkSensorsBSEC2Collector() {
  Serial.println("-------");
  Serial.println("Checking type SensorBSEC2Collector");
  int listLength = sizeof(sensorsBSEC2Collector) / sizeof(sensorsBSEC2Collector[0]);
  for (int checkID = 0; checkID < listLength; checkID++) {
    ptrSensorBSEC2Collector = new SensorBSEC2Collector(sensorsBSEC2Collector[checkID]);
    ptrSensorBSEC2Collector->begin();

    Serial.print("\tChecking ");
    Serial.println(sensorsBSEC2Collector[checkID]);
    for (int i = 0; i < 3; i++) {
      BHY2.update();
      Serial.println(String("\t\tSample n") + String(i) + String(" ") + ptrSensorBSEC2Collector->toString());
      delay(20);
    }

    ptrSensorBSEC2Collector->end();
  }
}