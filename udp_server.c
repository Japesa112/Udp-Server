#include "contiki.h"
#include "net/uip.h"
#include "net/simple-udp.h"
#include <stdio.h>

#define UDP_SERVER_PORT 1234

static struct simple_udp_connection udp_conn;

PROCESS(udp_server_process, "UDP Server");
AUTOSTART_PROCESSES(&udp_server_process);

static void udp_rx_callback(struct simple_udp_connection *c,
                             const uip_ipaddr_t *sender_addr,
                             uint16_t sender_port,
                             const uip_ipaddr_t *receiver_addr,
                             uint16_t receiver_port,
                             const uint8_t *data,
                             uint16_t datalen) {
  printf("Received request: '%.*s' from ", datalen, (char *)data);
  printf("Sending response back to %u\n", sender_port);
  
  // Send a response
  simple_udp_sendto(c, "ACK", strlen("ACK"), sender_addr);
}

PROCESS_THREAD(udp_server_process, ev, data) {
  PROCESS_BEGIN();
  
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL, UDP_SERVER_PORT, udp_rx_callback);
  
  while(1) {
    PROCESS_WAIT_EVENT();
  }
  
  PROCESS_END();
}
