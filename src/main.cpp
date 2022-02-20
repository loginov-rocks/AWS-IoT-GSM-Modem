#include <ArduinoJson.h>
#include <WiFiManager.h>

#include "AwsIot.h"
#include "Secrets.h"

#define SERIAL_BAUDRATE 9600

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
  Serial.println("Setup Wi-Fi...");

  WiFi.hostname(WIFI_HOSTNAME);

  WiFiManager wifiManager;
  wifiManager.autoConnect(WIFI_ACCESS_POINT_SSID);

  Serial.println("Wi-Fi setup was successful!");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
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

  Serial.print("Published [");
  Serial.print(publishTopicName);
  Serial.print("]: ");
  Serial.println(message);
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
  Serial.println("Setup AWS IoT...");

  awsIot.setCertificates(&trustAnchorCertificate, &clientCertificate, &clientPrivateKey)
      .setEndpoint(endpoint)
      .setReceiveMessageCallback(receiveMessage)
      .setClientId(clientId)
      .setSubscribeTopicFilter(subscribeTopicFilter)
      .connect();

  Serial.println("AWS IoT setup was successful!");
}

void setup()
{
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Setup...");

  setupWiFi();
  setupAwsIot();

  Serial.println("Setup was successful!");
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
      publishMessage();

      rxLine = "";
    }
  }
}
