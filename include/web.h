#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <index.h>
#include <rgb.h>
#include <credentials.h>

AsyncWebServer server(80);
String ledStatus = "LED off";

String liveVals(const String& var){
  if(var == "USERINPUT"){
    String interactive = "";
    interactive += "<button id=\"onoff\" onclick=\"changestatus()\">" + ledStatus + "</button><br>";
    interactive += "<label>RED</label><br><input type=\"range\" onchange=\"sendColor()\" oninput=\"updateColor()\" value=" + String(lastColorR) + " min=0 max=255 id=\"colorInputR\"></input><br>";
    interactive += "<label>GREEN</label><br><input type=\"range\" onchange=\"sendColor()\" oninput=\"updateColor()\" value=" + String(lastColorG) + " min=0 max=255 id=\"colorInputG\"></input><br>";
    interactive += "<label>BLUE</label><br><input type=\"range\" onchange=\"sendColor()\" oninput=\"updateColor()\" value=" + String(lastColorB) + " min=0 max=255 id=\"colorInputB\"></input><br>";
    //interactive += "<button id=\"apply\" onclick=\"sendColor()\">SEND</button>";
    return interactive;
  }
  return String();
}

String handleLed() {
    if (ledStatus == "LED on") {
        turnOff();
        ledStatus = "LED off";
    } 
    else if (ledStatus == "LED off") {
        turnOn();
        ledStatus = "LED on";
    }
    Serial.println(ledStatus);
    return ledStatus;
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
        request->send_P(200, "text/html", ledIndex, liveVals);
    });
    server.on("/ledToggle", HTTP_GET, [] (AsyncWebServerRequest *request) {
        request->send(200, "text/plain", handleLed());
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
        request->send(200, "text/plain", "{\"r\":" + inputR + ", \"g\":" + inputG + ",\"b\":" + inputB + "}");
    });
    server.begin();
}