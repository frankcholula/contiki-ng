#include "contiki.h"
#include "net/rime.h"

#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
{
  // Print the received message
  char *dataReceived = (char *)packetbuf_dataptr();
  dataReceived[packetbuf_datalen()] = 0;
  printf("A unicast received from %d.%d\n",from->u8[0],from->u8[1]);
  printf("The message is: '%s'\n",dataReceived);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)

  PROCESS_BEGIN();

  printf("I'm a client mote, my rime addr is: %d.%d\n",
           rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[1]);
  unicast_open(&uc, 146, &unicast_callbacks);

  static int n = 1;
  static rimeaddr_t addr;

  addr.u8[0] = 1;  // this is the server rime address (part 1)
  addr.u8[1] = 0;  // this is the server rime address (part 2)

  while(1) 
  {
    static struct etimer et;

    etimer_set(&et, CLOCK_SECOND*5/7);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    char message[100];
    sprintf(message,"Hello, I'm node %d.%d, This is message-%d",
           rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[1],n++);

    packetbuf_copyfrom(message, strlen(message));
    unicast_send(&uc, &addr);
    printf("A packet is sent to %d.%d.\n", addr.u8[0],addr.u8[1]);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
