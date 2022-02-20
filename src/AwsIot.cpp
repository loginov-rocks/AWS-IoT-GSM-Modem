#include "AwsIot.h"

void AwsIot::setupTime()
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

AwsIot::AwsIot() : wiFiSecureClient()
{
    pubSubClient = new PubSubClient(wiFiSecureClient);
}

AwsIot &AwsIot::setCertificates(const X509List *trustAnchorCertificate, const X509List *clientCertificate, const PrivateKey *clientPrivateKey)
{
    setupTime();

    wiFiSecureClient.setTrustAnchors(trustAnchorCertificate);
    wiFiSecureClient.setClientRSACert(clientCertificate, clientPrivateKey);

    return *this;
}

AwsIot &AwsIot::setEndpoint(const char *endpoint)
{
    pubSubClient->setServer(endpoint, 8883);

    return *this;
}

AwsIot &AwsIot::setReceiveMessageCallback(std::function<void(char *, uint8_t *, unsigned int)> callback)
{
    pubSubClient->setCallback(callback);

    return *this;
}

AwsIot &AwsIot::setClientId(const char *_clientId)
{
    clientId = _clientId;

    return *this;
}

AwsIot &AwsIot::setSubscribeTopicFilter(const char *_subscribeTopicFilter)
{
    subscribeTopicFilter = _subscribeTopicFilter;

    return *this;
}

void AwsIot::connect()
{
    Serial.println("Connecting to AWS IOT");

    while (!pubSubClient->connect(clientId))
    {
        Serial.print(".");
        delay(1000);
    }

    if (!pubSubClient->connected())
    {
        Serial.println("AWS IoT Timeout!");
        return;
    }

    pubSubClient->subscribe(subscribeTopicFilter);

    Serial.println("AWS IoT Connected!");
}

void AwsIot::loop()
{
    if (!pubSubClient->connected())
    {
        connect();
    }

    pubSubClient->loop();
}

boolean AwsIot::publishMessage(const char *topicName, const char *message)
{
    return pubSubClient->publish(topicName, message);
}
