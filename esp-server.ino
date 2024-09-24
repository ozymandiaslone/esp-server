#include <WiFi.h>
#include <WiFiClient.h>
#include <esp_camera.h>
#include <WebServer.h>
#include "index_html.h"

#define LED_BUILTIN 4

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

WebServer server(80); 
char ssid[] = "STARLINK";      
int led_state = LOW;

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleToggleLED() {
  led_state = !led_state;
  digitalWrite(LED_BUILTIN, led_state);
  server.send(200, "text/plain", (led_state == HIGH) ? "ON" : "OFF");
}

void handleCapture() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  camera_fb_t *fb = esp_camera_fb_get();
  if (fb) {
    esp_camera_fb_return(fb);
  }
  delay(10);
  fb = esp_camera_fb_get();
  digitalWrite(LED_BUILTIN, LOW);
  if (!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }
  server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
  server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);

}


void setup() {
  // Initialize onboard LED (if available) as an indicator
  pinMode(LED_BUILTIN, OUTPUT);
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;
 

  esp_err_t err = esp_camera_init(&config);
  while (err != ESP_OK) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(2);
    digitalWrite(LED_BUILTIN, LOW);
    err = esp_camera_init(&config);
    delay(100);
  }
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
  server.on("/capture", handleCapture);

  // Start the web server
  server.begin();
}

void loop() {
  server.handleClient(); // Handle incoming client requests
}
