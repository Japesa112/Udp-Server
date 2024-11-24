#include "contiki.h"
#include "net/netstack.h"
#include "net/ip/uip.h"
#include "sys/etimer.h"
#include "net/udp/udp.h"
#include <stdio.h>

#define UDP_PORT 1234
#define MAX_PAYLOAD_LEN  255

static struct udp_conn *server_conn;

PROCESS(udp_server_process, "UDP Server Process");
AUTOSTART_PROCESSES(&udp_server_process);

static void udp_receive_callback(struct udp_conn *c, const uip_ipaddr_t *sender_addr,
                                  uint16_t sender_port, uint8_t *data, uint16_t len) {
    printf("Received message: '%s' from %d.%d.%d.%d\n", data,
           sender_addr->u8[0], sender_addr->u8[1], sender_addr->u8[2], sender_addr->u8[3]);

    // Send ACK
    udp_packetbuf_clear();
    char ack[] = "ACK";
    udp_packetbuf_copyfrom(ack, sizeof(ack));
    udp_sendto(c, sender_addr, sender_port);
}

PROCESS_THREAD(udp_server_process, ev, data) {
    PROCESS_BEGIN();

    // Set up the UDP connection
    server_conn = udp_new(NULL, UIP_HTONS(UDP_PORT), udp_receive_callback);
    udp_bind(server_conn, UIP_HTONS(UDP_PORT));

    printf("UDP server started, waiting for data...\n");

    // Main loop for the server to handle incoming packets
    while(1) {
        PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}
