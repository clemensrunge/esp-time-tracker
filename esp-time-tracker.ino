#include "themes.h"
#include <ESP8266WiFi.h>
#include "HTTPSRedirect/HTTPSRedirect.h"
#include "HTTPSRedirect/HTTPSRedirect.cpp"
#include <WiFiClientSecure.h>
#include "secrets.h"

const int buzzerPin = 5;

const int buttonPinRed = 2;
const int ledPinRed = 0;

const int buttonPinGreen = 16;
const int ledPinGreen = 13;

const unsigned long updateIntervallMillis = 60000;

int buttonStateRed = 0;
int buttonStateGreen = 0;
bool workState = false;

HTTPSRedirect* client = nullptr;
unsigned long nextUpdateMillis = updateIntervallMillis;

void setup();
void loop();
void error();
void connectToWifi();
void initHttpsClient();
bool isWorking();
void setWorkStateLeds();
void playTheme(int melody[], int noteDurations[], int length, int pin);

void setup(void) {
  pinMode(buzzerPin, OUTPUT);  //buzzer
  pinMode(ledPinRed, OUTPUT);
  digitalWrite(ledPinRed, HIGH);
  pinMode(ledPinGreen, OUTPUT);
  digitalWrite(ledPinGreen, HIGH);
  pinMode(buzzerPin, OUTPUT);

  pinMode(buttonPinRed, INPUT_PULLUP);
  pinMode(buttonPinGreen, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.flush();
  connectToWifi();
  initHttpsClient();
  workState = isWorking();
  setWorkStateLeds();
  nextUpdateMillis = millis() + updateIntervallMillis;
}

void loop() {

  bool oldWorkState = workState;
  if (nextUpdateMillis < millis()) {
    nextUpdateMillis = millis() + updateIntervallMillis;
    workState = isWorking();
    setWorkStateLeds();
    delay(300);
  }

  if (digitalRead(buttonPinGreen) == LOW) {
    workState = true;
  }

  if (digitalRead(buttonPinRed) == LOW) {
    workState = false;
  }

  if (workState != oldWorkState) {
    setWorkStateLeds();

    if (workState == false) {
      playTheme(underworld_melody, underworld_tempo, sizeof(underworld_melody) / sizeof(int), buzzerPin);
      if (isWorking()) {
        Serial.println("Send End Work");
        sendStartStop();
      }
    }

    if (workState == true) {
      playTheme(intro_melody, intro_tempo, sizeof(intro_melody) / sizeof(int), buzzerPin);
      if (!isWorking()) {
        Serial.println("Send Start Work");
        sendStartStop();
      }
    }
  }
}

void error() {
  while (true) {
    digitalWrite(ledPinRed, HIGH);
    digitalWrite(ledPinGreen, HIGH);
    delay(1000);
    digitalWrite(ledPinRed, LOW);
    digitalWrite(ledPinGreen, LOW);
    delay(1000);
  }
}

void connectToWifi() {
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void initHttpsClient() {
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(false);
  client->setContentTypeHeader("application/json");

  if (client != nullptr && !client->connected()) {
    client->connect(host, httpsPort);
  } else {
    Serial.println("Error creating client object!");
    error();
  }
}

bool isWorking() {
  Serial.print("GET Data from cell: ");
  Serial.println(cellAddress1);

  int retrys = 5;

  while (0 < retrys) {
    if (client->GET(url1, host)) {
      String payload = client->getResponseBody();
      payload.trim();
      Serial.println(payload);

      if (payload == "working") {
        return true;
      } else {
        return false;
      }
    } else {
      retrys--;
    }
  }

  Serial.println("Error getting working data!");
  error();
  return false;
}

void sendStartStop() {
  Serial.print("Set Start/Stop Data:");

  String state = "stop";
  if (workState) {
    state = "start";
  }
  int retrys = 5;

  while (0 < retrys) {
    if (client->GET(url2 + state, host)) {
      String payload = client->getResponseBody();
      payload.trim();
      Serial.println(payload);

      if (payload == "Success" || payload == "No start/stop update") {
        return;
      } else {
        Serial.println("Error getting setting working data!");
        error();
      }
    } else {
      retrys--;
    }
  }

  Serial.println("Error getting setting working data!");
  error();
}

void setWorkStateLeds() {
  digitalWrite(ledPinRed, !workState);
  digitalWrite(ledPinGreen, workState);
}

void playTheme(int melody[], int noteDurations[], int length, int pin) {
  for (int thisNote = 0; thisNote < length; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    const int noteDuration = 1000 / noteDurations[thisNote];
    const int pauseBetweenNotes = noteDuration;

    tone(buzzerPin, melody[thisNote], noteDuration);
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }
}
