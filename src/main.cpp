#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

#include "Secrets.h"

#define SERIAL_BAUDRATE 9600

#define WIFI_HOSTNAME "AWS-IoT-GSM-Modem"
#define WIFI_ACCESS_POINT_SSID "AWS-IoT-GSM-Modem"

BearSSL::X509List trustAnchorCertificate(rootCaCertificate);
BearSSL::X509List clientCertificate(deviceCertificate);
BearSSL::PrivateKey clientPrivateKey(privateKeyFile);

WiFiClientSecure wiFiClient;
PubSubClient pubSubClient(wiFiClient);

const char mqttHost[] = "a3lfjfqxp1f1qa-ats.iot.eu-west-1.amazonaws.com";
const unsigned int mqttPort = 8883;
const char clientId[] = "testAwsIot";
const char subscribeTopicFiler[] = "testAwsIotTopic";
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

void setupTime(void)
{
  time_t nowish = 1510592825;

  Serial.print("Setting time using SNTP");
  // configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void setupCertificates()
{
  wiFiClient.setTrustAnchors(&trustAnchorCertificate);
  wiFiClient.setClientRSACert(&clientCertificate, &clientPrivateKey);
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

  pubSubClient.publish(publishTopicName, message);

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

void setupPubSubClient()
{
  pubSubClient.setServer(mqttHost, mqttPort);
  pubSubClient.setCallback(receiveMessage);

  Serial.println("Connecting to AWS IOT");

  while (!pubSubClient.connect(clientId))
  {
    Serial.print(".");
    delay(1000);
  }

  if (!pubSubClient.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  pubSubClient.subscribe(subscribeTopicFiler);

  Serial.println("AWS IoT Connected!");
}

void setup()
{
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Setup...");

  setupWiFi();
  setupTime();
  setupCertificates();
  setupPubSubClient();

  Serial.println("Setup was successful!");
}

void loop()
{
  if (!pubSubClient.connected())
  {
    setupPubSubClient();
    return;
  }

  pubSubClient.loop();

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
