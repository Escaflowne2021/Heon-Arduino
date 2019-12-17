#include "WiFi.h"
#include "ArduinoJson.h"

const char* ssid = "DP-and-DPL";
const char* password =  "escaflowne";

IPAddress ip;

WiFiServer wifiServer(2000);

DynamicJsonDocument doc(10000);
String json = "";

void setup() {
  Serial.begin(115200);
  Serial.println("Heon LED Controller");
  Serial.println("-------------------");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected !");
  ip = WiFi.localIP();
  Serial.println(ip);
  wifiServer.begin();



  xTaskCreate(
    wifiRun,
    "Wifi",
    10000,
    NULL,
    1,
    NULL);


}

void wifiRun(void * parameter) {
  while (true) {
    WiFiClient client = wifiServer.available();
    if (client) {
      while (client.connected()) {
        String text = "";
        while (client.available() > 0) {
          char c = client.read();
          if (c != '\n') {
            text += c;
            //Serial.write(c);
          } else {
            Serial.println("Commande trouve");
            Serial.println(text);
            json = text;
            text = "";

          }

        }

      }
      client.stop();
      Serial.println("Client disconnected");
    }
  }
  vTaskDelete(NULL);
}






// the loop function runs over and over again forever
void loop() {
  Serial.println("-----------------");
  DeserializationError err=deserializeJson(doc, json);
  if(err) {
    Serial.print(F("deserializeJson() failed with code "));Serial.println(err.c_str());
  }
  JsonObject arr = doc.as<JsonObject>();
  int count = arr.size();
  int memoryUsed = doc.memoryUsage();
  String type = doc["type"];
  Serial.println(type);
  
  
  delay(1000);
  

}
