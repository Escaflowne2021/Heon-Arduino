#include "WiFi.h"
#include "ArduinoJson.h"
#include "Adafruit_NeoPixel.h"

const char* ssid = "DP-and-DPL";
const char* password =  "escaflowne";

IPAddress ip;

WiFiServer wifiServer(2000);

DynamicJsonDocument doc(10000);
String json = "";

boolean NewData = false;

#define PIN 27

// Paramètre 1 = Le nombre de NéoPixels chainés
// Paramètre 2 = No de broche de données (Sur votre Arduino, la plupart convient)
// Paramètre 3 = Type de pixel (flags/drapeaux), a combiner ensemble en fonction du besoin:
//   NEO_KHZ800  flux de données à 800 KHz (plupart des NéoPixel basé sur les LEDs w/WS2812)
//   NEO_KHZ400  flux de données à 400 KHz (Pour les Pixels classiques 'v1' FLORA (pas les V2) pilotés par WS2811)
//   NEO_GRB     Pixels sont raccordés en flux de donnée GRB (GRB=Green,Red,Blue=Vert,Rouge,Bleu - la plupart des produits NéoPixel)
//   NEO_RGB     Pixels sont raccordés en flux de donnée RGB (RGB=Red,Green,Blue=Rouge,Vert,Bleu - Pixels FLORA v1, pas la v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

 String cmd = "";

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

  strip.begin();
  strip.show();


//  xTaskCreate(
//    wifiRun,
//    "Wifi",
//    10000,
//    NULL,
//    1,
//    NULL);


}

void LedTask(void * parameter){

  vTaskDelete(NULL);
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
//            Serial.println("Commande trouve");
//            Serial.println(text);
            json = text;
            NewData = true;
            text = "";

          }

        }

      }
      client.stop();
//      Serial.println("Client disconnected");
    }
  }
  vTaskDelete(NULL);
}


void SetupLEd(String json){
  if (NewData) {
  Serial.println("DEBUT");
  int pixel[256][3];
  int r = 0;
  int t = 0;
  int p = 0;

  for (int i = 0; i < json.length(); i++)
  {
    if (json.charAt(i) == ',' || json.charAt(i) == '#' ) {
      pixel[t/3][p] =  json.substring(r, i).toInt();
//     
//     
      
      if (p == 2){
        strip.setPixelColor(t/3, pixel[t/3][0], pixel[t/3][1], pixel[t/3][2]);
        p = 0;
      } else {
        p++;
      }
       r = (i + 1);
      t++;
    }

  }
  NewData = false;
 Serial.println("FIN");
 strip.show();
  
}
}



// the loop function runs over and over again forever
void loop() {
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
//            Serial.println("Commande trouve");
//            Serial.println(text);
            cmd = text;
            NewData = true;
            SetupLEd(cmd);
            text = "";
           

          }

        }

      }
      client.stop();
//      Serial.println("Client disconnected");
    }

  //delay(10);
}
