#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <string.h>

#define ss 5
#define rst 14
#define dio0 2
#define MAX_REDIRECT 5
#define MAX_BUFFER_SIZE 32

const char *ssid = "11";
const char *password = "0101010101";

const char *googleScriptURL = "https://script.google.com/macros/s/AKfycbztDOn1IdP2JrgMtb51rxRHkmdZNFcfQziyxM8Vi3q2HNldOKyOOoP1eNrN7wAb8to/exec";

float avgFlowRate = 0.0;
float stdFlowRate = 0.0;
unsigned long senderTimestamp = 0;

void sendToGoogleSheets(float avgFlow, float stdFlow, unsigned long senderTimestamp)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected - attempting reconnection...");
    WiFi.disconnect();
    delay(1000);
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10)
    {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    if (WiFi.status() == WL_CONNECTED)
      Serial.println("\nWiFi reconnected successfully");
    else
      Serial.println("\nFailed to reconnect WiFi");
    return;
  }
  HTTPClient http;

  http.begin(googleScriptURL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(15000);
  http.setConnectTimeout(10000);
  http.setReuse(false);
  http.setRedirectLimit(MAX_REDIRECT);

  StaticJsonDocument<400> doc;
  doc["timestamp"] = senderTimestamp;
  doc["avg_flow_rate"] = avgFlow;
  doc["std_flow_rate"] = stdFlow;

  String jsonString;
  serializeJson(doc, jsonString);

  Serial.print("Sending to Google Sheets: ");
  Serial.println(jsonString);

  Serial.println("Debug - Individual values:");
  Serial.print("Sender Timestamp: ");
  Serial.println(senderTimestamp);
  Serial.print("Average Flow Rate: ");
  Serial.println(avgFlow);
  Serial.print("Standard Deviation Flow Rate: ");
  Serial.println(stdFlow);

  Serial.println("Attempting HTTP POST...");
  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0)
  {
    String response = http.getString();
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);
    Serial.print("Server Response: ");
    Serial.println(response);

    if (httpResponseCode == 302 || httpResponseCode == 301)
    {
      Serial.println("WARNING: Received redirect! Please update your Google Script URL.");
      String location = http.header("Location");
      if (location.length() > 0)
      {
        Serial.print("Redirect location: ");
        Serial.println(location);
      }
    }
  }
  else
  {
    Serial.print("Error sending data. HTTP Error Code: ");
    Serial.println(httpResponseCode);

    switch (httpResponseCode)
    {
    case -1:
      Serial.println("Error: Connection refused");
      break;
    case -2:
      Serial.println("Error: Send header failed");
      break;
    case -3:
      Serial.println("Error: Send payload failed");
      break;
    case -4:
      Serial.println("Error: Not connected");
      break;
    case -5:
      Serial.println("Error: Connection lost");
      break;
    case -6:
      Serial.println("Error: No stream");
      break;
    case -7:
      Serial.println("Error: No HTTP server");
      break;
    case -8:
      Serial.println("Error: Too less RAM");
      break;
    case -9:
      Serial.println("Error: Encoding");
      break;
    case -10:
      Serial.println("Error: Stream write");
      break;
    case -11:
      Serial.println("Error: Read timeout - Check internet connection and Google Script URL");
      break;
    default:
      Serial.println("Error: Unknown HTTP error");
    }
  }

  http.end();
}

void setup()
{
  Serial.begin(9600);
  Serial.println("LoRa Receiver with Google Sheets Integration");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  LoRa.setPins(ss, rst, dio0);

  while (!LoRa.begin(433E6))
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

  delay(3000);
}

bool parseLoRaBinaryData(uint8_t *data, int length)
{
  if (length != 12)
  {
    Serial.print("Invalid binary packet length: ");
    Serial.print(length);
    Serial.println(" bytes (expected 12)");
    return false;
  }

  memcpy(&senderTimestamp, data, sizeof(senderTimestamp));
  memcpy(&avgFlowRate, data + 4, sizeof(avgFlowRate));
  memcpy(&stdFlowRate, data + 8, sizeof(stdFlowRate));

  Serial.println("Parsed LoRa binary data:");
  Serial.print("Sender Timestamp: ");
  Serial.println(senderTimestamp);
  Serial.print("Average Flow Rate: ");
  Serial.println(avgFlowRate, 3);
  Serial.print("Standard Deviation Flow Rate: ");
  Serial.println(stdFlowRate, 3);

  return true;
}

void loop()
{
  int packetSize = LoRa.parsePacket();
  if (!packetSize)
    return;
  Serial.print("Received binary packet: ");
  Serial.print(packetSize);
  Serial.println(" bytes");

  uint8_t buffer[MAX_BUFFER_SIZE];
  int bytesRead = 0;
  while (LoRa.available() && bytesRead < sizeof(buffer))
  {
    buffer[bytesRead] = LoRa.read();
    bytesRead++;
  }

  Serial.print("RSSI: ");
  Serial.println(LoRa.packetRssi());

  if (parseLoRaBinaryData(buffer, bytesRead))
  {
    Serial.println("Sending parsed data to Google Sheets...");
    sendToGoogleSheets(avgFlowRate, stdFlowRate, senderTimestamp);
  }
}