#include "WiFi.h"
#include "ArduinoJson.h"
#include "NeoPixelBus.h"
#include "NeoPixelAnimator.h"
#include "testanimation.h"


const char* ssid = "DP iPhone";
const char* password =  "escaflown2021";

IPAddress ip;

WiFiServer wifiServer(2000);

DynamicJsonDocument doc(10000);
String json = "";

boolean NewData = false;

#define PIN 27

 const uint16_t PixelCount = 23;
///////
MyAnimationState animationState[PixelCount];
NeoPixelAnimator animations(PixelCount); 
///////

// Paramètre 1 = Le nombre de NéoPixels chainés
// Paramètre 2 = No de broche de données (Sur votre Arduino, la plupart convient)
// Paramètre 3 = Type de pixel (flags/drapeaux), a combiner ensemble en fonction du besoin:
//   NEO_KHZ800  flux de données à 800 KHz (plupart des NéoPixel basé sur les LEDs w/WS2812)
//   NEO_KHZ400  flux de données à 400 KHz (Pour les Pixels classiques 'v1' FLORA (pas les V2) pilotés par WS2811)
//   NEO_GRB     Pixels sont raccordés en flux de donnée GRB (GRB=Green,Red,Blue=Vert,Rouge,Bleu - la plupart des produits NéoPixel)
//   NEO_RGB     Pixels sont raccordés en flux de donnée RGB (RGB=Red,Green,Blue=Rouge,Vert,Bleu - Pixels FLORA v1, pas la v2)
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(20, PIN, NEO_GRB + NEO_KHZ800);

NeoPixelBus<NeoGrbFeature, NeoEsp32BitBang800KbpsMethod> strip(PixelCount, PIN);

 String cmd = "0,0,0#";
 String INFO = "";
 boolean LightConnected = false;


void setup() {
  Serial.begin(115200);
    xTaskCreate(
      animated,
      "additional Task",
      10000,
      NULL,
      2,
      NULL);
      
  Serial.println("Heon LED Controller");
  Serial.println("-------------------");
  WiFi.begin(ssid, password);
  
  INFO="ERREUR";
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected !");
  ip = WiFi.localIP();
  Serial.println(ip);
  wifiServer.begin();

  //strip.begin();
  strip.Show();



  strip.Begin();
    strip.Show();

    SetRandomSeed();

  



}




void SetupLEd(String json){
  
  if (NewData) {
    //strip.Clear();
  //Serial.println("DEBUT");
  int pixel[256][3];
  int r = 0;
  int t = 0;
  int p = 0;
  int pix = 0;

  

  for (int i = 0; i < json.length(); i++)
  {
    if (json.charAt(i) == ',' || json.charAt(i) == '#' ) {
      pixel[t/3][p] =  json.substring(r, i).toInt();
//     
//     
      
      if (p == 2){
        RgbColor color(pixel[t/3][0], pixel[t/3][1], pixel[t/3][2]);
        
        PutColor(t/3,color);
        pix = t/3;
        //strip.SetPixelColor(t/3,color);
//        
//        Serial.print(t/3);
//        Serial.print(" R:");
//        Serial.print(pixel[t/3][0]);
//        Serial.print(" G:");
//        Serial.print(pixel[t/3][1]);
//        Serial.print(" B:");
//        Serial.println(pixel[t/3][2]);
        
        
        p = 0;
      } else {
        p++;
      }
       r = (i + 1);
      t++;
    }

  }

//bourrage
 RgbColor Black(0,0,0);
  for (pix++; pix < PixelCount+1; pix++)
  {
    //Serial.println(pix);
    PutColor(pix,Black);
  }
  
  NewData = false;
 //Serial.println("FIN");
 //strip.Show(); ------------------------------------- A REMETTRE
  
}
}

void animated(void * parameter) {
  for (;;){
    if (animations.IsAnimating())
    {
        // the normal loop just needs these two to run the active animations
        animations.UpdateAnimations();
        strip.Show();
    }
    else
    {
        // no animations runnning, start some 
        //
       
       
        if (INFO == "ERREUR") {
          Erreur();
        } 
        if (INFO == "CONNECTED" && !LightConnected) {
          Connected();
        } 
        if (INFO == "DATA") {
          
          SetupLEd(cmd);
        } 
        
        
        
        

        
       
        //PickRandom(0.2f); // 0.0 = black, 0.25 is normal, 0.5 is bright
    }
    delay(10);
  }
   vTaskDelete( NULL );
   

}


// the loop function runs over and over again forever
void loop() {

    
 WiFiClient client = wifiServer.available();
  //Serial.println("En attente");
    INFO="ERREUR";
  
  LightConnected = false;
  
    if (client) {
       Serial.println("Client Connected");
       INFO="CONNECTED";
       
      while (client.connected()) {
        
        String text = "";
        while (client.available() > 0) {
          
          char c = client.read();
          if (c != '\n') {
            text += c;
            
            //Serial.write(c);
          } else {
            INFO="DATA";
//            Serial.println("Commande trouve");
//            Serial.println(text);
            cmd = text;
            NewData = true;
            //SetupLEd(cmd);
            text = "";
            
           

          }

        }

      }
       
      client.stop();
      Serial.println("Client disconnected");
    }

  //delay(10);
}
