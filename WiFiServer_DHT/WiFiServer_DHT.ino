//  Adapted from HelloServer

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);

// following to allow unsigned updates from a browser
#include <ESP8266HTTPUpdateServer.h>
ESP8266HTTPUpdateServer httpUpdater;


#ifndef STASSID
#define STASSID "ropxxx"
#define STAPSK  "tonyxxx"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

//#include <NTPClient.h>
//#include <WiFiUdp.h>
//// Define NTP Client to get time
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "pool.ntp.org");

#include "ESPDateTime.h" // New Library to get DateTime

void setupDateTime() {
  // setup this after wifi connected
  // you can use custom timeZone,server and timeout
  // DateTime.setTimeZone(-4);
  //   DateTime.setServer("asia.pool.ntp.org");
  //   DateTime.begin(15 * 1000);
  DateTime.setServer("pool.ntp.org");
  //DateTime.setTimeZone("CST-8");
  DateTime.begin();
  if (!DateTime.isTimeValid()) {
    Serial.println("Failed to get time from server.");
  }
}


#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 5     // Digital pin connected to the DHT sensor D1

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated
// Updates DHT readings every 10 seconds
const long interval = 10000;

//Scale code when we implement

//#include "HX711.h"
//#define DOUT  13 //D7
//#define CLK  15 //D8
//HX711 scale;
 
//Change this calibration factor as per your load cell once it is found you many need to vary it in thousands and save it to EEPROM
float calibration_factor = 1.0; //-106600 worked for my 40Kg max scale setup


const int MaxRows=200;
const int MaxCol=2;
const int MaxLogRows=50;
int CurrentRowCount=0;
int ServedRowPointer=0;
String DataArray[MaxRows][MaxCol];
String TimeArray[MaxRows];
int CurrentLogCount=0;
String LogArray[MaxLogRows];
String timestamp;

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!\n  Open http://esp8266.local/update in your browser\n");
}

void handleDatahtml() {
 
  // Prepare the response
  String s = "<!DOCTYPE HTML>\r\n<html> <table>  <tr> <td>Time: <td> Temp <td> Humidity </tr> ";
  
  for (int i = 0; i < CurrentRowCount; i++) 
  {
    s +=  "<tr> <td>" + TimeArray[i]+"<td>"+DataArray[i][0]+"<td>"+DataArray[i][1]+"</tr>";
  } 
  s += " </table></html>\n";
  server.send(200, "text/html", s);
}
void handleDataTxt() {
 
  // Prepare the response
  String s = " ";
  
  if( ServedRowPointer > CurrentRowCount){ 
    // where the data buffer has looped over
      for (int i = ServedRowPointer; i < MaxRows; i++) {
      s +=  TimeArray[i]+"\t"+DataArray[i][0]+"\t"+DataArray[i][1]+"\n";
    } 
    ServedRowPointer=0;
  }
  
  for (int i = ServedRowPointer; i < CurrentRowCount; i++) 
  {
    s +=  TimeArray[i]+"\t"+DataArray[i][0]+"\t"+DataArray[i][1]+"\n";
  } 
  ServedRowPointer=CurrentRowCount;
  server.send(200, "text/plain", s);
}

void handleLogTxt() {
 
  // Prepare the response
  String s = "**Log Entries \n ";
  
  for (int i = 0; i < CurrentLogCount; i++) 
  {
    s +=  LogArray[i]+"\n";
  } 
  server.send(200, "text/plain", s);
}



void setup() {
  Serial.begin(115200);

 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  //enable the updates over web
  httpUpdater.setup(&server);
  // Start the server
  server.on("/", handleRoot);
  server.on("/Datahtml", handleDatahtml);
  server.on("/DataTxt", handleDataTxt);
  server.on("/LogTxt", handleLogTxt);
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

//  // Initialize a NTPClient to get time
//  timeClient.begin();
//  // Set offset time in seconds to adjust for your timezone, for example:
//  // GMT +1 = 3600
//  // GMT +8 = 28800
//  // GMT -1 = -3600
//  // GMT 0 = 0
//  timeClient.setTimeOffset(0);
//  timeClient.update();
  
  setupDateTime();
  
  String Timestring=DateTime.toUTCString();
  //LogArray[CurrentLogCount++]=timeClient.getFormattedTime() +": "+ "Webserver, NTP Started &  IP Addr:" + WiFi.localIP().toString() ;
  LogArray[CurrentLogCount++]=Timestring+": "+ "Webserver, NTP Started &  IP Addr:" + WiFi.localIP().toString() ;

  
  dht.begin();

  //Scale Logic
//  scale.begin(DOUT, CLK);
//  scale.set_scale();
//  scale.tare(); //Reset the scale to 0
// 
//  long zero_factor = scale.read_average(); //Get a baseline reading
//  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
//  Serial.println(zero_factor);
//  scale.set_scale(calibration_factor); //Adjust to this calibration factor

 
}

void loop() {

  //timeClient.update();
  server.handleClient();
  MDNS.update(); 
  
  
  unsigned long currentMillis = millis();
             
  if (currentMillis - previousMillis >= interval) {
    //Serial.print("UNITS: ");
    //Serial.print(scale.get_units(5));
    
    if( CurrentRowCount == MaxRows){
      CurrentRowCount=0;
    }
    if( CurrentLogCount == MaxLogRows){
      CurrentLogCount=0;
    }
    
    //TimeArray[CurrentRowCount]=timeClient.getFormattedTime();
    String Timestring=DateTime.toUTCString();
    TimeArray[CurrentRowCount]=Timestring;
    //TimeArray[CurrentRowCount]=DateTime.toUTCString();
    // save the last time you updated the DHT values
    Serial.print("Time: ");
    Serial.println(currentMillis);
    previousMillis = currentMillis;

    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    
    // Read Humidity
    float newH = dht.readHumidity();
    
    // if temperature or humidity read failed, don't change t value
    if (isnan(newT) || isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!"); 
      LogArray[CurrentLogCount++]=Timestring +": "+ "Failed to read from DHT sensor!" ;    
    }
    else{
      DataArray[CurrentRowCount][0]=String(newT,2);
      DataArray[CurrentRowCount][1]=String(newH,2);
    }    
    CurrentRowCount++;
  }
    
}
