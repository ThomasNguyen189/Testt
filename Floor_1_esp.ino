
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "AdafruitIO_WiFi.h"

#define Relay1            D3
#define Relay2            D2
#define button            D7
#define button1            D8
#define led1              D0
#define sensor            A0

int preStateButton=LOW;       //lưu trạng thái trước của button
int count = 0;               //biến lưu số lần nhấn button
int preStateButton1=LOW;       //lưu trạng thái trước của button1
int count1 = 0;               //biến lưu số lần nhấn button1
int sensor_trigger =0;
#define WLAN_SSID       "26 Giai Phong"             // SSID
#define WLAN_PASS       "GPMN1975"                  //  password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"                     //Adafruit Server
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "giacatliem"                          //username
#define AIO_KEY         "aio_PCuJ58oOvpgEdijm0dtIjUNEjoKP"    // key
//WIFI CLIENT
WiFiClient client;

// Connect to Wi-Fi and Adafruit IO handle
AdafruitIO_WiFi io(AIO_USERNAME, AIO_KEY, WLAN_SSID, WLAN_PASS);

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/Relay1"); // Feeds name should be same everywhere
Adafruit_MQTT_Subscribe Light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Relay2");
//Adafruit_MQTT_Subscribe Gas = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Gas");

//Adafruit_MQTT_Publish tSensor = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Sensor");
AdafruitIO_Feed *Gas = io.feed("Gas");
void MQTT_connect();

void setup() {
  
  Serial.begin(115200);
  pinMode(button1, INPUT);  
  pinMode(button, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(Relay1, OUTPUT);
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
  io.run();
    int sensor_value = analogRead(A0);
    sensor_trigger=0;
    if(sensor_value>800){ 
      sensor_trigger=sensor_trigger+1;
    }
  if(sensor_trigger==1){
    Gas->save(1);
    delay(5000);
}



    
int curStateButton = digitalRead(button);//lấy trạng thái hiện tại của button
      if (curStateButton==HIGH && preStateButton==LOW){
    count++;

  }
      preStateButton=curStateButton;
  if(count%2==0){
    digitalWrite(Relay1,LOW);
  }
  if(count%2==1){
    digitalWrite(Relay1,HIGH);
  }
  int curStateButton1 = digitalRead(button1);//lấy trạng thái hiện tại của button
      if (curStateButton1==HIGH && preStateButton1==LOW){
    count1++;
  }
      preStateButton1=curStateButton1;
  if(count1 %2==0){
    digitalWrite(Relay2,LOW);
  }
  if(count1 %2==1){
    digitalWrite(Relay2,HIGH);
  }


  MQTT_connect();
/*
  if(buttonsen==HIGH){
Sensor->save(1);
Serial.println("chay sensor");
}
  if(buttonsen==LOW ){
Sensor->save(0);
Serial.println("tat sensor");}
*/
  
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(10))) {
     
    if (subscription == &Light1) {
      
      Serial.print(F("Light 1 got: "));
      Serial.println((char *)Light1.lastread);
      int Light1_State = atoi((char *)Light1.lastread);
      count++;
      digitalWrite(Relay1, Light1_State);}
      
    if (subscription == &Light2) {
      Serial.print(F("Light 2 got: "));
      Serial.println((char *)Light2.lastread);
      int Light2_State = atoi((char *)Light2.lastread);
      count1 ++;
      digitalWrite(Relay2, Light2_State);}
    }

 // tSensor.publish(kq);
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
