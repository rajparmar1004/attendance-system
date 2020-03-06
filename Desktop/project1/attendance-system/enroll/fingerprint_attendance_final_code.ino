
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <WiFi.h>


const char* NAME;
const char* ID;


String Event_Name = "fingerprint";
String Key = "dfIIn12i9FQxryyybc-Mrew3Nnxyfwi_gk9aDcBYivE";
String resource = "/trigger/" + Event_Name + "/with/key/" + Key;
const char* server = "maker.ifttt.com";
const char* ssid     = "raj";
const char* password = "rajj1000";

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
  while (!Serial);  
  delay(100);
  Serial.println("\n\nFinger detect test");

  
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");

  Serial.print("Connecting to: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  int timeout = 10 * 4; // 10 seconds
  while (WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect, going back to sleep");
  }

  Serial.print("WiFi connected in: ");
  Serial.print(millis());
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  getFingerprintIDez();
  if (finger.fingerID == 36) {

    Serial.print("!!--");
    Serial.println(finger.fingerID);
    NAME = "RAJ";
    ID = "36";
    if (finger.confidence >= 60) {
      Serial.print("Attendace Marked for "); Serial.println(NAME);
      makeIFTTTRequest();
      
    }

  }

  if (finger.fingerID == 2 ) {
    Serial.print("!!--");
    Serial.println(finger.fingerID);
    digitalWrite(2, LOW);
    NAME = "NIKUNJ";
    ID = "2";
    if (finger.confidence >= 60) {
      Serial.print("Attendace Marked for "); Serial.println(NAME);
      makeIFTTTRequest();
      
    } 

  }



  if (finger.fingerID == 45 ) {
    Serial.print("!!--");
    Serial.println(finger.fingerID);
    digitalWrite(2, LOW);
    NAME = "BB";
    ID = "45";
    if (finger.confidence >= 60) {
      Serial.print("Attendace Marked for "); Serial.println(NAME);
      makeIFTTTRequest();
      
    } 

  }

  
  finger.fingerID = 0;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}


int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;


  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}


void makeIFTTTRequest() {
  Serial.print("Connecting to ");
  Serial.print(server);

  WiFiClient client;
  int retries = 5;
  while (!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!!!client.connected()) {
    Serial.println("Failed to connect...");
  }

  Serial.print("Request resource: ");
  Serial.println(resource);

  // Temperature in Celsius
  String jsonObject = String("{\"value1\":\"") + NAME + "\",\"value2\":\"" + ID
                      + "\"}";

  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server);
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);

  int timeout = 5 * 10; // 5 seconds
  while (!!!client.available() && (timeout-- > 0)) {
    delay(100);
  }
  if (!!!client.available()) {
    Serial.println("No response...");
  }
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("\nclosing connection");
  client.stop();
}
