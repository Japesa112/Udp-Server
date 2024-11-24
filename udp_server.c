#include "contiki.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/rime/rime.h"

#define MAX_RETRIES 3
#define ACK_WAIT_TIME (CLOCK_SECOND / 2)

PROCESS(interference_process, "Interference Generator");
AUTOSTART_PROCESSES(&interference_process);

static struct etimer timer;
static int retries;

static void ack_recv(struct unicast_conn *c, const linkaddr_t *from) {
  // Acknowledgment received; reset retry counter
  retries = 0;
}

static const struct unicast_callbacks unicast_callbacks = {ack_recv};
static struct unicast_conn uc;

PROCESS_THREAD(interference_process, ev, data) {
  PROCESS_EXITHANDLER(unicast_close(&uc);)
  
  PROCESS_BEGIN();
  
  // Initialize unicast connection
  unicast_open(&uc, 144, &unicast_callbacks);
  
  // Generate interference every 100ms
  etimer_set(&timer, CLOCK_SECOND / 10);
  
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    
    // Prepare and send packet
    packetbuf_copyfrom("NOISE", 5); // Dummy packet
    
    retries = 0; // Reset retry count
    while (retries < MAX_RETRIES) {
      if (unicast_send(&uc, NULL) == NETSTACK_MAC_TX_OK) {
        // Wait for ACK
        etimer_set(&timer, ACK_WAIT_TIME);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer) || ev == PROCESS_EVENT_MSG);
        
        if (ev == PROCESS_EVENT_MSG) {
          // ACK received
          break;
        }
      }
      retries++;
      etimer_set(&timer, CLOCK_SECOND / 10); // Wait before next attempt
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    }
    
    etimer_reset(&timer);
  }
  
  PROCESS_END();
}
