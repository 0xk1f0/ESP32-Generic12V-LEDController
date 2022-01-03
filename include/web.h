#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <html/pages.h>
#include <rgb.h>

AsyncWebServer server(80);
String ledStatus = "LED off";
StaticJsonDocument<250> jsonDocument;
char buffer[250];

String liveVals(const String& var){
  if(var == "USERINPUT"){
    String interactive = "";
    interactive += "<label>RED</label><br><input type=\"range\" onchange=\"sendColor()\" oninput=\"updateColor()\" value=" + String(lastColorR) + " min=0 max=255 id=\"colorInputR\"></input><br>";
    interactive += "<label>GREEN</label><br><input type=\"range\" onchange=\"sendColor()\" oninput=\"updateColor()\" value=" + String(lastColorG) + " min=0 max=255 id=\"colorInputG\"></input><br>";
    interactive += "<label>BLUE</label><br><input type=\"range\" onchange=\"sendColor()\" oninput=\"updateColor()\" value=" + String(lastColorB) + " min=0 max=255 id=\"colorInputB\"></input><br>";
    interactive += "<button id=\"onoff\" onclick=\"changestatus()\">" + ledStatus + "</button><br>";
    return interactive;
  }
  return String();
}

String settingsVals(const String& var){
  if(var == "SETTINGS"){
    String interactive = "";
    interactive += "<ul id=\"info\">";
    interactive += "<li>Hostame: <span style=\"font-style:italic\">" + String(WiFi.getHostname()) + "</span></li>";
    interactive += "<li>WiFi SSID: <span style=\"font-style:italic\">" + String(WiFi.SSID()) + "</span></li>";
    interactive += "<li>Connection Status: <span style=\"font-style:italic\">" + String(WiFi.isConnected()) + "</span></li>";
    interactive += "<li>Signal Strength: <span style=\"font-style:italic\">" + String(WiFi.RSSI()) + "dBm</span></li>";
    interactive += "<li>GPIO-RED: <span style=\"font-style:italic\">" + String(LED_PIN_RED) + "</span></li>";
    interactive += "<li>GPIO-GREEN: <span style=\"font-style:italic\">" + String(LED_PIN_GREEN) + "</span></li>";
    interactive += "<li>GPIO-BLUE: <span style=\"font-style:italic\">" + String(LED_PIN_BLUE) + "</span></li>";
    interactive += "</ul>";
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
    WiFi.setHostname(hostname);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

void handleUpdate(AsyncWebServerRequest *request) {
  request->send(200, "text/html", updateIndex);
};

void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index){
    size_t content_len;  
    content_len = request->contentLength();
    int cmd = (filename.indexOf(F(".spiffs.bin")) > -1 ) ? U_SPIFFS : U_FLASH;
    if (cmd == U_FLASH && !(filename.indexOf(F(".bin")) > -1) ) return; // wrong image for ESP32
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
      Update.printError(Serial);
    }
  }
  if (Update.write(data, len) == len) {
    int progress = Update.progress() / 10000;
    Serial.println(progress);
  } else {
    Update.printError(Serial);
  }
  if (final) {    
    if (!Update.end(true)){
      Update.printError(Serial);
    } else {
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", updateDoneIndex); 
      request->send(response); 
      delay(100);
      ESP.restart();
    }
  }
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
        writeToStrip(inputR.toInt(), inputG.toInt(), inputB.toInt(), ledStatus);
        Serial.println("COLOR CHANGED");
        request->send(200, "text/plain", "{\"r\":" + inputR + ", \"g\":" + inputG + ",\"b\":" + inputB + "}");
    });
    server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", updateIndex);
    });
    server.on("/doUpdate", HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
        handleDoUpdate(request, filename, index, data, len, final);}
    );
    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", settingsIndex, settingsVals);
    });
    server.on("/json", HTTP_GET, [] (AsyncWebServerRequest *request) {
        /*StaticJsonDocument<100> data;
        if (ledStatus == "LED on") {
          data["on"] = true;
          data["bri"] = 125;
        }
        else if (ledStatus == "LED off") {
          data["on"] = false;
          data["bri"] = 0;
        }
        */
        //serializeJson(data, response);
        String response = "{\"state\":{\"on\":true,\"bri\":150,\"seg\":[{\"col\":[[255,68,255]]}]},\"info\":{\"ver\":\"0.13.0-b3\",\"leds\":{\"rgbw\":false},\"str\":false,\"name\":\"espLight\",\"wifi\":{\"bssid\":\"8C:3B:AD:BA:15:AA\",\"rssi\":-69,\"signal\":62,\"channel\":8},\"ip\":\"10.0.0.90\"}}"; 
        request->send(200, "application/json", response);
    });
    server.on("/win", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String response;
        if (ledStatus == "LED on") {
          response = "<vs><ac>125</ac><cl>255</cl><cl>68</cl><cl>255</cl><cs>0</cs><cs>0</cs><cs>0</cs><ns>0</ns><nr>1</nr><nl>0</nl><nf>1</nf><nd>60</nd><nt>0</nt><sq>20</sq><gn>10</gn><fx>66</fx><sx>37</sx><ix>62</ix><f1>0</f1><f2>0</f2><f3>0</f3><fp>35</fp><wv>-1</wv><ws>0</ws><ps>0</ps><cy>0</cy><ds>espLight</ds><ss>0</ss></vs>";
        }
        else if (ledStatus == "LED off") {
          response = "<vs><ac>0</ac><cl>255</cl><cl>68</cl><cl>255</cl><cs>0</cs><cs>0</cs><cs>0</cs><ns>0</ns><nr>1</nr><nl>0</nl><nf>1</nf><nd>60</nd><nt>0</nt><sq>20</sq><gn>10</gn><fx>66</fx><sx>37</sx><ix>62</ix><f1>0</f1><f2>0</f2><f3>0</f3><fp>35</fp><wv>-1</wv><ws>0</ws><ps>0</ps><cy>0</cy><ds>espLight</ds><ss>0</ss></vs>";
        }
        request->send(200, "application/xml", response);
    });
    server.begin();
}