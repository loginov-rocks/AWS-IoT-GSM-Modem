#include <ArduinoJson.h>
#include <WiFiManager.h>

#include "AwsIot.h"
#include "Secrets.h"

#define SERIAL_BAUDRATE 9600
#define DEBUG_OUTPUT false

#define WIFI_HOSTNAME "AWS-IoT-GSM-Modem"
#define WIFI_ACCESS_POINT_SSID "AWS-IoT-GSM-Modem"

AwsIot awsIot;

BearSSL::X509List trustAnchorCertificate(rootCaCertificate);
BearSSL::X509List clientCertificate(deviceCertificate);
BearSSL::PrivateKey clientPrivateKey(privateKeyFile);

const char endpoint[] = "a3lfjfqxp1f1qa-ats.iot.eu-west-1.amazonaws.com";
const char clientId[] = "testAwsIot";
const char subscribeTopicFilter[] = "testAwsIotTopic";
const char publishTopicName[] = "testAwsIotTopic";

String rxLine;

/**
 * @see https://github.com/tzapu/WiFiManager/blob/master/examples/Basic/Basic.ino
 */
void setupWiFi()
{
  if (DEBUG_OUTPUT)
  {
    Serial.println("Setup Wi-Fi...");
  }

  WiFi.hostname(WIFI_HOSTNAME);

  WiFiManager wiFiManager;
  wiFiManager.setDebugOutput(DEBUG_OUTPUT);
  wiFiManager.autoConnect(WIFI_ACCESS_POINT_SSID);

  if (DEBUG_OUTPUT)
  {
    Serial.println("Wi-Fi setup was successful!");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
  }
}

void publishMessage()
{
  // @see https://arduinojson.org/v6/how-to/determine-the-capacity-of-the-jsondocument/
  DynamicJsonDocument json(1024);
  json["millis"] = millis();

  struct tm timeinfo;
  time_t now = time(nullptr);
  gmtime_r(&now, &timeinfo);
  json["time"] = asctime(&timeinfo);

  json["rx"] = rxLine;

  char message[1024];
  serializeJson(json, message);

  awsIot.publishMessage(publishTopicName, message);

  if (DEBUG_OUTPUT)
  {
    Serial.print("Published [");
    Serial.print(publishTopicName);
    Serial.print("]: ");
    Serial.println(message);
  }
}

void receiveMessage(char *topic, byte *payload, unsigned int length)
{
  DynamicJsonDocument json(1024);
  deserializeJson(json, payload);

  if (!json.containsKey("tx"))
  {
    return;
  }

  const char *tx = json["tx"];

  Serial.print(tx);
}

void setupAwsIot()
{
  if (DEBUG_OUTPUT)
  {
    Serial.println("Setup AWS IoT...");
  }

  awsIot.setDebugOutput(DEBUG_OUTPUT)
      .setCertificates(&trustAnchorCertificate, &clientCertificate, &clientPrivateKey)
      .setEndpoint(endpoint)
      .setReceiveMessageCallback(receiveMessage)
      .setClientId(clientId)
      .setSubscribeTopicFilter(subscribeTopicFilter)
      .connect();

  if (DEBUG_OUTPUT)
  {
    Serial.println("AWS IoT setup was successful!");
  }
}

void setup()
{
  Serial.begin(SERIAL_BAUDRATE);

  if (DEBUG_OUTPUT)
  {
    Serial.println("Setup...");
  }

  setupWiFi();
  setupAwsIot();

  if (DEBUG_OUTPUT)
  {
    Serial.println("Setup was successful!");
  }
}

void loop()
{
  awsIot.loop();

  while (Serial.available() > 0)
  {
    char rxChar = Serial.read();
    rxLine += rxChar;

    if (rxChar == '\n')
    {
      // Ignore empty lines.
      if (!rxLine.equals("\r\n"))
      {
        publishMessage();
      }

      rxLine = "";
    }
  }
}
