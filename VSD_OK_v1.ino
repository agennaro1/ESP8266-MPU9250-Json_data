#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <ArduinoJson.h>
#include<math.h>

#ifndef STASSID

//#define STASSID "WIFI_VISITAS"
//#define STAPSK  "invitadocdc"
#define STASSID "GENNARO WIFI"
#define STAPSK  "action01"
#endif

unsigned int localPort = 5006;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet,

WiFiUDP Udp;

#include "MPU9250.h"
MPU9250 IMU(Wire,0x68);
int status;
String MESSAGE = "aca iria el mensaje :-)";  // a string to send back
String XX = "-";
String YY = "-";
String ZZ = "-";
//float roll_rad = 0.0;
//float pitch_rad = 0.0;
//float yaw_rad = 0.0;
String roll= "-";
String pitch = "-";
String yaw = "-";
const char remoteIP[]="10.246.133.123";
int remotePort=5006;



void setup() 
{
    Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(50);
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);
 
  Udp.begin(localPort);
  status = IMU.begin();
  Serial.print("Status: ");
  Serial.println(status);
  
      // setting the accelerometer full scale range to +/-8G 
  IMU.setAccelRange(MPU9250::ACCEL_RANGE_8G);
  // setting the gyroscope full scale range to +/-500 deg/s
  IMU.setGyroRange(MPU9250::GYRO_RANGE_500DPS);
  // setting DLPF bandwidth to 20 Hz
  IMU.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_20HZ);
  // setting SRD to 19 for a 50 Hz update rate
  IMU.setSrd(19);
        delay(20);
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
}


void loop() 
{
  IMU.readSensor();
  
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  
  //toma la data
  XX = String(IMU.getMagX_uT(),2);
  YY = String(IMU.getMagY_uT(),2);
  ZZ = String(IMU.getMagZ_uT(),2);
  roll= String ((IMU.getGyroX_rads() * 180 / PI) , 2);
  pitch= String ((IMU.getGyroY_rads()* 180 /PI) , 2);
  yaw= String ((IMU.getGyroZ_rads()* 180 /PI) , 2);



  // valor original 384
  StaticJsonDocument<500> doc;

  JsonArray CAMARA1_UserData = doc["CAMARA1"].createNestedArray("UserData");
  CAMARA1_UserData.add(XX);
  CAMARA1_UserData.add(YY);
  CAMARA1_UserData.add(ZZ);
  CAMARA1_UserData.add(roll);
  CAMARA1_UserData.add(pitch);
  CAMARA1_UserData.add(yaw);
  CAMARA1_UserData.add(0);
  CAMARA1_UserData.add(0);
  CAMARA1_UserData.add(0);
  CAMARA1_UserData.add(0);

  serializeJson(doc, Serial);
  Serial.println("\t");
  
    // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
    // send data
    Udp.beginPacket(remoteIP, 5006); 
    serializeJson(doc,Udp);
    Udp.endPacket();
  }
