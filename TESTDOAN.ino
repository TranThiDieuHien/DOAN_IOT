#include <DHT.h>  // Including library for dht
#include <ESP8266WiFi.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);
//0x27 là địa chỉ màn hình trong bus I2C. 
//16 là số cột của màn hình 
//2 là số dòng của màn hình

String apiKey = "L4UP07991VIZ8ZUW";       // Enter your Write API key from ThingSpeak
const char* ssid = "Dat Do";           // Give your wifi network name
const char* password = "trandatdo";   // Give your wifi network password
const char* server = "api.thingspeak.com";
#define DHTPIN D7       //pin D0 where the dht11 is connected
#define LEDPIN 16
DHT dht(DHTPIN, DHT11);
WiFiClient client;

void setup() 
{
       Wire.begin();
       Serial.begin(115200);
       delay(10);

       lcd.begin (16,2); // for 16 x 2 LCD module
       lcd.backlight();
       lcd.setBacklight(HIGH);
       lcd.home (); // set cursor to 0,0
       lcd.print("nhiet do :"); 
       lcd.setCursor (0,1); // go to start of 2nd line
       lcd.print("do am :");            // Nhấp nháy con trỏ ở vị trí cuối cùng

       pinMode(LEDPIN,OUTPUT);
       dht.begin();
       Serial.println("Connecting to ");
       Serial.println(ssid);
       WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
}

void loop() 
{
      float h = dht.readHumidity();
      float t = dht.readTemperature();
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {    
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr += "\r\n\r\n";

                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);

                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.println("%. Send to Thingspeak.");

                             // Hiển thị kết quả đo ra màn hình lcd 16x2
                             lcd.setCursor (11,0); 
                             lcd.print(t); 
                             lcd.setCursor (9,1); // go to start of 2nd line
                             lcd.print(h); 

                             // Bật đèn nếu nhiệt độ cao hơn 30°C
                             if (t < 30) {
                              digitalWrite(LEDPIN, LOW);
                              Serial.println("The temp is low, turn on the led");
                             }
                            // Tắt đèn nếu nhiệt độ thấp hơn 30°C
                             else {
                               digitalWrite(LEDPIN, HIGH);
                               Serial.println("The temp is high, turn off the led");
                             }

                        }
          client.stop();

          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(30000);
}