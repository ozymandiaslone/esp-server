#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define LED_BUILTIN 4

WebServer server(80); // Create a web server object that listens on port 80
char ssid[] = "STARLINK";      // Your network SSID
int led_state = LOW;

void handleRoot() {
   // Serve a simple HTML page with JavaScript for AJAX
  String html = "<!DOCTYPE html><html><body>";
  html += "<h1>ESP32 Interactive Web Server</h1>";
  html += "<button onclick='toggleLED()'>Toggle LED</button>";
  html += "<p>LED State: <span id='ledState'>OFF</span></p>";
  html += "<script>";
  html += "function toggleLED() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/toggle', true);";
  html += "  xhr.onreadystatechange = function() {";
  html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "      document.getElementById('ledState').innerHTML = xhr.responseText;";
  html += "    }";
  html += "  };";
  html += "  xhr.send();";
  html += "}";
  html += "</script>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleToggleLED() {
  led_state = !led_state;
  digitalWrite(LED_BUILTIN, led_state);
  server.send(200, "text/plain", (led_state == HIGH) ? "ON" : "OFF");
}


void setup() {
  // Initialize onboard LED (if available) as an indicator
  pinMode(LED_BUILTIN, OUTPUT);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);

  // Blink LED while attempting to connect to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  server.on("/", handleRoot);
  server.on("/toggle", handleToggleLED);

  // Start the web server
  server.begin();
}

void loop() {
  server.handleClient(); // Handle incoming client requests
}
