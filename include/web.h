#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <index.h>
#include <rgb.h>
#include <credentials.h>

AsyncWebServer server(80);

void handleLedOn() {
    turnOn();
}

void handleLedOff() {
    turnOff();
}

void initWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

void initWeb() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", ledIndex);
    });
    server.on("/ledOn", HTTP_GET, [] (AsyncWebServerRequest *request) {
        handleLedOn();
        Serial.println("LED ON");
        request->send(200, "text/plain", "OK");
    });
    server.on("/ledOff", HTTP_GET, [] (AsyncWebServerRequest *request) {
        handleLedOff();
        Serial.println("LED OFF");
        request->send(200, "text/plain", "OK");
    });
    server.on("/color", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String inputR;
        String inputG;
        String inputB;
        if (request->hasParam("r") && request->hasParam("g") && request->hasParam("b")) {
            inputR = request->getParam("r")->value();
            inputG = request->getParam("g")->value();
            inputB = request->getParam("b")->value();
        } else {
            inputR = "No message sent";
            inputG = "No message sent";
            inputB = "No message sent";
        }
        Serial.println(inputR);
        Serial.println(inputG);
        Serial.println(inputB);
        writeToStrip(inputR.toInt(), inputG.toInt(), inputB.toInt());
        Serial.println("COLOR CHANGED");
        request->send(200, "text/plain", "OK");
    });
    server.begin();
}