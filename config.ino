#include <ArduinoJson.h>
#include "FS.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
//Defining Web Server
ESP8266WebServer server(80);
const char * ssid = "yoo";
const char * password = "323tay053";
IPAddress ap_local_IP(192,168,4,1);
IPAddress ap_gateway(192,168,1,116);
IPAddress ap_subnet(255,255,255,0);
char but0=4,but1=5,but2=12,but3=13;

//-----------------------------
File fsUploadFile;

//format bytes--------------------------------------------------------
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}
//--------------------loadConfig-------------------------------------------
const char * loadConfig( String  value) {
  const char * x="none";
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    x="false";
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    x="false";
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    x="false";
  }
  x= json[value];
  return x;
}

//======================================
//-----------------saveConfig--------------------------------
bool saveConfig(String data,String value) {
  data.trim();
  value.trim();
 File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }
  json[data]=value;
  configFile.close();
  configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}
//==========================
//---------handleSubmit--------------------------
void handleSubmit(){//dispaly values and write to memmory
  const char * test;
 if(!saveConfig("ssid",String(server.arg("ssid")))){
  Serial.println("error saving ssid");
  }else {
    test="saved";
    }
    if(!saveConfig("password",String(server.arg("password")))){
   Serial.println("error saving pass");
  }else {
    test="saved";
    }
String response ="<!DOCTYPE html>";
response +="<html>";
response +="<title>W3.CSS</title>";
response +="<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
response +="<link rel=\"stylesheet\" href=\"w3.css\">";
response +="<link rel=\"stylesheet\" href=\"styles.css\">";
response +="<body>";
response +="<div class=\"w3-container w3-light-blue\">";
response +="<p>NA</p><p class=\"w3-display-topright\">uptime</p>";
response +="</div>";
response +="<p>";
response +="<table class=\"w3-table w3-bordered\">";
response +="<tr>";
response +="<td>SSID:</td>";
response +="<td id=\"ssid\">";
response +=server.arg("ssid");
response +="</td>";
response +="</tr>";
response +="<tr>";
 response +="<td>password:</td>";
response +="<td  id=\"password\">";
response +=server.arg("password");
response +="</td>";
response +="</tr>";
response +="</table>";
response +="<h4> This will work after reboot</h4>";
response +="<button class=\"w3-button  w3-box w3-blue\"><a href=\"/reboot\">REBOOT</a></button>";
response +="<footer class=\"w3-container w3-blue\" ID=\"footer\">";
response +="<h3>Footer</h3>";
response +="</footer>";
response +="</body>";
response +="</html>";
  server.send(200, "text/html", response);
 //calling function that writes data to memory 
 
}
void reboot(){
 if (!handleFileRead("/reb.html")) server.send(404, "text/plain", "FileNotFound");
  delay(5000);
    ESP.restart();
  }
//=======================================
void handleRoot(){
if (server.hasArg("ssid")&& server.hasArg("password") ) {//If all form fields contain data call handelSubmit()
    handleSubmit();
  }
  else {
    if (!handleFileRead("/")) server.send(404, "text/plain", "FileNotFound");
  }
}
void setup() {
  pinMode(but0,OUTPUT);
  pinMode(but1,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  delay(1000);
  int i=0;
  Serial.begin(115200);//Starting serial comunication
   Serial.println("Mounting FS...");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }
    ssid =loadConfig("ssid");
  if (ssid=="false") {
    Serial.println("Failed to load ssid");
  } else {
    Serial.println("ssid loaded");
    Serial.println(ssid);
  }
      password =loadConfig("password");
  if (password=="false") {
    Serial.println("Failed to load ssid");
  } else {
    Serial.println("pass loaded");
    Serial.println(password);
  }
  Serial.print("connecting....");
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

// Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    i++;
    if(i==15){
  Serial.print("Configuring access point...");
  Serial.print("Setting soft-AP configuration ... ");
  WiFi.softAPConfig(ap_local_IP, ap_gateway, ap_subnet);
  Serial.print("Setting soft-AP ... ");
  WiFi.softAP("esp", "espespesp");
  Serial.print("Soft-AP IP address = ");
 
  break;
      }
  }



  delay(1000);

  Serial.println("");
  Serial.print(ssid);
  Serial.println(" is Connected  ");
  
  Serial.print("IP address: ");
  
 if(WiFi.status()== WL_CONNECTED){
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  }else{
     Serial.println(WiFi.softAPIP());
    }
  //server.on("/", handleRoot);
    server.on("/setLED", handleLED);
  server.on("/readADC", handleADC);
   server.on("/but1", handlebut1);
  server.on("/styles.css", HTTP_GET, []() {
  if (!handleFileRead("/styles.css")) server.send(404, "text/plain", "FileNotFound");
  });
    server.on("/w3.css", HTTP_GET, []() {
  if (!handleFileRead("/w3.css")) server.send(404, "text/plain", "FileNotFound");
  });
      server.on("/", HTTP_GET, []() {
  if (!handleFileRead("/index.html")) server.send(404, "text/plain", "FileNotFound");
  });
  butboot();
        server.on("/rep", handleSubmit);
        server.on("/reboot", reboot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");



//  if (!saveConfig()) {
//    Serial.println("Failed to save config");
//  } else {
//    Serial.println("Config saved");
//  }
  ssid =loadConfig("ssid");
  if (ssid=="false") {
    Serial.println("Failed to load config");
  } else {
    Serial.println("Config loaded");
    Serial.println(ssid);
  }
  

}

void loop() {
    server.handleClient();//Checks for web server activity
}
void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  message +="<H2><a href=\"/\">go home</a></H2><br>";
  server.send(404, "text/plain", message);
}
void handleADC() {
   int Time = millis();
 String adcValue = String(Time);
 
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}
void handlebut1() {
     int Time = millis();
 String adcValue = String(Time);
 String but=loadConfig("but0");
        but+=",";
        but+=loadConfig("but1");
        but+=",";
        but+=loadConfig("but2");
        but+=",";
        but+=loadConfig("but3");
        but+=",";
        but+=loadConfig("ssid");
        but+=",";
        but+=adcValue;
        but+=",";
 server.send(200, "text/plane", but); //Send button value only to client ajax request
Serial.println(but);
}

void handleLED() {
 String data="but";
int LedNum=10;

 String t_state = server.arg("LEDstate"); //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
 String t_sta=server.arg("LEDstate1");
 int test=t_sta.toInt();
      switch (test) {
      case 0:
        LedNum=4;
        break;
      case 1:
         LedNum=5;
        break;
      case 2:
         LedNum=12;
        break;
      case 3:
         LedNum=13;
        break;
      default:
      ;
        }
 data+=t_sta;
  String ledState =loadConfig(data);
  if (ledState=="false") {
    Serial.println("Failed to load ssid");
  } else {
    Serial.println("ledState loaded");
    Serial.println(ledState);
  }
 Serial.println(t_state);
 if(t_state == "1")
 {
  digitalWrite(LedNum,HIGH); //LED ON
  ledState = "HIGH"; //Feedback parameter
  saveConfig(data,"HIGH");
 }
 else
 {
  digitalWrite(LedNum,LOW);
  ledState = "LOW"; //Feedback parameter
  saveConfig(data,"LOW"); 
 }
  Serial.print(data);
  Serial.print(":");
  Serial.println(ledState);

 server.send(200, "text/plane", ledState); //Send web page
}
void butboot(){
if(loadConfig("but0")=="HIGH"){
  digitalWrite(4,HIGH);
  }else {
  digitalWrite(4,LOW);    
    }
if(loadConfig("but1")=="HIGH"){
  digitalWrite(5,HIGH);
  }else {
  digitalWrite(5,LOW);    
    }
if(loadConfig("but2")=="HIGH"){
  digitalWrite(14,HIGH);
  }else {
  digitalWrite(14,LOW);    
    }
if(loadConfig("but3")=="HIGH"){
  digitalWrite(16,HIGH);
  }else {
  digitalWrite(16,LOW);    
    }
  }
