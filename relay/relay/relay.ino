#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "signal.h"

#define BAUD_RATE 74880

using namespace websockets;

WebsocketsClient client;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println();

  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while(WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
  }
  pinMode(DATA_PIN, OUTPUT);

  connect();
}

void connect() {
  // create a new client
  client = WebsocketsClient(client);
  
  // run callback when messages are received
  client.onMessage(onMessageCallback);
  
  // run callback when events are occuring
  client.onEvent(onEventsCallback);

  // Before connecting, set the ssl fingerprint of the server
  client.setInsecure();

  // Connect to server
  client.connect(SECRET_SERVER);
}

long lastPing = 0;

void loop() {
  client.poll();
  if(millis() - lastPing > 5000) {
    Serial.println("Sending periodic ping");
    lastPing = millis();
    client.ping();
  }
}

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, message.data());
    long chan = doc["channel"];
    const char* btn = doc["button"];

    Serial.printf("Channel = %d, btn = %s\n", chan, btn);

    bool result = sendCode(chan, btn);
    Serial.printf("Code send: %s", result ? "successful" : "failed");
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
      Serial.println("Connnection Opened");
      // Send a ping
      client.ping();
    } else if(event == WebsocketsEvent::ConnectionClosed) {
      CloseReason reason = client.getCloseReason();
      Serial.printf("Connnection Closed. Reason [%s], reconnect.\n", reason);
      connect();
    } else if(event == WebsocketsEvent::GotPing) {
      Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
      Serial.println("Got a Pong!");
    }
}
