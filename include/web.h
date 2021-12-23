#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <index.h>
#include <update.h>
#include <rgb.h>

AsyncWebServer server(80);
String ledStatus = "LED off";

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
    // check file names for type
    int cmd = (filename.indexOf(F(".spiffs.bin")) > -1 ) ? U_SPIFFS : U_FLASH;
    if (cmd == U_FLASH && !(filename.indexOf(F(".bin")) > -1) ) return; // wrong image for ESP32
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) == len) {
    Serial.println(Update.progress() / 10000 );
  } else {
    Update.printError(Serial);
  }

  if (final) {    
    if (!Update.end(true)){
      Update.printError(Serial);
    } else {
      String response_message;
      response_message.reserve(1000);
      response_message += "<h1>Update done!</h1>";
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", response_message);
      response->addHeader("Refresh", "20");  
      response->addHeader("Location", "/");
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
    server.begin();
}