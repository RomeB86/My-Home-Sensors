/*
    My first sketch ever.

    Made for the temperature and humidity sensor (GY21) and the D1 Mini Board with an ESP8266 microcontroller
    and a battery shield ordered at AZ-Delivery.de.

    The setup is powered by a 18650 Li-Ion Battery.
    Because of the battery I have added a DeepSleep mode to avoid charging the battery every few days.

    The battery shield keeps the voltage at a rate of 3.3V.    

    The sensor provides its data every 30 Minutes.

    A perfect way to monitor your home with the non consistant weather in Germany
    so you can avoid problems with your walls.

    Libraries needed:
      - ESP8266 (https://arduino.esp8266.com/stable/package_esp8266com_index.json)
      - GY21-master (thx to https://github.com/JonasGMorsch)
      - ArduinoOTA
      - PubSubClient
*/


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <GY21.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>


#define wifi_ssid "Darth-Fritz"                                              //  set own SSID
#define wifi_password "328ab7d2328AB7D2"                                      //  set own WIFI Password
#define mqtt_server "192.168.178.2"                                     //  set IP of your MQTT broker
#define mqtt_port "1883"
#define mqtt_user "admin"                                              //  set MQTT Username from your broker
#define mqtt_password "100986"                                 //  set MQTT Password from your broker        
#define ESPHostname "D1"                                              //  change Hostname if you have multiple sensors
String clientId = "ESP8266-";

#define temperature_topic "d1eltern/temperature"                            //  changing topic names from "d1" 
#define humidity_topic "d1eltern/humidity"                                  //  to "d1XX" makes sense
#define outTopic "d1eltern/outtopic"                                        //  when having multiple sensors in
#define inTopic "d1eltern/intopic"                                          //  different locations

float temp = 0.00;
float hum = 0.00;
float tempDiff = 0.2;
float humDiff = 1.0;
int sensorTakt = 5000; 

ESP8266WebServer server(80);
GY21 sensor;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

#ifdef ESP8266
#define SCL 05                                                        // D1 ON NODEMCU OR D1mini
#define SDA 04                                                        // D2 ON NODEMCU OR D1mini
#endif

// Connect VIN to 3V3 pin
// Connect GND to ground
// Connect SCL to I2C clock pin
// Connect SDA to I2C data pin

void setup()
{ 
  Serial.begin(115200);             
  delay(1000);                      

  WiFi.begin(wifi_ssid,wifi_password);        
  Serial.print("Connecting to ");
  Serial.print(wifi_ssid);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected! IP Adress: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    Serial.println();
    Serial.println("Starting Output:");

  server.begin();

 


  ArduinoOTA.setHostname(ESPHostname);
  ArduinoOTA.setPassword(mqtt_password);
  ArduinoOTA.begin();

#ifdef ESP8266
  Wire.begin(SDA, SCL);
#else
  Wire.begin();
#endif

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  if (!ESP8266) {                                         
    Serial.println("CHECK YOUR SETTINGS!!!");
    while (true);
  }


}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle(); 
  long now = millis();
  if (now - lastMsg > sensorTakt) {   
    lastMsg = now;
    getGY21Values();
  }

  server.handleClient();

  float temp = sensor.GY21_Temperature();
  float hum = sensor.GY21_Humidity();

//  Output to Serial Monitor if you want

/*
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print("°C    ");
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println("\%");

  delay(10000);
*/


// Code for DeepSleep: connect RST and D0 after uploading sketch!!!

  delay(10000);                                                       // 10 seconds to connect to MQTT and send your data

  Serial.println("Going DeepSleep");
  ESP.deepSleep(1800e6);                                              // DeepSleep for 30 minutes (1800 seconds * 10^6 - where 1s=1.000.000us)
}
void getGY21Values() {
  float newTemp = sensor.GY21_Temperature();
  float newHum = sensor.GY21_Humidity();

  if(checkBound(newTemp, temp, tempDiff)) {
    temp = newTemp;
    Serial.print("Temperatur: ");
    Serial.println(String(temp).c_str());
    client.publish(temperature_topic, String(temp).c_str(), true);
    }
  
  if(checkBound(newHum, hum, humDiff)) {
    hum = newHum;
    Serial.print("Feuchtigkeit: ");
    Serial.println(String(hum).c_str());
    client.publish(humidity_topic, String(hum).c_str(), true);
    }  
}

void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == '1') {
    //Room for Code
  } else {
    // Room for Code
  }

}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
  (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, ESPHostname);
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

