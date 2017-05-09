#include <ESP8266WiFi.h>

const char* ssid = "hackafe.org";
const char* password = "";

#define manualOverrideButton 5
#define intercomSignalRelays 13
#define intercomUnlockRelay 15

byte buttonState = HIGH;
boolean isLocked = true;
String request = "";
String accessStatus = "";
String imageStatus = "";
String string1 = "HTTP/1.1 200 OK\n";
String string2 = "Content-Type: text/html\n";
String string3 = "\n"; // do not forget this one
String string4 = "", string5 = "", string6 = "", string7 = "";
String formString = "", buttonsString="", finalString = "";

//18-FE-34-A1-FE-94  esp mac

IPAddress ip(192, 168, 1, 78);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);
WiFiClient client = server.available();

void renderPage(bool isLocked);

void setup() {
  pinMode(intercomSignalRelays, OUTPUT);
  pinMode(intercomUnlockRelay, OUTPUT);
  digitalWrite(intercomSignalRelays, LOW);
  digitalWrite(intercomUnlockRelay, LOW);
  
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  
  server.begin();

  string4 = "<!DOCTYPE html>\n\
<html lang=\"en\">\n\
  <head>\n\
    <title>Hackafe door system</title>\n\
    <meta charset=\"UTF-8\" />\n\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no\" />\n\
    <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\" />\n\
    <link rel=\"stylesheet\" href=\"https://hackafe.github.io/door-system/css/style.css\" />\n\
  </head>\n\
  <body>\n\
    <div class=\"container-fluid\">\n\
      <div class=\"row\">\n\
        <div class=\"col-sm-3\"></div>\n\
        <div class=\"col-sm-2\"></div>\n\
        <div class=\"col-sm-2\">\n\
          <a href=\"http://www.hackafe.org\" title=\"Hackafe website\">\n\
            <img class=\"img-responsive center-block\" src=\"https://hackafe.github.io/door-system/images/hackafe-logo-black-yellow-transparent.png\" draggable=\"false\" />\n\
          </a>\n\
        </div>\n\
        <div class=\"col-sm-2\"></div>\n\
        <div class=\"col-sm-3\"></div>\n\
      </div>\n\
  <div class=\"row\">\n\
        <div class=\"col-sm-3\"></div>\n\
        <div class=\"col-sm-2\"></div>\n\
        <div class=\"col-sm-2\">\n\
          <a href=\"/unlock=true\" class=\"a center-block\">\n\
          ";
          //imageStatus
         string5 = " </a>\n\
        </div>\n\
        <div class=\"col-sm-2\"></div>\n\
        <div class=\"col-sm-3\"></div>\n\
      </div>\n\
<div class=\"row\">\n\
        <div class=\"col-sm-3\"></div>\n\
        <div class=\"col-sm-2\"></div>\n\
        <div class=\"col-sm-2\">\n\
          <h2 class=\"text center-block\">\n\
            <strong>\n\
            ";
            //accessStatus
          string6 = "  </strong>\n\
          </h2>\n\
        </div>\n\
        <div class=\"col-sm-2\"></div>\n\
        <div class=\"col-sm-3\"></div>\n\
      </div>\n\
  ";

  formString = "<div class=\"row\">\n\
      <div class=\"col-sm-3\"></div>\n\
      <div class=\"col-sm-2\"></div>\n\
      <div class=\"col-sm-2\">\n\
        <form action=\"/unlock=true\" method=\"get\" class=\"form  center-block\" id=\"form\">\n\
          <div class=\"form-group\">\n\
            <input type=\"password\" class=\"form-control\" id=\"password\" placeholder=\"Password\">\n\
          </div>\n\
        </form>\n\
        <a href=\"/unlock=true\" class=\"btn btn-lg btn-purple center-block\" role=\"button\" onclick=\"form.submit();\"><span class=\"glyphicon glyphicon-lock\"></span>&nbsp;Unlock</a>\n\
      </div>\n\
      <div class=\"col-sm-2\"></div>\n\
      <div class=\"col-sm-3\"></div>\n\
    </div>\n\
    <br />\n\
  ";

  buttonsString = "<div class=\"row\">\n\
      <div class=\"col-sm-3\"></div>\n\
      <div class=\"col-sm-2\"></div>\n\
      <div class=\"col-sm-2\">\n\
        <a href=\"http://192.168.1.78\" class=\"btn btn-lg btn-green center-block\" role=\"button\">Back</a>\n\
      </div>\n\
      <div class=\"col-sm-2\"></div>\n\
      <div class=\"col-sm-3\"></div>\n\
    </div>\n\
    <br />\n\
  ";
  
  string7 = "<div class=\"row\">\n\
          <div class=\"col-sm-3\"></div>\n\
          <div class=\"col-sm-2\"></div>\n\
          <div class=\"col-sm-2\">\n\
            <a href=\"\" class=\"a center-block\">Copyright &copy; 2017 Hackafe.</a>\n\
          </div>\n\
          <div class=\"col-sm-2\"></div>\n\
          <div class=\"col-sm-3\"></div>\n\
        </div>\n\
      </div>\n\
    </body>\n\
  </html>\n\
  ";
}

void loop() {
  
  if (digitalRead(manualOverrideButton) == LOW) {
    buttonState = LOW;
    digitalWrite(intercomSignalRelays, HIGH);
  } else {
    buttonState = HIGH;
    digitalWrite(intercomSignalRelays, LOW);
  }
  
  client = server.available();
  
  if (!client) {
    return;
  }
  
  request = client.readStringUntil('\r');
  client.flush();
  
  if (request.indexOf("/unlock=true") != -1) {
    isLocked = false;

    renderPage(isLocked);
    
    if (buttonState == LOW) {
      digitalWrite(intercomSignalRelays, LOW);
    } else {
      digitalWrite(intercomSignalRelays, HIGH);
    }
    delay(600);
    
    digitalWrite(intercomUnlockRelay, HIGH);
    delay(600);
    
    digitalWrite(intercomUnlockRelay, LOW);
    delay(600);
    
    if (buttonState == LOW) {
      digitalWrite(intercomSignalRelays, HIGH);
    } else {
      digitalWrite(intercomSignalRelays, LOW);
    }
    
    delay(600);
    
  }

  if (isLocked) {
    renderPage(isLocked);
  }
  isLocked = true;
  client.stop();
}

void renderPage(bool isLocked) {
  switch (isLocked) {
    case 0:
      accessStatus = "Access Granted!";
      imageStatus = "<img class=\"img-responsive center-block\" src=\"https://hackafe.github.io/door-system/images/unlocked.png\" draggable=\"false\" />";
      break;
    case 1:
      imageStatus = "<img class=\"img-responsive center-block\" src=\"https://hackafe.github.io/door-system/images/locked.png\" draggable=\"false\" />";
      break;
    default:; 
  }        
  
  if (isLocked) {
    finalString = string4 + imageStatus + string5 + string6 + formString + string7;
  } else {
    finalString = string4 + imageStatus + string5 + accessStatus + string6 + buttonsString + string7;
  }
  client.println(finalString);
}
