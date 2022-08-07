
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define Relay5            D3
#define Relay2            D2
      

#define button            D7
#define led1              D0
int preStateButton=LOW;       //lưu trạng thái trước của button
int count = 0;               //biến lưu số lần nhấn button
bool gas;

//int curStateButton = digitalRead(button);//lấy trạng thái hiện tại của button

#define WLAN_SSID       "26 Giai Phong"             // SSID
#define WLAN_PASS       "GPMN1975"        //  password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com" //Adafruit Server
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "giacatliem"            //username
#define AIO_KEY         "aio_PCuJ58oOvpgEdijm0dtIjUNEjoKP"    // key

//WIFI CLIENT
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/Relay5"); // Feeds name should be same everywhere
Adafruit_MQTT_Subscribe Light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Relay2");
Adafruit_MQTT_Subscribe Gas = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Gas");

void MQTT_connect();







void setup() {
  
  Serial.begin(115200);
  
   pinMode(button, INPUT);
  pinMode(led1, OUTPUT);
  

  pinMode(Relay5, OUTPUT);
  pinMode(Relay2, OUTPUT);
  
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
 
  mqtt.subscribe(&Light1);
  mqtt.subscribe(&Light2);

}

void loop() {

int curStateButton = digitalRead(button);//lấy trạng thái hiện tại của button
      if (curStateButton==HIGH && preStateButton==LOW){
    count++;
  }
      preStateButton=curStateButton;
  if(count%2==1){
    digitalWrite(Relay5,LOW);
  }
  if(count%2==0){
    digitalWrite(Relay5,HIGH);

  }


  MQTT_connect();

  
  Adafruit_MQTT_Subscribe *subscription;
  Adafruit_MQTT_Publish *publision;
  while ((subscription = mqtt.readSubscription(10))) {
     
    if (subscription == &Light1) {
      
      Serial.print(F("Light 1 got: "));
      Serial.println((char *)Light1.lastread);
      int Light1_State = atoi((char *)Light1.lastread);
      count++;
      digitalWrite(Relay5, Light1_State);}
/*  
    if (subscription == &Light2) {
      Serial.print(F("Light 2 got: "));
      Serial.println((char *)Light2.lastread);
      int Light2_State = atoi((char *)Light2.lastread);
      digitalWrite(Relay2, Light2_State);}*/
    }

}


void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); 
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
