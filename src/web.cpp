#include <Arduino.h>
#include <AsyncJson.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <pages.h>
#include <build_ver.h>
#include <rgb.h>
#include <web.h>
#include <settings/pinSettings.h>
#include <settings/wifiSettings.h>

AsyncWebServer server(80);
String ledStatus = "LED off";
bool isOn = false;

String liveVals(const String& var){
  if(var == "USERINPUT"){
    String interactive = "";
    interactive += "<label>RED</label><br><input type=\"range\" onchange=\"sendColor()\" oninput=\"updateColor()\" value=" + String(getCurrColor('R')) + " min=0 max=255 id=\"colorInputR\"></input><br>";
    interactive += "<label>GREEN</label><br><input type=\"range\" onchange=\"sendColor()\" oninput=\"updateColor()\" value=" + String(getCurrColor('G')) + " min=0 max=255 id=\"colorInputG\"></input><br>";
    interactive += "<label>BLUE</label><br><input type=\"range\" onchange=\"sendColor()\" oninput=\"updateColor()\" value=" + String(getCurrColor('B')) + " min=0 max=255 id=\"colorInputB\"></input><br>";
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
    if (isOn == true) {
      turnOff();
      ledStatus = "LED off";
      isOn = false;
    } 
    else {
      turnOn();
      ledStatus = "LED on";
      isOn = true;
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
        writeToStrip(inputR.toInt(), inputG.toInt(), inputB.toInt(), isOn);
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
    server.on("/win", HTTP_GET, [] (AsyncWebServerRequest *request) {
      String inputR;
      if (request->hasParam("T")) {
        inputR = request->getParam("T")->value();
        if (inputR == "0") {
          turnOff();
          isOn = false;
        } else if (inputR == "1") {
          turnOn();
          isOn = true;
        }
      }
      String response;
      if (isOn == true) {
        response = "<vs><ac>125</ac><cl>" + String(getCurrColor('R')) + "</cl><cl>" + String(getCurrColor('G')) + "</cl><cl>" + String(getCurrColor('B')) + "</cl><ds>" + String(WiFi.getHostname()) + "</ds><ss>0</ss></vs>";
      }
      else {
        response = "<vs><ac>0</ac><cl>" + String(getCurrColor('R')) + "</cl><cl>" + String(getCurrColor('G')) + "</cl><cl>" + String(getCurrColor('B')) + "</cl><ds>" + String(WiFi.getHostname()) + "</ds><ss>0</ss></vs>";
      }
      request->send(200, "application/xml", response);
    });
    server.on("/win&T=2", HTTP_GET, [] (AsyncWebServerRequest *request) {
      String response;
      Serial.println("HTTP_GET power toggle!");
      handleLed();
      if (isOn == true) {
        response = "<vs><ac>125</ac><cl>" + String(getCurrColor('R')) + "</cl><cl>" + String(getCurrColor('G')) + "</cl><cl>" + String(getCurrColor('B')) + "</cl><ds>" + String(WiFi.getHostname()) + "</ds><ss>0</ss></vs>";
      }
      else {
        response = "<vs><ac>0</ac><cl>" + String(getCurrColor('R')) + "</cl><cl>" + String(getCurrColor('G')) + "</cl><cl>" + String(getCurrColor('B')) + "</cl><ds>" + String(WiFi.getHostname()) + "</ds><ss>0</ss></vs>";
      }
      request->send(200, "application/xml", response);
    });
    server.on("/json", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("JSON base fetched!");
        String response;
        response += "{\"state\":{";
        response += "\"on\":" + String((isOn ? "true" : "false"));
        response += ",\"bri\":125,\"seg\":[{\"col\":[";
        response += "[" + String(getCurrColor('R')) + "," + String(getCurrColor('G')) + "," + String(getCurrColor('B')) + "]";
        response += "]}]},\"info\":{\"ver\":\"" + build_ver + "\",\"leds\":{\"rgbw\":false},\"str\":false,";
        response += "\"name\":\"" + String(WiFi.getHostname()) + "\",\"wifi\":{\"bssid\":\"" + String(WiFi.BSSIDstr()) + "\",\"rssi\":" + String(WiFi.RSSI()) + ",\"signal\":" + map(WiFi.RSSI(), -90, -30, 0, 100) + ",\"channel\":" + String(WiFi.channel()) + "},\"ip\":\"10.0.0.90\"}}"; 
        request->send(200, "application/json", response);
    });
    AsyncCallbackJsonWebHandler *handlerBase = new AsyncCallbackJsonWebHandler("/json/state", [](AsyncWebServerRequest *request, JsonVariant &json) {
      Serial.println("JSON state triggered!");
      StaticJsonDocument<200> data;
      data = json.as<JsonObject>();
      String response;
      String isOnBool = data["on"];
      if (isOnBool == "t") {
        Serial.println("JSON power toggle!");
        handleLed();
        response += "{\"on\":" + String((isOn ? "true" : "false"));
        response += ",\"bri\":125,\"seg\":[{\"col\":[";
        response += "[" + String(getCurrColor('R')) + "," + String(getCurrColor('G')) + "," + String(getCurrColor('B')) + "]";
        response += "]}]},\"info\":{\"ver\":\"" + build_ver + "\",\"leds\":{\"rgbw\":false},\"str\":false,";
        response += "\"name\":\"" + String(WiFi.getHostname()) + "\",\"wifi\":{\"bssid\":\"" + String(WiFi.BSSIDstr()) + "\",\"rssi\":" + String(WiFi.RSSI()) + ",\"signal\":" + map(WiFi.RSSI(), -90, -30, 0, 100) + ",\"channel\":" + String(WiFi.channel()) + "},\"ip\":\"10.0.0.90\"}}"; 
      }
      request->send(200, "application/json", response);
    });
    server.addHandler(handlerBase);
    AsyncCallbackJsonWebHandler *handlerState = new AsyncCallbackJsonWebHandler("/json", [](AsyncWebServerRequest *request, JsonVariant &json) {
      Serial.println("JSON base triggered!");
      StaticJsonDocument<200> data;
      data = json.as<JsonObject>();
      String response;
      String isOnBool = data["on"];
      if (isOnBool == "t") {
        Serial.println("JSON Turn-Off triggered!");
        handleLed();
        response += "{\"state\":{";
        response += "\"on\":" + String((isOn ? "true" : "false"));
        response += ",\"bri\":125,\"seg\":[{\"col\":[";
        response += "[" + String(getCurrColor('R')) + "," + String(getCurrColor('G')) + "," + String(getCurrColor('B')) + "]";
        response += "]}]},\"info\":{\"ver\":\"" + build_ver + "\",\"leds\":{\"rgbw\":false},\"str\":false,";
        response += "\"name\":\"" + String(WiFi.getHostname()) + "\",\"wifi\":{\"bssid\":\"" + String(WiFi.BSSIDstr()) + "\",\"rssi\":" + String(WiFi.RSSI()) + ",\"signal\":" + map(WiFi.RSSI(), -90, -30, 0, 100) + ",\"channel\":" + String(WiFi.channel()) + "},\"ip\":\"10.0.0.90\"}}"; 
      }
      request->send(200, "application/json", response);
      Serial.println(isOnBool);
    });
    server.addHandler(handlerState);
    server.begin();
}