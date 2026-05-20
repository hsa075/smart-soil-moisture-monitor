#include <SoftwareSerial.h>

SoftwareSerial esp8266(10, 11); // RX, TX

String apiKey = "YOUR_THINGSPEAK_API_KEY";

const char* ssid = "Wokwi-GUEST";
const char* password = "";

int soilPin = A0;

int redLED = 6;
int greenLED = 7;

void setup() {

  Serial.begin(9600);
  esp8266.begin(115200);

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  Serial.println("Smart Soil Moisture Monitor");

  connectWiFi();
}

void loop() {

  int moisture = analogRead(soilPin);

  Serial.print("Soil Moisture: ");
  Serial.println(moisture);

  // Dry Soil
  if (moisture > 700) {

    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);

    Serial.println("Soil Dry");

  }

  // Moist Soil
  else {

    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);

    Serial.println("Soil Moist");

  }

  sendData(moisture);

  delay(15000);
}

void connectWiFi() {

  sendCommand("AT", 2000);

  sendCommand("AT+CWMODE=1", 2000);

  String cmd = "AT+CWJAP=\"";
  cmd += ssid;
  cmd += "\",\"";
  cmd += password;
  cmd += "\"";

  sendCommand(cmd, 5000);

  Serial.println("WiFi Connected");
}

void sendData(int moisture) {

  String cmd = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80";
  sendCommand(cmd, 3000);

  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr += "&field1=";
  getStr += String(moisture);
  getStr += "\r\n";

  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());

  sendCommand(cmd, 2000);

  esp8266.print(getStr);

  Serial.println("Data Sent to ThingSpeak");

  delay(3000);

  sendCommand("AT+CIPCLOSE", 2000);
}

void sendCommand(String command, const int timeout) {

  esp8266.println(command);

  long int time = millis();

  while ((time + timeout) > millis()) {

    while (esp8266.available()) {

      char c = esp8266.read();

      Serial.write(c);
    }
  }
}