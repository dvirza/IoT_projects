#include <pgmspace.h>

#define THINGNAME "esp32_1"  // replace with thing name

const char WIFI_SSID[] = "WiFi-Yehida _2.4";  // replace with wifi ssid
const char WIFI_PASSWORD[] = "Lutter@1212";   // replace with wifi password

const char AWS_IOT_ENDPOINT[] = "a3l9qbpyz2pw5m-ats.iot.us-east-1.amazonaws.com";  // replace with iot endpoint

// Amazon Root CA 1 :
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
INSERT HERE YOUR AWS CA CERTIFICATE
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
INSERT HERE YOUR AWS DEVICE CERTIFICATE
-----END CERTIFICATE-----

)KEY";

// Device Private key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
INSERT HERE YOUR AWS PRIVATE CERTIFICATE
-----END RSA PRIVATE KEY-----

)KEY";