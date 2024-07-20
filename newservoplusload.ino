// Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL3d0SvzOoE"
#define BLYNK_TEMPLATE_NAME "Petfeeder"
#define BLYNK_AUTH_TOKEN "sPJIlxg1MrbtD9BCTNRoBmpp4_fcwtxu"
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <HX711.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

// WiFi credentials
const char* ssid = "POCOM3";
const char* password = "wifihotspot";

// Telegram Bot credentials
#define BOTtoken "6678871722:AAFt8YjRzuIqqU5-ExX84WpQeLunbXCKUZk"
#define CHAT_ID "1310279629"

// Load cell setup
#define LOADCELL_DOUT_PIN  4
#define LOADCELL_SCK_PIN  5

HX711 scale;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const float weight_threshold = 250.0;  // Adjust this threshold based on your application



Servo servo;
int lastPosition = 0;
int onnPos = 1800;  // clockwise direction
int offPos = 1400;  // counter-clockwise direction

void setup() {
  Serial.begin(115200);
  
  // Load cell setup
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-810.4);  // You might need to adjust this calibration factor
  scale.tare();  // Reset the scale to 0
  
  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");  // get UTC time via NTP
    client.setTrustAnchors(&cert);     // Add root certificate for api.telegram.org
  #endif
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  
  // Blynk setup
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  servo.attach(D4);
}

void loop() {
  Blynk.run();
  
  if (scale.is_ready()) {
    float weight = scale.get_units(10);
    Serial.print("Weight: ");
    Serial.println(weight);
    
    if (weight >= weight_threshold) {
      bot.sendMessage(CHAT_ID, "Food is Dispensed", "");
      Serial.println("Message sent successfully");
      
      // Wait until weight is removed before checking again
      while (scale.get_units(10) >= weight_threshold) {
        delay(1000);
      }
    }
  } else {
    Serial.println("HX711 not found.");
  }
  delay(1000);  // Adjust the delay as needed
}

BLYNK_WRITE(V4) {
  int value = param.asInt();
  Serial.println(value);

  if (value == 1 && lastPosition != onnPos) {
    servo.attach(D4);
    servo.writeMicroseconds(onnPos);
    delay(500);
    servo.detach();
    lastPosition = onnPos;
  } else if (value == 0 && lastPosition != offPos) {
    servo.attach(D4);
    servo.writeMicroseconds(offPos);
    delay(500);
    servo.detach();
    lastPosition = offPos;
  }
}
