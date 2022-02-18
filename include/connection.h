#define WIFI_APT " "
#define WIFI_SSID " "

#define WIFI_PRIMARY_SSID "A2C_Device"    //"Infinix Smart 3 Plus"
#define WIFI_PRIMARY_PASS "PatA2C_Device1"    //"4099bde020b9"
#define WIFI_SECONDARY_SSID " " //"MTNHyNetflex 2.4Ghz"
#define WIFI_SECONDARY_PASS " " //"43792866"

#define WIFI_TERTIARY_SSID  "DESKTOP-POOMJG6 0347"
#define WIFI_TERTIARY_PASS  "9249u]1J"

#define WS_PING_INTERVAL 5000
#define WS_PONG_INTERVAL 3000
#define WS_RECONNECT_INTERVAL 3000
#define WS_DISCONNECT_TIMEOUT_COUNT 2

const char *ws_username = "uwsname";
const char *ws_userpass = "uWSpass1357";
const char *ws_domain = "simbank.local";
bool ws_domain_resolved = false;

//"127.0.0.1" never use this with the swoole server
/*
#define SERVER_HOST "app.iothings.ng"
#define SERVER_PORT 443     //80 //8083 //3000    //443     //
#define SERVER_URL "/socket.io/?transport=websocket&token=live-53478db4-d44c-47e0-8c29-cc1b05849b2a"
#define SSL_CERT "215FBE6A603BA8523260E55A4CD9CD0D3885842E1C4334B7C3A69E86915B686D"

#define SERVER_ADDR "wss://app.iothings.ng/socket.io/?EIO=3&transaport=websocket&token=live-53478db4-d44c-47e0-8c29-cc1b05849b2a"  //server ip address or base domain address

// KH, Update Let's Encrypt CA Cert, May 2nd 2021
// Valid from : Sep  4 00:00:00 2020 GMT
// Expired    : Sep 15 16:00:00 2025 GMT
const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIEZTCCA02gAwIBAgIQQAF1BIMUpMghjISpDBbN3zANBgkqhkiG9w0BAQsFADA/
MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT
DkRTVCBSb290IENBIFgzMB4XDTIwMTAwNzE5MjE0MFoXDTIxMDkyOTE5MjE0MFow
MjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxCzAJBgNVBAMT
AlIzMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuwIVKMz2oJTTDxLs
jVWSw/iC8ZmmekKIp10mqrUrucVMsa+Oa/l1yKPXD0eUFFU1V4yeqKI5GfWCPEKp
Tm71O8Mu243AsFzzWTjn7c9p8FoLG77AlCQlh/o3cbMT5xys4Zvv2+Q7RVJFlqnB
U840yFLuta7tj95gcOKlVKu2bQ6XpUA0ayvTvGbrZjR8+muLj1cpmfgwF126cm/7
gcWt0oZYPRfH5wm78Sv3htzB2nFd1EbjzK0lwYi8YGd1ZrPxGPeiXOZT/zqItkel
/xMY6pgJdz+dU/nPAeX1pnAXFK9jpP+Zs5Od3FOnBv5IhR2haa4ldbsTzFID9e1R
oYvbFQIDAQABo4IBaDCCAWQwEgYDVR0TAQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8E
BAMCAYYwSwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5p
ZGVudHJ1c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTE
p7Gkeyxx+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEE
AYLfEwEBATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2Vu
Y3J5cHQub3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0
LmNvbS9EU1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYf
r52LFMLGMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjANBgkqhkiG9w0B
AQsFAAOCAQEA2UzgyfWEiDcx27sT4rP8i2tiEmxYt0l+PAK3qB8oYevO4C5z70kH
ejWEHx2taPDY/laBL21/WKZuNTYQHHPD5b1tXgHXbnL7KqC401dk5VvCadTQsvd8
S8MXjohyc9z9/G2948kLjmE6Flh9dDYrVYA9x2O+hEPGOaEOa1eePynBgPayvUfL
qjBstzLhWVQLGAkXXmNs+5ZnPBxzDJOLxhF2JIbeQAcH5H0tZrUlo5ZYyOqA7s9p
O5b85o3AM/OJ+CktFBQtfvBhcJVd9wvlwPsk+uyOy2HI7mNxKKgsBTt375teA2Tw
UdHkhVNcsAKX1H7GNNLOEADksd86wuoXvg==
-----END CERTIFICATE-----
)EOF";
*/


//#define SSL_CERT "3BEE5E245BD0A3D79D8152190E70006FA46B793FED476476B6E329D32A44B6F6" // for app.iothings.ng 


// #define SERVER_HOST "a2c-iot-server.patriciadev.com"
// #define SERVER_PORT 443     //80 //8083 //3000    // 
// #define SERVER_URL "/socket.io/?transport=websocket&token=live-0a37ba5b-96d0-4107-8017-e7567aa93984"
// #define SSL_CERT "8AFF6597AFF527CDF9DBFF2DC4FB7ED32E77F82D7D71DB528A487BB81B8049E7"  // for dev



// #define SERVER_HOST "glover-a2c-server.patriciadev.com"
// #define SERVER_PORT 443     //80 //8083 //3000    // 
// #define SERVER_URL "/socket.io/?transport=websocket&token=live-b43f1bfb-643a-4d6c-b7a4-2f12afffdc00"
// #define SSL_CERT "8AFF6597AFF527CDF9DBFF2DC4FB7ED32E77F82D7D71DB528A487BB81B8049E7"  // for dev

/*
// glover dev
#define SERVER_HOST "glover-a2c-server.private-gke.patriciadev.com"
#define SERVER_PORT 443     //80 //8083 //3000    // 
#define SERVER_URL "/socket.io/?transport=websocket&token=live-9864b555-2c89-4eec-ab96-42ebd080ef74"
*/

/*
// live-e72114d3-d73b-45d2-ab10-8f6b585e9ce6
//  clackweb dev
#define SERVER_URL "/socket.io/?transport=websocket&token=live-841aefdf-3ff6-4c9e-9c2d-dac3e2491598"
#define SERVER_HOST "a2c.private-gke.patriciadev.com"   //https://
#define SERVER_PORT 443
*/
/*
#define SERVER_HOST "a2c.gloverapp.co"
#define SERVER_PORT 443     //80 //8083 //3000    // 
#define SERVER_URL "/socket.io/?EIO=3&transaport=websocket&token=live-8015f403-36bd-4416-9df7-649d2847a990"
*/

//franscis demo server
#define SERVER_HOST "iothings.com.ng"
#define SERVER_PORT 443     //80 //8083 //3000    // 
#define SERVER_URL "/socket.io/?EIO=3&transaport=websocket&token=live-53478db4-d44c-47e0-8c29-cc1b05849b2a"




//
#define SSL_CERT "CEE86BCFF6D84D0786A89583A6A8431E0BF8D5D2AA64D8F995A9F81AF98CFB7F"  // for dev

#define SERVER_ADDR "wss://glover-a2c-server.private-gke.patriciadev.com/socket.io/?EIO=3&transport=websocket&token=live-9864b555-2c89-4eec-ab96-42ebd080ef74"

const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIEZTCCA02gAwIBAgIQQAF1BIMUpMghjISpDBbN3zANBgkqhkiG9w0BAQsFADA/
MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT
DkRTVCBSb290IENBIFgzMB4XDTIwMTAwNzE5MjE0MFoXDTIxMDkyOTE5MjE0MFow
MjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxCzAJBgNVBAMT
AlIzMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuwIVKMz2oJTTDxLs
jVWSw/iC8ZmmekKIp10mqrUrucVMsa+Oa/l1yKPXD0eUFFU1V4yeqKI5GfWCPEKp
Tm71O8Mu243AsFzzWTjn7c9p8FoLG77AlCQlh/o3cbMT5xys4Zvv2+Q7RVJFlqnB
U840yFLuta7tj95gcOKlVKu2bQ6XpUA0ayvTvGbrZjR8+muLj1cpmfgwF126cm/7
gcWt0oZYPRfH5wm78Sv3htzB2nFd1EbjzK0lwYi8YGd1ZrPxGPeiXOZT/zqItkel
/xMY6pgJdz+dU/nPAeX1pnAXFK9jpP+Zs5Od3FOnBv5IhR2haa4ldbsTzFID9e1R
oYvbFQIDAQABo4IBaDCCAWQwEgYDVR0TAQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8E
BAMCAYYwSwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5p
ZGVudHJ1c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTE
p7Gkeyxx+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEE
AYLfEwEBATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2Vu
Y3J5cHQub3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0
LmNvbS9EU1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYf
r52LFMLGMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjANBgkqhkiG9w0B
AQsFAAOCAQEA2UzgyfWEiDcx27sT4rP8i2tiEmxYt0l+PAK3qB8oYevO4C5z70kH
ejWEHx2taPDY/laBL21/WKZuNTYQHHPD5b1tXgHXbnL7KqC401dk5VvCadTQsvd8
S8MXjohyc9z9/G2948kLjmE6Flh9dDYrVYA9x2O+hEPGOaEOa1eePynBgPayvUfL
qjBstzLhWVQLGAkXXmNs+5ZnPBxzDJOLxhF2JIbeQAcH5H0tZrUlo5ZYyOqA7s9p
O5b85o3AM/OJ+CktFBQtfvBhcJVd9wvlwPsk+uyOy2HI7mNxKKgsBTt375teA2Tw
UdHkhVNcsAKX1H7GNNLOEADksd86wuoXvg==
-----END CERTIFICATE-----
)EOF";
//#define SERVER_URL "/sendData"
//#define SERVER_URL "/"