#include <WiFi.h>
#include <time.h>
#include <string.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define DATE_STRING_LENGTH 30
StaticJsonDocument<200> doc;

const char* ssid       = "iPhome";
const char* password   = "20072000";

const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_username = "";
const char* mqtt_password = "";
const char* mqtt_topic = "Smartfarm/#";
WiFiClient espClient;
PubSubClient clientmqtt(espClient);

const char* host = "script.google.com";
const int httpsPort = 443;
String GAS_ID = "AKfycbxUyHw9_SOphgRA56ILyIqVT9oET6FE6x2xTLFJE6WRU7ZdHFh9rdB20zfoThCuRiOvGw"; //--> spreadsheet script ID
WiFiClientSecure client; //--> Create a WiFiClientSecure object.

// Declare global variables for NTP server and time offsets
const char* ntpServer = "1.th.pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 0;

int mode = 0;
uint8_t relay[4] = {14,27,26,25};

// Declare function prototypes
char* getDateTime();
int getDayOfWeek();
int getMonth();
int getDay(); 
void sendData();
void reconnect();
int soild_per();
int soilm_per();
void modewoking();

const int d_sf = 30;
const int d_sd = 65; 
const int m_sf = 30;
const int m_sd = 55; 
int monthsoild[] = {d_sf, d_sf, d_sf+getDay()/2, d_sf+getDay(),d_sf+getDay()/2, d_sf+getDay(),d_sd,d_sd,d_sd,d_sd,d_sd,d_sd};
int monthsoilm[] = {m_sf, m_sf, m_sf+getDay()/2, m_sf+getDay(),m_sd,m_sd,m_sd,m_sd,m_sd,m_sd,m_sd,m_sd};
int weeksoild[] = {d_sd, d_sd, d_sd, d_sd,d_sd,d_sd,d_sd};
int weeksoilm[] = {m_sd, m_sd, m_sd, m_sd,m_sd,m_sd,m_sd};
int daysoild = d_sd;
int daysoilm = m_sd;
int solenoidd = 0;
int solenoidm = 0;

// void onMessage;
void onMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message content: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  deserializeJson(doc, payload);
  mode = doc["mode"];
  Serial.println(mode);
  if((mode == 1)){
    if(doc["fruit"] == "Durian"){
      for(int i = 0; i < 12; i++) {
        monthsoild[i] = doc["data"][i]; // Assign the value of i to the i-th element of the array
      }
    }else if(doc["fruit"] == "Mangosteen"){
      for(int i = 0; i < 12; i++) {
        monthsoild[i] = doc["data"][i]; // Assign the value of i to the i-th element of the array
      }
    }
  }else if(mode == 2){
    if(doc["fruit"] == "Durian"){
      for(int i = 0; i < 7; i++) {
        weeksoild[i] = doc["data"][i]; // Assign the value of i to the i-th element of the array
      }
    }else if(doc["fruit"] == "Mangosteen"){
      for(int i = 0; i < 7; i++) {
        weeksoilm[i] = doc["data"][i]; // Assign the value of i to the i-th element of the array
      }
    }
  }
  else if(mode == 3){
    if(doc["fruit"] == "Durian"){
      daysoild = doc["data"];
    }else if(doc["fruit"] == "Mangosteen"){
      daysoilm = doc["data"];
    }
  }else if(mode == 4){
    if(doc["fruit"] == "Durian"){
      solenoidd = doc["data"];
    }else if(doc["fruit"] == "Mangosteen"){
      solenoidm = doc["data"];
    }
  }
}

void setup() {
  // Connect to WiFi
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(13, INPUT);
  pinMode(32, INPUT);
  pinMode(33, INPUT);
  pinMode(34, INPUT);
  pinMode(14, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(25, OUTPUT);
  
  WiFi.begin(ssid, password);
  Serial.begin(9600);
  Serial.printf("Connecting to %s ", ssid);
  Serial.print("");
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");
  // digitalWrite(relay[0],1);
  // delay(1000);
  // digitalWrite(relay[0],0);
  // Configure time using NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  clientmqtt.setServer(mqtt_server, 1883);
  clientmqtt.setCallback(onMessage);
  // Other setup code...
}

void loop() {
  sendData();
  // String url = "/macros/s/" + GAS_ID + "/exec?date=" + getDateTime() + "&temp="+temp+ "&humid="+humid+ "&soild="+soild_per()+ "&soilm="+soilm_per()+ "&water="+water; //  2 variables 
  // Serial.println(soild_per());
  // Serial.println(soilm_per());
  Serial.print(digitalRead(13));
  modewoking();
  if (!clientmqtt.connected()) {
    reconnect();
  }
  clientmqtt.loop();
  delay(300);
}

char* getDateTime() {
  static char dateTimeString[DATE_STRING_LENGTH];
  time_t now = time(NULL);
  struct tm* timeinfo = localtime(&now);

  strftime(dateTimeString, DATE_STRING_LENGTH, "%Y-%m-%d-%H:%M:%S", timeinfo);

  int dayOfWeek = timeinfo->tm_wday;
  const char* dayNames[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  sprintf(dateTimeString + strlen(dateTimeString), "-%s", dayNames[dayOfWeek]);

  return dateTimeString;
}

int getDayOfWeek() {
  time_t now = time(NULL);
  struct tm* timeinfo = localtime(&now);
  int dayOfWeek = timeinfo->tm_wday;
  return dayOfWeek;
  // switch (dayOfWeek) {
  //   case 0:
  //     return "Sunday";
  //   case 1:
  //     return "Monday";
  //   case 2:
  //     return "Tuesday";
  //   case 3:
  //     return "Wednesday";
  //   case 4:
  //     return "Thursday";
  //   case 5:
  //     return "Friday";
  //   case 6:
  //     return "Saturday";
  //   default:
  //     return "Invalid day of week";
  // }
}
int getHour(){
  time_t now = time(NULL);
  struct tm* timeinfo = localtime(&now);
  int hour = timeinfo->tm_hour;
  // int hour = (timeinfo->tm_sec)/2;
  return hour;
}
int getDay(){
  time_t now = time(NULL);
  struct tm* timeinfo = localtime(&now);
  int day = timeinfo->tm_mday;
  // int day = (timeinfo->tm_sec)/2;
  return day;
}
int getMonth() {
  time_t now = time(NULL);
  struct tm* timeinfo = localtime(&now);
  int month = timeinfo->tm_mon;
  return month;
  // int month = (timeinfo->tm_min);
  // switch (month) {
  //   case 0:
  //     return "January";
  //   case 1:
  //     return "February";
  //   case 2:
  //     return "March";
  //   case 3:
  //     return "April";
  //   case 4:
  //     return "May";
  //   case 5:
  //     return "June";
  //   case 6:
  //     return "July";
  //   case 7:
  //     return "August";
  //   case 8:
  //     return "September";
  //   case 9:
  //     return "October";
  //   case 10:
  //     return "November";
  //   case 11:
  //     return "December";
  //   default:
  //     return "Invalid month";
  // }
}

//soil 34 35 rain 2 temp 0 waterlever 4 relay 14 37 26 25 switch 13 
void sendData() {
  //----------------------------------------Connect to Google host
  
   
  //----------------------------------------

  //----------------------------------------Proses dan kirim data  
  // date temp soild soilm water
  int temp = analogRead(32); 
  int soild = analogRead(34);
  int soilm = analogRead(35);
  int water = digitalRead(13);
  int humid = analogRead(33);
  String url = "/macros/s/" + GAS_ID + "/exec?date=" + getDateTime() + "&temp="+temp+ "&humid="+humid+ "&soild="+soild_per()+ "&soilm="+soilm_per()+ "&water="+water*100; //  2 variables 
  Serial.println(url);
  client.setInsecure();
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //---------------------------------------
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println(line);
  
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //---------------------- 
}
//parameter
// d    m
// // 2800
// 3000
// 2000 3100
// // 1000 1000
int soild_per(){
  return 100 - ((analogRead(34)-1000)*100/2000);
}
int soilm_per(){
  return 100 - ((analogRead(35)-1000)*100/2000);
}
void modewoking(){
  switch(mode) {
    case 1:
      if(getHour()==5 and soild_per()< monthsoild[getMonth()]){
        digitalWrite(relay[0],1); 
        //เปิด sw
      }
      else{
        digitalWrite(relay[0],0);
        //ปิด sw
      }
      if(getHour()==5 and soild_per()< monthsoilm[getMonth()]){
        digitalWrite(relay[1],1); 
        //เปิด sw
      }
      else{
        digitalWrite(relay[1],0);
        //ปิด sw
      }
      break;
    case 2:
      if(getHour()==5 && soild_per()< weeksoild[getDayOfWeek()]){
        digitalWrite(relay[0],1); 
        //เปิด sw
      }
      else{
        digitalWrite(relay[0],0);
        //ปิด sw
      }
      if(getHour()==5 && soild_per()< weeksoilm[getDayOfWeek()]){
        digitalWrite(relay[1],1); 
        //เปิด sw
      }
      else{
        digitalWrite(relay[1],0);
        //ปิด sw
      }
      break;
    case 3:
      // Serial.println("daysoild = "+soild_per());
      // Serial.println("daysoilm = "+soilm_per());
      if( soild_per()< daysoild && soilm_per()< daysoilm){
        Serial.println("if1");
        digitalWrite(relay[1],1); 
        digitalWrite(relay[2],1); 
        digitalWrite(relay[3],1);
      }
      else if( soild_per()< daysoild && soilm_per()> daysoilm){
        Serial.println("if2");
        // digitalWrite(relay[0],0); 
        digitalWrite(relay[1],1); 
        digitalWrite(relay[2],1); 
        digitalWrite(relay[3],0);
      }else if( soild_per()> daysoild && soilm_per()< daysoilm){
        Serial.println("if3");
        // digitalWrite(relay[0],0); 
        digitalWrite(relay[1],1); 
        digitalWrite(relay[2],0); 
        digitalWrite(relay[3],1);
      }else{
        Serial.println("if4");
        digitalWrite(relay[1],0);
        digitalWrite(relay[2],0);  
        digitalWrite(relay[3],0); 
      }
      break;
    case 4:
      if(solenoidm == 1 && solenoidd == 1){
        digitalWrite(relay[1],1); 
        digitalWrite(relay[2],1); 
        digitalWrite(relay[3],1);
      }
      if(solenoidd == 1 && solenoidm == 0){
        digitalWrite(relay[1],1); 
        digitalWrite(relay[2],1); 
        digitalWrite(relay[3],0);
        //เปิด sw
      }else if(solenoidm == 1 && solenoidd == 0){
        digitalWrite(relay[1],1); 
        digitalWrite(relay[2],0); 
        digitalWrite(relay[3],1); 
        //เปิด sw
      }else if(solenoidm == 0 && solenoidd == 0){
        digitalWrite(relay[1],0); 
        digitalWrite(relay[2],0);
        digitalWrite(relay[3],0);
      }
      break;
    default:
      break;
    }
}

void reconnect() {
  while (!clientmqtt.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (clientmqtt.connect("ESP32_client", mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
      clientmqtt.subscribe(mqtt_topic);
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(clientmqtt.state());
      Serial.println("Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
