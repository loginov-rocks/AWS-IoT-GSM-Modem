# AWS IoT GSM Modem

## Hardware

![Scheme](https://raw.githubusercontent.com/loginov-rocks/AWS-IoT-GSM-Modem/main/docs/Scheme.png)

## Quick Start

Copy `~Secrets.h` to `Secrets.h` and fill certificates and private key file contents.

## Messages

* `{"tx": "AT\r\n"}` - ping

### Calls

* `{"tx": "ATD+7XXXXXXXXXX;\r\n"}` - call `+7XXXXXXXXXX`
* `{"tx": "ATA\r\n"}` - answer call
* `{"tx": "ATH0\r\n"}` - reject call

### SMS

* `{"tx": "AT+CSCS=\"GSM\"\r\n"}` - set GSM encoding
* `{"tx": "AT+CMGS=\"+7XXXXXXXXXX\"\r\n"}` - send SMS to `+7XXXXXXXXXX`, followed by a message:
  * `{"tx": "Hello, world!\u001a"}` - should end up with substitute character `\u001a`
* `{"tx": "AT+CMGL=\"ALL\"\r\n"}` - read all SMS
* `{"tx": "AT+CMGR=13\r\n"}` - read SMS number `13`
