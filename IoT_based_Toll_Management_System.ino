#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN D4
#define RST_PIN D3
#include <Wire.h>
#include <Servo.h>

Servo myservo;



#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#define FIREBASE_HOST "iot-toll-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "KafDBJizK2sXPnXtGslMEkfXog7z2r4c072hLhga"
#define WIFI_SSID "IOT_TOLL"
#define WIFI_PASSWORD "1234567890"
FirebaseData firebaseData;
FirebaseJson json;

float _balance[3] = {10000, 12000,15000}, tollprice = 100;
int _count[3] = {0, 0},senLim=430;
String _name[3] = {"Moksedul", "Mithila","Kayesur"};


MFRC522 mfrc522(SS_PIN, RST_PIN);

void writeDB(int person) {
  Firebase.setString(firebaseData, "/" + _name[person] + "/Balance", String(_balance[person]) );
  Firebase.setString(firebaseData, "/" + _name[person] + "/Count", String(_count[person]) );
}




void initSystem() {
  myservo.attach(D0);
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  dis("Connecting Wifi!");
  while (WiFi.status() != WL_CONNECTED)
  {

    //    digitalWrite(D4, 0);
    delay(150);
    //    Serial.print(".");
    //    digitalWrite(D4, 1);
    //    delay(100);
  }
  dis("Wifi connected!");
  delay(500);
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}







boolean credit(int person) {
  if (_balance[person] < tollprice) {
    dis("Insufficient balance");
    return false;
  } else {
    dis("Credited " + String(tollprice) + "tk from " + _name[person]);
    _balance[person] -= tollprice;
    _count[person]++;
    writeDB(person);
    return true;
  }

}


int getCardID() {
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return -1;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return -1;
  }
  //Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  delay(500);
  content.toUpperCase();

  if (content.substring(1) == "19 72 43 A2")
  {

    return 0;
  }

  else if (content.substring(1) == "89 4D 44 A2") {
    return 1;
  }
  

  else if (content.substring(1) == "7C 00 FE 38") {

    return 2;
  }
  

  return -1;
}



void dis(String text) {
  Serial.println(text);
}


void servo(bool is) {
   is ? myservo.write(150) : myservo.write(0);

  Firebase.setString(firebaseData, "/Bar", is?"1":"0");
}












void setup() {

  initSystem();


  
  pinMode(D1,OUTPUT);//Buzzer
  
}



int ID = -1;
void loop() {
  delay(200);
  dis("Toll Management");


  for (; analogRead(A0)<senLim;) {
    digitalWrite(D1,1);
    dis("Punch your card");
    delay(200);
    digitalWrite(D1,0);
    servo(true);
    dis("Punch your card");
    ID = getCardID();
    if (ID >-1  && ID<3) {
      digitalWrite(D1,1);
      credit(ID);
      dis("Credited");
      servo(false);
      while (analogRead(A0)<senLim) {
        delay(200);
      }
      delay(2000);
      digitalWrite(D1,0);
    }
  


  }

  servo(false);

  delay(300);

}
