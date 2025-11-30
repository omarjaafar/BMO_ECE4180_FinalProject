#include "MicVoice.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "Sound.h"
#include "DisplayUI.h"

// --------- CONFIG ---------

// SPW2430 DC -> ADC1 pin
#define MIC_PIN        6           // your chosen ADC1-capable pin
// Pushbutton -> GPIO 7 (to GND, INPUT_PULLUP)
#define MIC_BTN_PIN    7

#define SAMPLE_RATE    16000       // Hz
#define MAX_RECORD_SEC 5           // max recording length
#define MAX_SAMPLES    (SAMPLE_RATE * MAX_RECORD_SEC)

// Backend voice endpoint (set to your PC's LAN IP)
const char* VOICE_ENDPOINT = "http://172.20.10.3:3001/api/voice/input";

// Reuse WiFi creds
const char* micWifiSsid     = "Ridwan";
const char* micWifiPassword = "Derp2414";

// --------- STATE ---------

static int16_t audioBuf[MAX_SAMPLES];
static size_t  sampleCount = 0;
static bool    recording   = false;

static bool lastButtonState = false;
static unsigned long lastSampleMicros = 0;

// --------- INTERNAL HELPERS ---------

static void connectWifiIfNeeded() {
  if (WiFi.status() == WL_CONNECTED) return;

  WiFi.mode(WIFI_STA);
  WiFi.begin(micWifiSsid, micWifiPassword);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
    delay(250);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("MicVoice: WiFi connected, IP=");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("MicVoice: WiFi connect failed");
  }
}

static void startRecording() {
  sampleCount = 0;
  recording = true;
  lastSampleMicros = micros();
  Serial.println("MicVoice: start recording");
}

static void stopRecordingAndSend() {
  recording = false;
  Serial.println("MicVoice: stop recording");

  if (sampleCount == 0) {
    Serial.println("MicVoice: no samples captured");
    return;
  }

  // const size_t MIN_SAMPLES = SAMPLE_RATE / 5; // 0.2 s
  // if (sampleCount < MIN_SAMPLES) {
  //   Serial.print("MicVoice: too short (");
  //   Serial.print(sampleCount);
  //   Serial.println(" samples), not sending");
  //   sampleCount = 0;
  //   return;
  // }

  connectWifiIfNeeded();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("MicVoice: WiFi not connected, cannot send audio");
    return;
  }

  HTTPClient http;
  http.begin(VOICE_ENDPOINT);
  http.addHeader("Content-Type", "application/octet-stream");

  size_t bytesLen = sampleCount * sizeof(int16_t);
  Serial.print("MicVoice: sending ");
  Serial.print(sampleCount);
  Serial.println(" samples");

  int code = http.POST((uint8_t*)audioBuf, bytesLen);
  Serial.print("MicVoice: HTTP POST result = ");
  Serial.println(code);

  if (code == 200) {
    String response = http.getString();
    Serial.println("MicVoice: raw server response:");
    Serial.println(response);

    StaticJsonDocument<1536> doc;
    DeserializationError err = deserializeJson(doc, response);
    if (err) {
      Serial.print("MicVoice: JSON parse error: ");
      Serial.println(err.f_str());
    } else {
      const char* bmoText = doc["bmoResponse"]["content"];
      if (bmoText) {
        Serial.print("MicVoice: BMO says: ");
        Serial.println(bmoText);

        showBMOReplyOnScreen(bmoText);
        soundVoiceReply();
      } else {
        Serial.println("MicVoice: bmoResponse.content missing");
      }
    }
  }

  http.end();
  sampleCount = 0;
}

// --------- PUBLIC API ---------

void initMicVoice() {
  analogReadResolution(12);
  analogSetPinAttenuation(MIC_PIN, ADC_0db);
  pinMode(MIC_PIN, INPUT);

  pinMode(MIC_BTN_PIN, INPUT_PULLUP);

  if (!Serial) {
    Serial.begin(115200);
    delay(50);
  }

  Serial.println("MicVoice: initialized");
}

void micVoiceLoop() {
  bool btnPressed = (digitalRead(MIC_BTN_PIN) == LOW);

  if (btnPressed && !lastButtonState) {
    startRecording();
  }

  if (!btnPressed && lastButtonState) {
    stopRecordingAndSend();
  }

  lastButtonState = btnPressed;

  if (recording && sampleCount < MAX_SAMPLES) {
    unsigned long nowMicros = micros();
    unsigned long interval  = 1000000UL / SAMPLE_RATE;

    if (nowMicros - lastSampleMicros >= interval) {
      lastSampleMicros += interval;

      int raw = analogRead(MIC_PIN);            // 0..4095
      int16_t centered = (int16_t)(raw - 2048); // center around 0
      audioBuf[sampleCount++] = centered;
    }
  }
}
