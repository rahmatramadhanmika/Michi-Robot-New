#include "esp_camera.h"
#include <WiFi.h>

#define CAMERA_MODEL_AI_THINKER  // Match your board

#include "camera_pins.h"

const char* ssid = "iPhone 6s";
const char* password = "hihiha123";

void startCameraServer();
void setupLedFlash(int pin);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  
  config.frame_size = FRAMESIZE_CIF;   
  config.pixel_format = PIXFORMAT_JPEG;
  config.jpeg_quality = 20;            
  config.fb_count = 1;                 
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_DRAM; 

  // Camera Initialized
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Failed initializing camera: 0x%x", err);
    ESP.restart();
  }

  sensor_t* s = esp_camera_sensor_get();
  
  if (s->id.PID == OV2640_PID) {
    Serial.println("Configurize sensor OV2640...");
    
    s->set_special_effect(s, 1);     // 1 = Negative effect (alternatif noir)
    
    s->set_exposure_ctrl(s, 0);      // Manual exposure
    s->set_aec2(s, 0);               // Disable AEC2
    s->set_ae_level(s, 0);           // Balanced
    s->set_aec_value(s, 300);
    
    s->set_gain_ctrl(s, 0);          // Manual gain
    s->set_agc_gain(s, 8);
    
    s->set_whitebal(s, 0);           // Disable auto WB
    s->set_awb_gain(s, 0);           // Disable AWB gain
    s->set_wb_mode(s, 1);            // Manual WB mode
    
    s->set_brightness(s, 0);         // Brightness netral
    s->set_contrast(s, 0);           // Contrast netral
    s->set_saturation(s, 0);         // Saturation netral
    s->set_sharpness(s, 0);          // Sharpness netral
    
    s->set_dcw(s, 0);                // Disable downsize
    s->set_bpc(s, 0);                // Disable bad pixel correction
    s->set_wpc(s, 0);                // Disable white pixel correction
    s->set_lenc(s, 0);               // Disable lens correction
    s->set_hmirror(s, 0);            // No mirror
    s->set_vflip(s, 0);              // No flip
    s->set_colorbar(s, 0);           // Disable color bar
    
    Serial.println("Sensor configured with ultra-stabilize mode!");
  }

  s->set_framesize(s, FRAMESIZE_CIF);  // 352x288

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("Connecting to Wifi");
  int timeout = 30;
  while (WiFi.status() != WL_CONNECTED && timeout-- > 0) {
    delay(500);
    Serial.print(".");
    
    yield();
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nConnection Failed. Restarting...");
    ESP.restart();
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  startCameraServer();

  #if defined(LED_GPIO_NUM)
    setupLedFlash(LED_GPIO_NUM);
  #endif
  
  Serial.println("Setup finish - system ready!");
}

void loop() {
  // Feed watchdog and delay
  yield();
  delay(2000);
  
  // Monitor heap memory
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 5000) {
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    lastPrint = millis();
  }
}