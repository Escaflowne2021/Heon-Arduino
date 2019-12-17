#include "WiFi.h"
#include "ArduinoJson.h"

const char* ssid = "DP-and-DPL";
const char* password =  "escaflowne";

IPAddress ip;

WiFiServer wifiServer(2000);

DynamicJsonDocument doc(10000);
String json = "";

boolean NewData = false;

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
            //Serial.println("Commande trouve");
            //Serial.println(text);
            json = text;
            NewData = true;
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
  //  Serial.println("-----------------");
  //  DeserializationError err=deserializeJson(doc, json);
  //  if(err) {
  //    Serial.print(F("deserializeJson() failed with code "));Serial.println(err.c_str());
  //  }
  //  JsonObject arr = doc.as<JsonObject>();
  //  int count = arr.size();
  //  int memoryUsed = doc.memoryUsage();
  //  String type = doc["type"];
  //  Serial.println(type);
  //  delay(1000);
  
  //String pixel[256];
  //int r = 0;
  //int t = 0;
  //
  //for (int i=0; i < json.length(); i++)
  //{
  // if(json.charAt(i) == '#')
  //  {
  //    pixel[t] = json.substring(r, i);
  //    Serial.println( pixel[t]);
  //    r=(i+1);
  //    t++;
  //  }
  //}
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
//      Serial.print( String(t/3) );
//       Serial.print( "-" );
//        Serial.print( String(p) );
//        Serial.print( "- String :" );
//         Serial.print( json.substring(r, i) );
//        Serial.print( "- Int :" );
//        Serial.print( json.substring(r, i).toInt() );
//         Serial.print( "- tableau :" );
//          Serial.println(pixel[t/3][p]);
//     
      
      if (p == 2){
//          Serial.print(pixel[t/3][0]);
//           Serial.print("/");
//           Serial.print(pixel[t/3][1]);
//           Serial.print("/");
//           Serial.print(pixel[t/3][2]);
//           Serial.print("/");
//           Serial.println("");
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
  
}
  //delay(10);
}
