#ifndef AwsIot_h
#define AwsIot_h

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

class AwsIot
{
private:
    WiFiClientSecure wiFiSecureClient;
    PubSubClient *pubSubClient;

    const char *clientId;
    const char *subscribeTopicFilter;

    void setupTime();

public:
    AwsIot();

    AwsIot &setCertificates(const X509List *, const X509List *, const PrivateKey *);
    AwsIot &setEndpoint(const char *);
    AwsIot &setReceiveMessageCallback(std::function<void(char *, uint8_t *, unsigned int)>);

    AwsIot &setClientId(const char *);
    AwsIot &setSubscribeTopicFilter(const char *);

    void connect();
    void loop();

    boolean publishMessage(const char *, const char *);
};

#endif
