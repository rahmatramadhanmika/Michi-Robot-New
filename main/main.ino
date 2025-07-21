#include <Arduino.h>
#include <driver/i2s_std.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>
#include <SPI.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Audio.h>
#include "action.h"

const uint32_t SAMPLE_RATE = 16000;
const uint16_t SAMPLE_BITS = 16;
const uint8_t CHANNELS = 1;
const uint32_t RECORD_TIME_SECONDS = 10;
const char *FILENAME = "/buffer/record.wav";
const char *MUSIC_FOLDER = "/cut";
const char *HAPPY_FOLDER = "/happy";
const char *MAD_FOLDER = "/mad";
const char *SAD_FOLDER = "/sad";
const char *GREETINGS_FOLDER = "/greetings";
const char *DETECTING_FOLDER = "/detecting";
const char *HEARING_FOLDER = "/hearing";

#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 32
#define I2S_PORT_NUM I2S_NUM_0

#define I2S_DOUT 18
#define I2S_BCLK 5
#define I2S_LRC 17
#define I2S_PORT_NUM_PLAY I2S_NUM_1

#define SD_CS 12
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 13

#define VAD_THRESHOLD 1000
#define WAKE_WORD_DETECTION_TIME 3000
#define COOLDOWN_TIME 3000
#define SILENCE_THRESHOLD 1000
#define SILENCE_DURATION 3000
#define LED_PIN 2

const char *ssid = "iPhone 6s";
const char *password = "hihiha123";
const char *serverUrl = "http://172.20.10.2:5000/process_input";
const char *wakeWordUrl = "http://172.20.10.2:5000/detect_wakeword";
const char *streamURL = "http://172.20.10.2:5000/audio_response";

char *MQTT_SERVER = "broker.emqx.io";
int MQTT_PORT = 1883;

enum State {
  IDLE,
  DETECTING_WAKE_WORD,
  TRANSITION,
  RECORDING_CONVERSATION,
  PLAYING_RESPONSE,
  SLEEP,
  MAD,
  SAD,
  HAPPY,
  DANCE,
  GREETINGS,
};

unsigned long lastSoundTime = 0;
bool isSpeaking = false;
bool audioFinished = false;
State currentState = IDLE;
unsigned long stateStartTime = 0;
unsigned long lastActionTime = 0;
unsigned long lastAudioTime = 0;
File recordFile;
uint32_t totalBytesWritten = 0;
const size_t bufferSize = 1024;
uint8_t buffer[bufferSize];
String musicFiles[50];
int musicFileCount = 0;
String happyFiles[50];
int happyFileCount = 0;
String madFiles[50];
int madFileCount = 0;
String sadFiles[50];
int sadFileCount = 0;
String greetingsFiles[50];
int greetingsFileCount = 0;
String detectingFiles[50];
int detectingFileCount = 0;
String hearingFiles[50];
int hearingFileCount = 0;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
Audio audio;
i2s_chan_handle_t rx_handle = NULL;
Action robot;

void initConnection() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
  Serial.println("\nWiFi Connected: " + WiFi.localIP().toString());
  digitalWrite(LED_PIN, LOW);
}

void connectToBroker(String clientName) {
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(callback);
  Serial.println("Connecting to MQTT Broker");
  String clientId = "ESP32Client-" + clientName;
  mqttClient.connect(clientId.c_str());
  while (!mqttClient.connected()) {
    Serial.print("Failed: ");
    Serial.println(mqttClient.state());
    Serial.println("Attempting to reconnect");
    delay(500);
  }
  Serial.print("Connected to ");
  Serial.println(clientName);
  if (mqttClient.connected()) {
    doSubscribe();
  }
}

void doPublish(String topic, String payload) {
  mqttClient.publish(topic.c_str(), payload.c_str());
  Serial.print(topic);
  Serial.print(" ==> ");
  Serial.println(payload);
}

void doSubscribe() {
  mqttClient.subscribe("testtopic/mwtt");
}

void loadAudioFiles(const char *folder, String *fileArray, int &fileCount) {
  File dir = SD.open(folder);
  if (!dir) {
    Serial.printf("Failed to open %s directory\n", folder);
    return;
  }
  fileCount = 0;
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    if (!entry.isDirectory()) {
      String fileName = entry.name();
      if (fileName.endsWith(".mp3") || fileName.endsWith(".wav")) {
        fileArray[fileCount] = String(folder) + "/" + fileName;
        fileCount++;
        if (fileCount >= 50) break;
      }
    }
    entry.close();
  }
  dir.close();
  Serial.printf("Found %d audio files in %s\n", fileCount, folder);
}

void playRandomAudio(String *fileArray, int fileCount, const char *stateName) {
  if (fileCount == 0) {
    Serial.printf("No audio files found in %s folder\n", stateName);
    currentState = IDLE;
    digitalWrite(LED_PIN, LOW);
    lastActionTime = millis();
    robot.idle();
    return;
  }
  int randomIndex = random(fileCount);
  String filePath = fileArray[randomIndex];
  Serial.printf("Playing %s audio: %s\n", stateName, filePath.c_str());
  audio.stopSong();
  audioFinished = false;
  audio.connecttoFS(SD, filePath.c_str());
  unsigned long audioTimeout = millis() + 10000;
  while (!audio.isRunning() && millis() < audioTimeout) {
    delay(100);
    audio.loop();
  }
  if (!audio.isRunning()) {
    Serial.printf("Failed to play %s audio\n", stateName);
    currentState = IDLE;
    digitalWrite(LED_PIN, LOW);
    lastActionTime = millis();
    robot.idle();
  }
}

void callback(char *topic, byte *message, unsigned int length) {
  Serial.print("Topic: ");
  Serial.print(topic);
  Serial.print(" Message: ");
  String msgTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    msgTemp += (char)message[i];
  }
  Serial.println();
  if (String(topic) == "testtopic/mwtt") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msgTemp);
    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
      return;
    }
    if (doc.containsKey("response")) {
      String response = doc["response"].as<String>();
      if (response == "talk") {
        Serial.println("Received 'talk' command");
        if (currentState == RECORDING_CONVERSATION) {
          recordFile.seek(0);
          writeWavHeader(recordFile, totalBytesWritten);
          recordFile.close();
          Serial.println("Conversation finish. Uploading...");
          robot.thinking(); // Set thinking animation during upload
          String response = uploadToServer(FILENAME, serverUrl);
          if (!response.isEmpty()) {
            currentState = PLAYING_RESPONSE;
            stateStartTime = millis();
            digitalWrite(LED_PIN, HIGH);
            robot.answer();
            audio.stopSong();  // Stop any ongoing audio
            audio.connecttohost(streamURL);
          } else {
            currentState = IDLE;
            digitalWrite(LED_PIN, LOW);
            lastActionTime = millis();
            robot.idle();
          }
          isSpeaking = false;
        }
      } else if (response == "sleep") {
        Serial.println("Received 'sleep' command");
        currentState = SLEEP;
        stateStartTime = millis();
        digitalWrite(LED_PIN, LOW);
        audio.stopSong();  // Stop any ongoing audio
        robot.standBy();
      } else if (response == "mad" && currentState != SLEEP) {
        Serial.println("Received 'mad' command");
        currentState = MAD;
        stateStartTime = millis();
        digitalWrite(LED_PIN, HIGH);
        robot.mad();
        audio.stopSong();  // Stop any ongoing audio
        playRandomAudio(madFiles, madFileCount, "mad");
      } else if (response == "sad" && currentState != SLEEP) {
        Serial.println("Received 'sad' command");
        currentState = SAD;
        stateStartTime = millis();
        digitalWrite(LED_PIN, HIGH);
        robot.sad();
        audio.stopSong();  // Stop any ongoing audio
        playRandomAudio(sadFiles, sadFileCount, "sad");
      } else if (response == "happy" && currentState != SLEEP) {
        Serial.println("Received 'happy' command");
        currentState = HAPPY;
        stateStartTime = millis();
        digitalWrite(LED_PIN, HIGH);
        robot.happy();
        audio.stopSong();  // Stop any ongoing audio
        playRandomAudio(happyFiles, happyFileCount, "happy");
      } else if (response == "dance" && currentState != SLEEP) {
        Serial.println("Received 'dance' command");
        currentState = DANCE;
        stateStartTime = millis();
        digitalWrite(LED_PIN, HIGH);
        robot.dance();
        audio.stopSong();  // Stop any ongoing audio
        playRandomAudio(musicFiles, musicFileCount, "dance");
      } else if (response == "greetings" && currentState != SLEEP) {
        Serial.println("Received 'greetings' command");
        currentState = GREETINGS;
        stateStartTime = millis();
        digitalWrite(LED_PIN, HIGH);
        robot.happy();
        audio.stopSong();  // Stop any ongoing audio
        playRandomAudio(greetingsFiles, greetingsFileCount, "greetings");
      }
    }
  }
}

void setupI2S() {
  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_PORT_NUM, I2S_ROLE_MASTER);
  ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
  i2s_data_bit_width_t data_bit_width = I2S_DATA_BIT_WIDTH_16BIT;
  i2s_slot_bit_width_t slot_bit_width = I2S_SLOT_BIT_WIDTH_16BIT;
  i2s_slot_mode_t slot_mode = I2S_SLOT_MODE_MONO;
  i2s_std_config_t std_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
    .slot_cfg = {
      .data_bit_width = data_bit_width,
      .slot_bit_width = slot_bit_width,
      .slot_mode = slot_mode,
      .slot_mask = I2S_STD_SLOT_LEFT,
      .ws_width = slot_bit_width,
      .ws_pol = false,
      .bit_shift = true,
      .msb_right = true,
    },
    .gpio_cfg = {
      .mclk = GPIO_NUM_NC,
      .bclk = static_cast<gpio_num_t>(I2S_SCK),
      .ws = static_cast<gpio_num_t>(I2S_WS),
      .dout = GPIO_NUM_NC,
      .din = static_cast<gpio_num_t>(I2S_SD),
      .invert_flags = {
        .mclk_inv = false,
        .bclk_inv = false,
        .ws_inv = false,
      },
    },
  };
  ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
  ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));
}

void writeWavHeader(File file, uint32_t dataSize) {
  uint32_t chunkSize = 36 + dataSize;
  uint16_t audioFormat = 1;
  uint16_t numChannels = CHANNELS;
  uint32_t byteRate = SAMPLE_RATE * CHANNELS * SAMPLE_BITS / 8;
  uint16_t blockAlign = CHANNELS * SAMPLE_BITS / 8;
  uint16_t bitsPerSample = SAMPLE_BITS;
  file.write((const uint8_t *)"RIFF", 4);
  file.write((uint8_t *)&chunkSize, 4);
  file.write((const uint8_t *)"WAVE", 4);
  file.write((const uint8_t *)"fmt ", 4);
  uint32_t subChunk1Size = 16;
  file.write((uint8_t *)&subChunk1Size, 4);
  file.write((uint8_t *)&audioFormat, 2);
  file.write((uint8_t *)&numChannels, 2);
  file.write((uint8_t *)&SAMPLE_RATE, 4);
  file.write((uint8_t *)&byteRate, 4);
  file.write((uint8_t *)&blockAlign, 2);
  file.write((uint8_t *)&bitsPerSample, 2);
  file.write((const uint8_t *)"data", 4);
  file.write((uint8_t *)&dataSize, 4);
}

String uploadToServer(const char *filepath, const char *endpoint) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi not connected.");
    return "";
  }
  File file = SD.open(filepath, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for upload");
    return "";
  }
  WiFiClient client;
  HTTPClient http;
  http.begin(client, endpoint);
  http.addHeader("Content-Type", "audio/wav");
  http.setTimeout(15000);
  int httpResponseCode = http.sendRequest("POST", &file, file.size());
  String response = "";
  if (httpResponseCode > 0) {
    Serial.printf("Upload successful! HTTP %d\n", httpResponseCode);
    response = http.getString();
    Serial.println(response);
  } else {
    Serial.printf("Upload failed. Error %d: %s\n", httpResponseCode, http.errorToString(httpResponseCode).c_str());
  }
  http.end();
  file.close();
  return response;
}

bool connectToAudioStream() {
  Serial.printf("\nConnecting to audio stream: %s\n", streamURL);
  audio.stopSong();
  audioFinished = false;
  audio.connecttohost(streamURL);
  unsigned long audioTimeout = millis() + 10000;
  while (!audio.isRunning() && millis() < audioTimeout) {
    delay(100);
    audio.loop();
  }
  if (!audio.isRunning()) {
    Serial.println("Failed to start audio stream!");
  } else {
    Serial.println("Audio stream started successfully!");
  }
  return audio.isRunning();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialize Action
  robot.begin();

  // Initialize SD Card
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  if (!SD.begin(SD_CS)) {
    Serial.println("Fail to initialized SD card");
    return;
  }
  Serial.println("SD card initialized successfuly");
  if (!SD.exists("/buffer")) {
    SD.mkdir("/buffer");
  }
  if (!SD.exists(MUSIC_FOLDER)) {
    SD.mkdir(MUSIC_FOLDER);
  }
  if (!SD.exists(HAPPY_FOLDER)) {
    SD.mkdir(HAPPY_FOLDER);
  }
  if (!SD.exists(MAD_FOLDER)) {
    SD.mkdir(MAD_FOLDER);
  }
  if (!SD.exists(SAD_FOLDER)) {
    SD.mkdir(SAD_FOLDER);
  }
  if (!SD.exists(GREETINGS_FOLDER)) {
    SD.mkdir(GREETINGS_FOLDER);
  }
  if (!SD.exists(DETECTING_FOLDER)) {
    SD.mkdir(DETECTING_FOLDER);
  }
  if (!SD.exists(HEARING_FOLDER)) {
    SD.mkdir(HEARING_FOLDER);
  }

  // Load audio files
  loadAudioFiles(MUSIC_FOLDER, musicFiles, musicFileCount);
  loadAudioFiles(HAPPY_FOLDER, happyFiles, happyFileCount);
  loadAudioFiles(MAD_FOLDER, madFiles, madFileCount);
  loadAudioFiles(SAD_FOLDER, sadFiles, sadFileCount);
  loadAudioFiles(GREETINGS_FOLDER, greetingsFiles, greetingsFileCount);
  loadAudioFiles(DETECTING_FOLDER, detectingFiles, detectingFileCount);
  loadAudioFiles(HEARING_FOLDER, hearingFiles, hearingFileCount);
  randomSeed(analogRead(0));

  // Initialize I2S for recording
  setupI2S();

  // Initialize audio for playback
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(12);
  audio.setTone(0.8, 1.0, 1.2);

  // Connect to WiFi and MQTT
  initConnection();
  connectToBroker("my-localhost");
  Serial.println("Waiting for Wake word...");
}

void loop() {
  mqttClient.loop();
  unsigned long currentTime = millis();

  // Update robot actions continuously
  robot.update();

  // Handle sleep state exclusively
  if (currentState == SLEEP) {
    robot.standBy();
    return;
  }

  // Handle audio streaming - but exclude TRANSITION state from auto-transitioning to IDLE
  if (currentState == PLAYING_RESPONSE || currentState == DANCE || currentState == HAPPY || currentState == MAD || currentState == SAD || currentState == GREETINGS || currentState == DETECTING_WAKE_WORD || currentState == TRANSITION || currentState == RECORDING_CONVERSATION) {
    if (currentTime - lastAudioTime >= 1) {
      lastAudioTime = currentTime;
      audio.loop();
      if (!audio.isRunning() || audioFinished) {
        // Only print and handle audio finished for states that should transition to IDLE
        if (currentState != DETECTING_WAKE_WORD && currentState != RECORDING_CONVERSATION && currentState != TRANSITION) {
          Serial.println("Audio stream finished.");
          audioFinished = false;
          currentState = IDLE;
          digitalWrite(LED_PIN, LOW);
          lastActionTime = currentTime;
          robot.idle();
        }
      }
    }
  }

  // Audio recording and state machine
  if (currentState != PLAYING_RESPONSE && currentState != DANCE && currentState != HAPPY && currentState != MAD && currentState != SAD && currentState != GREETINGS) {
    size_t bytesRead;
    esp_err_t read_result = i2s_channel_read(rx_handle, buffer, bufferSize, &bytesRead, portMAX_DELAY);
    if (read_result != ESP_OK) {
      Serial.printf("I2S read error: %d\n", read_result);
      return;
    }

    int16_t *samples = (int16_t *)buffer;
    int maxAmplitude = 0;
    for (int i = 0; i < bytesRead / 2; i++) {
      int amplitude = abs(samples[i]);
      if (amplitude > maxAmplitude) maxAmplitude = amplitude;
    }

    switch (currentState) {
      case IDLE:
        if (maxAmplitude > VAD_THRESHOLD) {
          Serial.println("Voice detected! searching for wake word...");
          recordFile = SD.open(FILENAME, FILE_WRITE);
          if (!recordFile) {
            Serial.println("Failed to open the record file!");
            return;
          }
          recordFile.seek(0);
          writeWavHeader(recordFile, 0);
          recordFile.seek(44);
          totalBytesWritten = 0;
          stateStartTime = currentTime;
          lastSoundTime = currentTime;
          currentState = DETECTING_WAKE_WORD;
          digitalWrite(LED_PIN, HIGH);
          robot.detectWakeword();
          playRandomAudio(detectingFiles, detectingFileCount, "detecting");
        }
        break;

      case DETECTING_WAKE_WORD:
        if (maxAmplitude > VAD_THRESHOLD) {
          lastSoundTime = currentTime;
        }
        recordFile.write(buffer, bytesRead);
        totalBytesWritten += bytesRead;
        if (currentTime - lastSoundTime > SILENCE_DURATION) {
          Serial.println("No voice detected. Canceling.");
          recordFile.close();
          SD.remove(FILENAME);
          currentState = IDLE;
          digitalWrite(LED_PIN, LOW);
          lastActionTime = currentTime;
          robot.idle();
        } else if (currentTime - stateStartTime >= WAKE_WORD_DETECTION_TIME) {
          recordFile.seek(0);
          writeWavHeader(recordFile, totalBytesWritten);
          recordFile.close();
          String response = uploadToServer(FILENAME, wakeWordUrl);
          if (response.indexOf("true") != -1) {
            Serial.println("Wake word detected! Transitioning to play hearing audio...");
            audioFinished = false; // Reset audio finished flag
            currentState = TRANSITION;
            stateStartTime = currentTime;
            digitalWrite(LED_PIN, HIGH);
            robot.hearingQuestion();
            playRandomAudio(hearingFiles, hearingFileCount, "hearing");
          } else {
            Serial.println("Wake word not detected.");
            SD.remove(FILENAME);
            currentState = IDLE;
            digitalWrite(LED_PIN, LOW);
            lastActionTime = currentTime;
            robot.idle();
          }
        }
        break;

      case RECORDING_CONVERSATION:
        if (maxAmplitude > VAD_THRESHOLD) {
          lastSoundTime = currentTime;
          isSpeaking = true;
        }
        recordFile.write(buffer, bytesRead);
        totalBytesWritten += bytesRead;
        if (isSpeaking && (currentTime - lastSoundTime > SILENCE_DURATION)) {
          Serial.println("Conversation finish. Uploading...");
          recordFile.seek(0);
          writeWavHeader(recordFile, totalBytesWritten);
          recordFile.close();
          robot.thinking(); // Set thinking animation during upload
          String response = uploadToServer(FILENAME, serverUrl);
          if (!response.isEmpty()) {
            currentState = PLAYING_RESPONSE;
            stateStartTime = currentTime;
            digitalWrite(LED_PIN, HIGH);
            robot.answer();
            audio.stopSong();  // Stop any ongoing audio before streaming response
            if (!connectToAudioStream()) {
              Serial.println("Failed to connect to the audio stream.");
              currentState = IDLE;
              digitalWrite(LED_PIN, LOW);
              lastActionTime = currentTime;
              robot.idle();
            }
          } else {
            currentState = IDLE;
            digitalWrite(LED_PIN, LOW);
            lastActionTime = currentTime;
            robot.idle();
          }
          isSpeaking = false;
        }
        break;

      case PLAYING_RESPONSE:
      case HAPPY:
      case MAD:
      case SAD:
      case DANCE:
      case GREETINGS:
        break;
      case SLEEP:
        break;
    }
  }

  // Handle TRANSITION state separately (no I2S data needed, just audio status check)
  if (currentState == TRANSITION) {
    // Don't use I2S audio data in this state, just check if hearing audio has finished
    if (audioFinished || !audio.isRunning()) {
      Serial.println("Hearing audio finished. Start recording conversation...");
      audioFinished = false; // Reset the flag
      audio.stopSong(); // Stop any remaining audio
      
      recordFile = SD.open(FILENAME, FILE_WRITE);
      if (!recordFile) {
        Serial.println("Failed to open the record file!");
        currentState = IDLE;
        digitalWrite(LED_PIN, LOW);
        lastActionTime = currentTime;
        robot.idle();
      } else {
        recordFile.seek(0);
        writeWavHeader(recordFile, 0);
        recordFile.seek(44);
        totalBytesWritten = 0;
        stateStartTime = currentTime;
        lastSoundTime = currentTime;
        currentState = RECORDING_CONVERSATION;
        digitalWrite(LED_PIN, HIGH);
        robot.hearingQuestion();
        isSpeaking = false; // Reset speaking flag
      }
    }
  }

  // Maintain WiFi connection
  static unsigned long lastCheck = 0;
  if (currentTime - lastCheck > 30000) {
    lastCheck = currentTime;
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected, reconnecting...");
      initConnection();
      connectToBroker("my-localhost");
    }
  }

  // Cooldown
  if (currentTime - lastActionTime < COOLDOWN_TIME && currentState == IDLE) {
    return;
  }
}

void audio_info(const char *info) {
  Serial.print("info: ");
  Serial.println(info);
  if (strstr(info, "end of stream")) {
    audioFinished = true;
  } else if (strstr(info, "Stream lost")) {
    if (!audioFinished) {
      audioFinished = true;
    }
  }
}

void audio_showstreamtitle(const char *info) {
  Serial.print("Now playing: ");
  Serial.println(info);
}

void audio_eof_mp3(const char *info) {
  Serial.print("eof_mp3: ");
  Serial.println(info);
  audioFinished = true;
}