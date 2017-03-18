#include <ESP8266WiFi.h>

const char* ssid = "hackafe.org";
const char* password = "";

#define manualOverrideButton 5
#define intercomSignalRelays 13
#define intercomUnlockRelay 15

byte buttonState = HIGH;
boolean isLocked = true;
String request = "";

unsigned long previousMillis = 0;

unsigned long currentMillis = 0;

const unsigned short interval = 500;

//18-FE-34-A1-FE-94  esp mac

IPAddress ip(192, 168, 1, 78);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);
WiFiClient client = server.available();

void setup() {
  pinMode(intercomSignalRelays, OUTPUT);
  pinMode(intercomUnlockRelay, OUTPUT);
  digitalWrite(intercomSignalRelays, LOW);
  digitalWrite(intercomUnlockRelay, LOW);
  
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  
  server.begin();
}


void generateTopHTML() {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE html>\
<html lang=\"en\">\
  <head>\
    <title>Hackafe door system</title>\
    <meta charset=\"UTF-8\" />\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no\" />\
    <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\" />\
    <link rel=\"stylesheet\" href=\"https://hackafe.github.io/door-system/css/style.css\" />\
  </head>\
  <body>\
    <div class=\"container-fluid\">\
      <div class=\"row\">\
        <div class=\"col-sm-3\"></div>\
        <div class=\"col-sm-2\"></div>\
        <div class=\"col-sm-2\">\
          <a href=\"http://www.hackafe.org\" title=\"Hackafe website\">\
            <img class=\"img-responsive center-block\" src=\"https://hackafe.github.io/door-system/images/hackafe-logo-black-yellow-transparent.png\" draggable=\"false\" />\
          </a>\
        </div>\
        <div class=\"col-sm-2\"></div>\
        <div class=\"col-sm-3\"></div>\
      </div>\
  ");
}

void generateImageStatusHTML(int choice) {
  client.println("<div class=\"row\">\
        <div class=\"col-sm-3\"></div>\
        <div class=\"col-sm-2\"></div>\
        <div class=\"col-sm-2\">\
          <a href=\"/unlock=true\" class=\"a center-block\">\
  ");
  switch (choice) {
    case 1:
    client.println("<img class=\"img-responsive center-block\" src=\"https://hackafe.github.io/door-system/images/unlocked.png\" draggable=\"false\" />");
    break;
    case 0:
    client.println("<img class=\"img-responsive center-block\" src=\"https://hackafe.github.io/door-system/images/locked.png\" draggable=\"false\" />");
    break;
    default: return;   
  }        
 client.println("</a>\
        </div>\
        <div class=\"col-sm-2\"></div>\
        <div class=\"col-sm-3\"></div>\
      </div>\
  ");
}

void generateAccessStatusHTML(int choice) {
  client.println("<div class=\"row\">\
        <div class=\"col-sm-3\"></div>\
        <div class=\"col-sm-2\"></div>\
        <div class=\"col-sm-2\">\
          <h2 class=\"text center-block\">\
            <strong>\
 ");
 switch (choice) {
    case 1:
    client.println("Access Granted!");
    break;
    case 0:
    client.println("Access Denied!");
    break;
    default: return;   
  }        
 client.println("</strong>\
          </h2>\
        </div>\
        <div class=\"col-sm-2\"></div>\
        <div class=\"col-sm-3\"></div>\
      </div>\
  ");
}

void generatePasswordFormHTML() {
  client.println("<div class=\"row\">\
      <div class=\"col-sm-3\"></div>\
      <div class=\"col-sm-2\"></div>\
      <div class=\"col-sm-2\">\
        <form action=\"/unlock=true\" method=\"get\" class=\"form  center-block\" id=\"form\">\
          <div class=\"form-group\">\
            <input type=\"password\" class=\"form-control\" id=\"password\" placeholder=\"Password\">\
          </div>\
        </form>\
      </div>\
      <div class=\"col-sm-2\"></div>\
      <div class=\"col-sm-3\"></div>\
    </div>\
    <div class=\"row\">\
      <div class=\"col-sm-3\"></div>\
      <div class=\"col-sm-2\"></div>\
      <div class=\"col-sm-2\">\
        <a href=\"/unlock=true\" class=\"btn btn-lg btn-green center-block\" role=\"button\" onclick=\"form.submit();\"><span class=\"glyphicon glyphicon-lock\"></span>&nbsp;Unlock</a>\
      </div>\
      <div class=\"col-sm-2\"></div>\
      <div class=\"col-sm-3\"></div>\
    </div><br />\
  ");
}

void generateBottomHTML() {  
  client.println("<div class=\"row\">\
          <div class=\"col-sm-3\"></div>\
          <div class=\"col-sm-2\"></div>\
          <div class=\"col-sm-2\">\
            <a href=\"\" class=\"a center-block\">Copyright &copy; 2017 Hackafe.</a>\
          </div>\
          <div class=\"col-sm-2\"></div>\
          <div class=\"col-sm-3\"></div>\
        </div>\
      </div>\
    </body>\
  </html>\
  ");
}

void loop() {

  currentMillis = millis();

  
  if (digitalRead(manualOverrideButton) == LOW) {
    buttonState = LOW;
    digitalWrite(intercomSignalRelays, HIGH);
  } else {
    buttonState = HIGH;
    digitalWrite(intercomSignalRelays, LOW);
  }



  
  // Check if a client has connected
  client = server.available();
  
  if (!client) {
    return;
  }



  request = client.readStringUntil('\r');
  client.flush();


   if (request.indexOf("/unlock=true") != -1) {
   isLocked = false;
   generateTopHTML();
   generateImageStatusHTML(1);
   generateAccessStatusHTML(1);
   generateBottomHTML();
  

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
  

// Return the response
  //client.println("HTTP/1.1 200 OK");
  //client.println("Content-Type: text/html");
  //client.println(""); //  do not forget this one
if (isLocked) {
   generateTopHTML();
   generateImageStatusHTML(0);
   generatePasswordFormHTML();
   //generateAccessStatusHTML("Access Granted!");
   generateBottomHTML();
   //you shall not pass :D
   isLocked = true;
}
   //client.println("<!DOCTYPE html>\
   // <html>\
    // <head>\
     // </head>\
     // <body>\
     // <center>\
     // <h1>Click <a href=\"/unlock=true\">here</a> to unlock the door</h1>\
      //</center>\
    //  </body>\
   // </html>\
 // ");

  
  client.stop();
} 
