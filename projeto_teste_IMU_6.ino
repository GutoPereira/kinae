#include<Wire.h>
#include <MPU6050_tockn.h>
#include <SoftwareSerial.h>
#include "SensorFusion.h"

SoftwareSerial mySerial(11,10);// RX, TX

// Endereco I2C do MPU6050
const int MPU = 0x68;

MPU6050 mpu6050(Wire);

SF fusion;

float pitch, roll, yaw;
float deltat;

void setup()
{
  delay(5000);
//  Serial.begin(9600);
  mySerial.begin(9600);
  Wire.begin();

  mpu6050.begin();
  //mpu6050.setGyroOffsets(26, -64, -28);
  mpu6050.calcGyroOffsets(true);
  delay(3000);
//  Serial.println();
//  Serial.println("GO!");
}

void loop()
{
  mpu6050.update();

  deltat = fusion.deltatUpdate(); //this have to be done before calling the fusion update

  // NOTE: the gyroscope data have to be in radians
  // if you have them in degree convert them with: DEG_TO_RAD example: gx * DEG_TO_RAD
  fusion.MahonyUpdate(mpu6050.getGyroX()* DEG_TO_RAD, mpu6050.getGyroY()* DEG_TO_RAD, mpu6050.getGyroZ()* DEG_TO_RAD / 2, mpu6050.getAccX(), mpu6050.getAccY(), mpu6050.getAccZ(), deltat);  //mahony is suggested if there isn't the mag and the mcu is slow

  if (mySerial.available()) {
    String in = mySerial.readStringUntil('\n');
    in.trim();
//    Serial.println("received " + in);
    if (in.charAt(0) == 'r') {
      pitch = fusion.getPitch();
      roll = fusion.getRoll();    //you could also use getRollRadians() ecc
      yaw = -fusion.getYaw();

      String out = "";

      out += "{\"x\":";
      out += roll;
      out += ",\"y\":";
      out += yaw;
      out += ",\"z\":";
      out += pitch;
      out += "}";

      mySerial.println(out);
//      Serial.println(out);
    }

    if (in.charAt(0) == 'z') {
      mpu6050.calcGyroOffsets(true);
      delay(3000);
//      Serial.println();
//      Serial.println("GO!");
    }
  }
}
