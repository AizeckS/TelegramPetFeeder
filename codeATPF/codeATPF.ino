#include <ESP8266WiFi.h>         //  
#include <ESP8266WiFiMulti.h>    // Підключення WiFi бібліотек    
#include <ESP8266HTTPClient.h>   // 
#include <WiFiClientSecureBearSSL.h> // 
#include <Servo.h>   // Піключення бібліотеки сервоприводу 
 
Servo servo1;    // створення об’єкту сервоприводу  
int servoPIN=D0;  //контакт сервоприводу 
 
int Y0=0;      // кут сервоприводу в закритому положенні 
int Y1=45;    // кут сервоприводу в відкритому положенні  
int t=40;            // затримка для режиму тряски  
 
const uint8_t fingerprint[20] = {}; //Відбиток SHA1 
 
ESP8266WiFiMulti WiFiMulti;  HTTPClient https;      
 
std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure); 
 
#define btnSend D2 
 
boolean pressedButton = false; 
boolean answer = false;
boolean firstTime=true; 
 
String payload; 
String userMessageId = ""; 
String timeMessage = "time";
String timeNewMessage = " "; 
 
String token  = "719976365:AAF4ydWGCUKXceXer0bQwAHFz4hTyk7E5w";     //токен бота 
String userId = "229136130";     //Telegram ID користувача 
 
void setup() {   
  pinMode(btnSend, INPUT); 
 
  Serial.begin(115200); 
 
  Serial.println();   
  Serial.println();   
  Serial.println(); 
 
  for (uint8_t t = 4; t > 0; t--) {     
    Serial.printf("[SETUP] WAIT %d...\n", t);     
    Serial.flush();     
    delay(1000);   } 
 
  WiFi.mode(WIFI_STA); 
 
  WiFiMulti.addAP("arduinobot", "arduinobot"); // логін та пароль до WiFi 
  } 
  void sendBotMessage(String message) {  
    HTTPS_GET("https://api.telegram.org/bot" + token + "/sendMessage?chat_id=" + userId + "&parse_mode=html&text=" + message); } 
    String readBotMessage() {  
      HTTPS_GET("https://api.telegram.org/bot" + token + "/getUpdates");   return answerBot(); } 
    void HTTPS_GET(String messageLink) {   Serial.print("[HTTPS] begin...\n");   
    if (https.begin(*client, messageLink)) {     
      Serial.print("[HTTPS] GET...\n");    
  int httpCode = https.GET(); 
 
    if (httpCode > 0) 
    {       Serial.printf("[HTTPS] GET... code: % d\n", httpCode);      
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) { payload = https.getString(); Serial.println(payload); } }
      else {       Serial.printf("[HTTPS] GET... failed, error: % s\n", https.errorToString(httpCode).c_str());     }     https.end();   } 
      else {     Serial.printf("[HTTPS] Unable to connect\n");   } } String answerBot() { 
 
  int posId = payload.lastIndexOf("{\"id\":");   
  userMessageId = payload.substring(posId + 6);   
  posId = userMessageId.indexOf("\"");   
  userMessageId = userMessageId.substring(0, posId - 1); 
 
  int posDate = payload.lastIndexOf("\"date\":");    
  timeNewMessage = payload.substring(posDate + 6);   
  posDate = timeNewMessage.indexOf("\"");   
  timeNewMessage = timeNewMessage.substring(0, posDate - 1); 
 
  int posText = payload.lastIndexOf("\"text\":\"");   
  String MessageText = payload.substring(posText + 8);   
  posText = MessageText.indexOf("\"");  
  MessageText = MessageText.substring(0, posText);    
  // if (timeMessage == "time"); MessageText= " ";     return MessageText; } 
 
void loop() { 
 
  if ((WiFiMulti.run() == WL_CONNECTED)) { 
 
    client->setFingerprint(fingerprint); 
 
    if (firstTime) {sendBotMessage("Привіт, годівниця готова до роботи! Напишіть /feed для разового годування");      //повідомлення при ввімкненні присторю     
    firstTime=false;     delay(3000);     }            
    String botCommand = readBotMessage(); 
 
    if (userMessageId.equals(userId) && timeNewMessage!=timeMessage) {     
      timeMessage=timeNewMessage;     // if  owner's  command and new message 
 
    if   (botCommand.equals("/feed")){       
      Open () ;       
      sendBotMessage("Годування проведено");}        
 
    else if   (botCommand.equals("/start")) {         
      String welcome = "Для годування відправте повідомлення /feed";     // початкове повідомлення          
    sendBotMessage(welcome);      }     }       
    Serial.println("Wait 7s before next round...");       
    delay(3000);       } } 
 
void Open() //метод повороту сервоприводу 
{  servo1.attach(servoPIN);    int pos;    for(pos = Y0; pos < Y1; pos += 5)    // цикл плавного відкривання з кроком 5 градусів   
{     servo1.write(pos);     
delay(20);           }      
delay(20);              
 
  for(pos = Y1; pos >= Y0; pos -= 8) // цикл закривання з режимом тряски  
  {                                   
    servo1.write(pos);       
    delay(t);                  
    servo1.write(pos-5);       
    delay(t);     
    servo1.write(pos);       
    delay(t);     }   
    for(int i = 0; i <= 3; i += 1) //додаткова тряска  
    {     
      servo1.write(14);      
      delay(400);     
      servo1.write(Y0);           
      delay(200);   }       
      delay(1500);      
      servo1.detach();  } 