//#include <Dhcp.h>
//#include <Dns.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
//#include <Twitter.h>
#include <util.h>

#include "parser.h"
#include "rec868.h"
#include <Wire.h>

#define BUFLEN 64

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 178, 50);

uint16_t localUdpPort(8888);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);
EthernetUDP udp;

char cmd[BUFLEN];
char output[BUFLEN];
uint16_t len;
uint16_t volatile udp_count = 0;
extern volatile struct rec868_global_t rec868_global;

void setup() {
  // put your setup code here, to run once:
  // start the Ethernet connection and the server:
  rec868_init();
  Ethernet.begin(mac, ip);
  server.begin();
  udp.begin(localUdpPort);
}

void loop() {
  // put your main code here, to run repeatedly:
    // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    uint8_t pos = 0;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        cmd[pos++] = c;
        if (c == '\n') {
          cmd[pos++] = '\0'; //Stringende anhängen
          uint16_t len = parser(cmd, output, BUFLEN);
          output[len + 1] = '\n';
          client.println(output);
          break;
        }
      }
    }
    delay(1);
    client.stop();
  }
  rec868_process();
}
