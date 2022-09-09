#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN D4
#define RST_PIN D2
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>

Servo myservo;

LiquidCrystal_I2C lcd(0x27, 16, 2);


#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#define FIREBASE_HOST "toll-management-system-b2d9b-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "gVtMq22XDKrk4Yl9qqrfBXDYL9k0iImXycuf8773"
#define WIFI_SSID "IOT_TOLL"
#define WIFI_PASSWORD "1234567890"
FirebaseData firebaseData;
FirebaseJson json;

float _balance[2] = {10000, 12000}, tollprice = 100;
int _count[2] = {0, 0};
String _name[2] = {"Nitu", "Toriqul"};


MFRC522 mfrc522(SS_PIN, RST_PIN);

void writeDB(int person) {
  Firebase.setString(firebaseData, "/" + _name[person] + "/Balance", String(_balance[person]) );
  Firebase.setString(firebaseData, "/" + _name[person] + "/Count", String(_count[person]) );
  //  Serial.println("Attendence with " + field);
}




void initSystem() {
  lcd.init();                 //Init the LCD
  lcd.clear();
  lcd.backlight();            //Activate backlight
 // lcd.home();
  myservo.attach(D8);
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

  if (content.substring(1) == "06 2D 2F F8")
  {
    beep();
    return 0;
  }

  else if (content.substring(1) == "EB 4B 6D 26") {
    beep();
    return 1;
  }

  return -1;
}



void dis(String text) {
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print(text);
  Serial.println(text);
}

void beep() {
  digitalWrite(A0, 1);
  delay(100);
  digitalWrite(A0, 0);
  delay(50);
  digitalWrite(A0, 1);
  delay(100);
  digitalWrite(A0, 0);
}


void servo(bool is) {
  is ? myservo.write(50) : myservo.write(120);

  Firebase.setString(firebaseData, "/Bar", is?"1":"0");
}











int ir = D0;


void setup() {

  initSystem();


  pinMode(A0, OUTPUT);//Buzzer
  pinMode(ir, INPUT);
  
}



int ID = -1;
void loop() {
  delay(200);
  dis("Toll Management");


  for (; !digitalRead(ir);) {

    dis("Punch your card");
    delay(200);
    servo(true);
    dis("Punch your card");
    ID = getCardID();
    if (ID == 0) {
      credit(ID);
      dis("Credited");
      servo(false);
      while (!digitalRead(ir)) {
        delay(200);
      }
    } else if (ID == 1) {
      credit(ID);
    }



  }

  servo(false);

  delay(300);

}
