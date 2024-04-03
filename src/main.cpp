#include <WiFi.h>
#include <Servo.h>
#include <EEPROM.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "Adfnet";                   // "HMIGripper";
const char *password = "33394151923058759658"; // "123456789";

const int servoHertz = 2000;
const int minUs = 1000;
const int maxUs = 2000;

Servo myservo;
int minPos = 0; // variable to store the servo position
int maxPos = 180; // variable to store the servo position

// EEPROM address where the data is stored
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
int servoPin = 13;

AsyncWebServer server(80);

const char HTML[] PROGMEM = "<!DOCTYPE html>\n"
"<html>\n"
"\n"
"<head>\n"
"  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
"  <link rel=\"icon\" href=\"data:,\">\n"
"  <style>\n"
"    body {\n"
"      text-align: center;\n"
"      font-family: \"Trebuchet MS\", Arial;\n"
"      margin-left: auto;\n"
"      margin-right: auto;\n"
"    }\n"
"\n"
"    .slider {\n"
"      width: 300px;\n"
"    }\n"
"  </style>\n"
"  <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>\n"
"</head>\n"
"\n"
"<body>\n"
"  <h1>Setup HMI Grabber Position</h1>\n"
"  <p>Position: <span id=\"servoPos\"></span></p>\n"
"  <p><span id=\"result\"></span></p>\n"
"  <input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" />\n"
"  <button>Set Min Position</button>\n"
"  <button>Set max Position</button>\n"
"  <button>Grab !</button>\n"
"  <script>\n"
"    var slider = document.getElementById(\"servoSlider\");\n"
"    var servoP = document.getElementById(\"servoPos\");\n"
"    servoP.innerHTML = slider.value;\n"
"    slider.oninput = function () {\n"
"      slider.value = this.value;\n"
"      servoP.innerHTML = this.value;\n"
"    }\n"
"    $.ajaxSetup({ timeout: 1000 });\n"
"\n"
"    function servo(pos) {\n"
"      $.post(`/set?position=${pos}`, ).done(res => {\n"
"        console.log(res);\n"
"      }) \n"
"    }\n"
"\n"
"    function setMinimun() {\n"
"      const val = $(\"#servoSlider\").val;\n"
"      $.post(`/set?min=${val}`).done(res => {\n"
"        $('#result').text(\"Minimal Position set\");\n"
"      }) \n"
"    }\n"
"\n"
"    function setMaximum() {\n"
"      const val = $(\"#servoSlider\").val;\n"
"      $.post(`/set?max=${val}`).done(res => {\n"
"        $('#result').text(\"Maximal Position set\");\n"
"      }) \n"
"    }\n"
"\n"
"    function grab() {\n"
"      $.post(`/grab`).done(res => {\n"
"        console.log(res) \n"
"      }) \n"
"    }\n"
"\n"
"  </script>\n"
"</body>\n"
"\n"
"</html>";


void setup()
{
  EEPROM.begin(8);

  Serial.begin(9600);
  // WiFi.softAP(ssid, password);
  // WiFi.softAPIP();
  // Serial.print("AP IP Adress ");
  // Serial.println(WiFi.softAPIP());
  WiFi.begin(ssid, password);
  EEPROM.get(0, minPos);
  EEPROM.get(4, maxPos);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("Connected to WiFi with IP: ");
  Serial.println(WiFi.localIP());

  myservo.attach(servoPin);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", HTML); });

  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if(request->hasParam("position)")) {
                  myservo.write(request->getParam("position")->value().toInt());
              }
              if(request->hasParam("min)")) {
                int reqValue = request-> getParam("min")->value().toInt();
                minPos = reqValue;
                EEPROM.put(0, reqValue);
              }
              if(request->hasParam("max)")) {
                int reqValue = request-> getParam("min")->value().toInt();
                maxPos = reqValue;
                EEPROM.put(4, reqValue);
              }

            });
  server.on("/grab", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              myservo.write(180);
              request->send(200, "text/plain", "Grabbing");
            });
}

void loop()
{
  //delay(1000);
}