#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
const char* ssid = "SSID";
const char* password = "PASSWORD";


#include <GY21.h>

#ifdef ESP8266
#define SCL 05 // D1 ON NODEMCU
#define SDA 04 // D2 ON NODEMCU
#endif

// Connect VIN to 3-5VDC
// Connect GND to ground
// Connect SCL to I2C clock pin
// Connect SDA to I2C data pin

GY21 sensor;

void setup()
{ 
  Serial.begin(115200);             //Serielle Ausgabe  an den Monitor
  delay(1000);                      //Warteschleife 1s

  WiFi.begin(ssid,password);        //Initiierung der Verbindung zum Netzwerk
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ... ");
  delay(20000);                     //20s Wartezeit bis zur Verbindung

  Serial.println("Connected!");
  Serial.print("IP Adresse:\t");
  Serial.println(WiFi.localIP());   //lokale IP Adresse anzeigen
  delay (5000);  

  Serial.println("Start");

#ifdef ESP8266
  Wire.begin(SDA, SCL);
#else
  Wire.begin();
#endif

}

void loop()
{
  float temp = sensor.GY21_Temperature();
  float hum = sensor.GY21_Humidity();

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print("°C    ");
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println("\%");

  delay(10000);
}
