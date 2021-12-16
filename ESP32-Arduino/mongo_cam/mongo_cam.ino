/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-post-image-photo-server/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/



#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "mbedtls/base64.h"
/************ Useful Setup Constants *****************/


const char* AtlasAPIEndpoint = "https://eu-west-1.aws.data.mongodb-api.com/app/CHANGE_THIS/endpoint/data/beta/action/insertOne";
const char* AtlasAPIKey = "YOUR_API_KEY";
const char* ssid = "machair";
const char* password = "2efdaf4b59";

/* This one Works withj Atlas OK*/

const char* rootCACertificate = \
                                "-----BEGIN CERTIFICATE-----\n" \
                                "MIIF6TCCA9GgAwIBAgIQBeTcO5Q4qzuFl8umoZhQ4zANBgkqhkiG9w0BAQwFADCB\n" \
                                "iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n" \
                                "cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n" \
                                "BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTQw\n" \
                                "OTEyMDAwMDAwWhcNMjQwOTExMjM1OTU5WjBfMQswCQYDVQQGEwJGUjEOMAwGA1UE\n" \
                                "CBMFUGFyaXMxDjAMBgNVBAcTBVBhcmlzMQ4wDAYDVQQKEwVHYW5kaTEgMB4GA1UE\n" \
                                "AxMXR2FuZGkgU3RhbmRhcmQgU1NMIENBIDIwggEiMA0GCSqGSIb3DQEBAQUAA4IB\n" \
                                "DwAwggEKAoIBAQCUBC2meZV0/9UAPPWu2JSxKXzAjwsLibmCg5duNyj1ohrP0pIL\n" \
                                "m6jTh5RzhBCf3DXLwi2SrCG5yzv8QMHBgyHwv/j2nPqcghDA0I5O5Q1MsJFckLSk\n" \
                                "QFEW2uSEEi0FXKEfFxkkUap66uEHG4aNAXLy59SDIzme4OFMH2sio7QQZrDtgpbX\n" \
                                "bmq08j+1QvzdirWrui0dOnWbMdw+naxb00ENbLAb9Tr1eeohovj0M1JLJC0epJmx\n" \
                                "bUi8uBL+cnB89/sCdfSN3tbawKAyGlLfOGsuRTg/PwSWAP2h9KK71RfWJ3wbWFmV\n" \
                                "XooS/ZyrgT5SKEhRhWvzkbKGPym1bgNi7tYFAgMBAAGjggF1MIIBcTAfBgNVHSME\n" \
                                "GDAWgBRTeb9aqitKz1SA4dibwJ3ysgNmyzAdBgNVHQ4EFgQUs5Cn2MmvTs1hPJ98\n" \
                                "rV1/Qf1pMOowDgYDVR0PAQH/BAQDAgGGMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYD\n" \
                                "VR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMCIGA1UdIAQbMBkwDQYLKwYBBAGy\n" \
                                "MQECAhowCAYGZ4EMAQIBMFAGA1UdHwRJMEcwRaBDoEGGP2h0dHA6Ly9jcmwudXNl\n" \
                                "cnRydXN0LmNvbS9VU0VSVHJ1c3RSU0FDZXJ0aWZpY2F0aW9uQXV0aG9yaXR5LmNy\n" \
                                "bDB2BggrBgEFBQcBAQRqMGgwPwYIKwYBBQUHMAKGM2h0dHA6Ly9jcnQudXNlcnRy\n" \
                                "dXN0LmNvbS9VU0VSVHJ1c3RSU0FBZGRUcnVzdENBLmNydDAlBggrBgEFBQcwAYYZ\n" \
                                "aHR0cDovL29jc3AudXNlcnRydXN0LmNvbTANBgkqhkiG9w0BAQwFAAOCAgEAWGf9\n" \
                                "crJq13xhlhl+2UNG0SZ9yFP6ZrBrLafTqlb3OojQO3LJUP33WbKqaPWMcwO7lWUX\n" \
                                "zi8c3ZgTopHJ7qFAbjyY1lzzsiI8Le4bpOHeICQW8owRc5E69vrOJAKHypPstLbI\n" \
                                "FhfFcvwnQPYT/pOmnVHvPCvYd1ebjGU6NSU2t7WKY28HJ5OxYI2A25bUeo8tqxyI\n" \
                                "yW5+1mUfr13KFj8oRtygNeX56eXVlogMT8a3d2dIhCe2H7Bo26y/d7CQuKLJHDJd\n" \
                                "ArolQ4FCR7vY4Y8MDEZf7kYzawMUgtN+zY+vkNaOJH1AQrRqahfGlZfh8jjNp+20\n" \
                                "J0CT33KpuMZmYzc4ZCIwojvxuch7yPspOqsactIGEk72gtQjbz7Dk+XYtsDe3CMW\n" \
                                "1hMwt6CaDixVBgBwAc/qOR2A24j3pSC4W/0xJmmPLQphgzpHphNULB7j7UTKvGof\n" \
                                "KA5R2d4On3XNDgOVyvnFqSot/kGkoUeuDcL5OWYzSlvhhChZbH2UF3bkRYKtcCD9\n" \
                                "0m9jqNf6oDP6N8v3smWe2lBvP+Sn845dWDKXcCMu5/3EFZucJ48y7RetWIExKREa\n" \
                                "m9T8bJUox04FB6b9HbwZ4ui3uRGKLXASUoWNjDNKD/yZkuBjcNqllEdjB+dYxzFf\n" \
                                "BT02Vf6Dsuimrdfp5gJ0iHRc2jTbkNJtUQoj1iM=\n" \
                                "-----END CERTIFICATE-----\n";

/****************************************************************************/
/* This converts a buffer to a Stream whcih lets POST send larger data sets */
/* Was the Solution to ESP32 Secure HTTPS LIbrary not working, Constructor takes */
/* And Arduino_JSON object */

class JSONStream: public Stream {
  private:
    uint8_t *buffer;
    size_t buffer_size;
    size_t served;
    int start;
    int end;

  public:
    JSONStream(DynamicJsonDocument &payload ) {
      int jsonlen = measureJson(payload);
      this->buffer = (uint8_t*) heap_caps_calloc(jsonlen + 1, 1, MALLOC_CAP_8BIT);
      this->buffer_size = serializeJson(payload,  this->buffer, jsonlen + 1);
      this->served = 0;
      this->start = millis();
    }
    ~JSONStream() {
      heap_caps_free((void*)this->buffer);
    }

    void clear() {}
    size_t write(uint8_t) {}
    int  available() {
      size_t whatsleft = buffer_size - served;
      if (whatsleft == 0) return -1;
      return whatsleft;
    }
    int peek() {
      return 0;
    }
    void flush() { }
    int read() {}
    size_t readBytes(uint8_t *outbuf, size_t nbytes) {
      //Serial.println(millis()-this->start);
      if (nbytes > buffer_size - served) {
        nbytes = buffer_size - served;
      }
      memcpy(outbuf, buffer + served, nbytes);
      served = served + nbytes;
      return nbytes;
    }

};


/**************************************************************/


// CAMERA_MODEL_AI_THINKER
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

void configureCamera()
{

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


  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 12;  //0-63 lower number means higher quality
  config.fb_count = 1;


  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  //Assume USB port on top
  sensor_t * s = esp_camera_sensor_get();
  s->set_saturation(s,2);
  s->set_vflip(s, 1);
  //s->set_exposure_ctrl(s,0);
  //  s->set_whitebal(s, 0);
  s->set_hmirror(s, 1);
}

const int timerInterval = 333;    // time between each HTTP POST image
unsigned long previousMillis = 0;   // last time image was sent

WiFiClientSecure *client;
HTTPClient https;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());

  configureCamera();
  setClock();


  client = new WiFiClientSecure;
  if (client) {
    client -> setCACert(rootCACertificate);
  }


  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  delay(200);
  digitalWrite(4, LOW);


  if (!https.begin(*client, AtlasAPIEndpoint)) {  // HTTPS

    Serial.println("Error starting client");
  }

  sendPhoto();
  pinMode(33, OUTPUT);
  pinMode(4, OUTPUT);
}

/* Simple loop to take a shot every N milliseconds*/

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= timerInterval) {
    sendPhoto();
    previousMillis = currentMillis;
  }
}


const char* base64EncodeImage(camera_fb_t *fb)
{
  /* Base 64 encode the image - this was the simplest way*/
  unsigned char* src = fb->buf;
  size_t slen = fb->len;
  size_t dlen = 0;

  int err = mbedtls_base64_encode(NULL, 0 , &dlen, src, slen);

  /* For a larger allocation like thi you need to use capability allocation*/
  const char *dst = (char*) heap_caps_calloc(dlen, 1, MALLOC_CAP_8BIT);
  size_t olen;
  err = mbedtls_base64_encode((unsigned char*)dst, dlen , &olen, src, slen);

  if (err != 0) {
    Serial.printf("error base64 encoding, error %d, buff size: %d", err, olen);
    return NULL;
  }

  return dst;
}



String sendPhoto() {

  /* Capture a JPEG from the camera*/
  
Serial.println(ESP.getFreeHeap());
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }

  https.addHeader("Content-Type", "application/json");
    https.addHeader("api-key", AtlasAPIKey);


  /* Generate an InsertOne Payload*/
  DynamicJsonDocument payload (1024);
  payload["dataSource"] = "Cluster0";
  payload["database"] = "espcam";
  payload["collection"] = "frames";
  payload["document"]["camname"] = "MongoCam_1";

  time_t nowSecs = time(nullptr);

  char datestring[32];

  sprintf(datestring, "%lu000", nowSecs);

  payload["document"]["time"]["$date"]["$numberLong"] = datestring; /*Encode Date() as EJSON*/

  const char* base64Image = base64EncodeImage(fb) ;
  payload["document"]["img"]["$binary"]["base64"] = base64Image; /*Encide as a Binary() */
  payload["document"]["img"]["$binary"]["subType"] = "07";


  JSONStream *buffer = new JSONStream(payload);
  heap_caps_free((void*)base64Image); //Free Base64 as soon as we serialze JSON and save some RAM
  digitalWrite(33, LOW);
  int atime = millis();
  int httpCode = https.sendRequest("POST", buffer, buffer->available());
  Serial.println(millis() - atime);
  digitalWrite(33, HIGH);
  delete buffer;

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = https.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTPS] POST ... failed, error: %s\n", https.errorToString(httpCode).c_str());
  }
 
} 


/* No real time clock so set this to let us timestamp docs*/

void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}
