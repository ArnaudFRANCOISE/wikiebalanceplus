/*
This code sends 2 types of messages to Z1 :
  - Identification messages
  - Data from the rapsberry

authors : MAHRAZ Anass and ROBYNS Jonathan
 */



#include "contiki.h"
#include "dev/serial-line.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include <stdio.h>
#include <stdlib.h>

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

static struct simple_udp_connection udp_conn;

int check=0;    // If check=0, we are in identification mode
struct ctimer identification_timer;    // If this timer ends, we set check=0
/*---------------------------------------------------------------------------*/
PROCESS(identification, "Identification");
PROCESS(check_sonde, "Check Sonde");
PROCESS(serial_process, "Serial process");
AUTOSTART_PROCESSES(&identification,&check_sonde,&serial_process);
/*---------------------------------------------------------------------------*/
static void timeout(void *ptr) {
  check=0;
}

/*---------------------------------------------------------------------------*/

static void
my_udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
/*
c : our udp connection
sender_addr : sender IP address
sender_port : the port the sender uses to send the message
receiver_addr : our IP adress
receiver_port : the port the we use to get the message
data : the message
datalen : message length

This fonction is called when we get a message
*/
  ctimer_set(&identification_timer, 85*CLOCK_SECOND, timeout, NULL);   // If we get a message from Z1, we reset the identification timer
  LOG_INFO("Received demande '%.*s' from the user\n", datalen, (char *) data);
  printf("%s\n", (char *) data);    // Print to send a message to the raspberry

  if (data[0]=='O' && data[1]=='K') {
    check=1;    // Exit the identification mode
  }
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");
}

/*---------------------------------------------------------------------------*/

static void my_read_sensor(int sensor_id) {
/*
sensor_id : sensor number

This funtion asks the raspberry to read a sensor with the id sensor_id
*/
  printf("{\"ID\":%d,\"T\":0,\"O\":1}\n",sensor_id);
}

/*---------------------------------------------------------------------------*/

static int size_message;
static void * my_create_message(int table_size,int *table){
/*
table_size : size of the table
table : table

This funtion returns a message which is the table with "Z" + "table size" at the beginning
*/                                          
  unsigned char * message;
  size_message=table_size*sizeof(int)+2;
  message=malloc(size_message);                                                        
  message[0]='Z';
  message[1]=(unsigned char) table_size;
  for (int i=0; i<table_size;i++) {
    message[i+2]=table[i];
 }
  return ((void*) message);
}

/*---------------------------------------------------------------------------*/ 
PROCESS_THREAD(identification, ev, data)
{
/*
This process sends identification messages to the root (Z1)
*/
  uip_ipaddr_t dest_ipaddr;  // root IP address
  static struct etimer d_t;
  int tab[5]={1,24,102,144,182};  // sensors ID (mock data)

  void *message_ids;

  PROCESS_BEGIN();
  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL, UDP_SERVER_PORT, my_udp_rx_callback);

  while(1) {
    while(check==0) {
      if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr) && check==0) {
        /* Send to DAG root (Z1) */
        message_ids=my_create_message(5,tab);
        LOG_INFO("Sending ID message to ");
        LOG_INFO_6ADDR(&dest_ipaddr);
        LOG_INFO_("\n");
        simple_udp_sendto(&udp_conn, message_ids, size_message, &dest_ipaddr);     // Fonction that sends the message
      } else {
        LOG_INFO("Not reachable yet\n");
      }
      etimer_set(&d_t, 10*CLOCK_SECOND);    // 10 seconds timer between each ID message
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&d_t));
    }
    PROCESS_PAUSE();
 }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(check_sonde, ev, data)
{
/*
This process simulates sensor readings
*/
  static struct etimer reading_sensor_timer;
  PROCESS_BEGIN();

  etimer_set(&reading_sensor_timer, 20*CLOCK_SECOND);    // 20 seconds timer between each sensor reading

  while(1) {
    PROCESS_WAIT_UNTIL(etimer_expired(&reading_sensor_timer));
    if (check==1) {
      my_read_sensor(1);
    }
    etimer_reset(&reading_sensor_timer);
    PROCESS_WAIT_UNTIL(etimer_expired(&reading_sensor_timer));
    if (check==1) {
      my_read_sensor(24);
    }
    etimer_reset(&reading_sensor_timer);
    PROCESS_WAIT_UNTIL(etimer_expired(&reading_sensor_timer));
    if (check==1) {
      my_read_sensor(102);
    }
    etimer_reset(&reading_sensor_timer);
    PROCESS_WAIT_UNTIL(etimer_expired(&reading_sensor_timer));
    if (check==1) {
      my_read_sensor(144);
    }
    etimer_reset(&reading_sensor_timer);
    PROCESS_WAIT_UNTIL(etimer_expired(&reading_sensor_timer));
    if (check==1) {
 my_read_sensor(182);
    }
    etimer_reset(&reading_sensor_timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(serial_process, ev, data)
{
/*
This process sends the data from the sensors to the root (Z1)
*/
  static char message_to_Z1[256];
  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();

  while(1) {
    PROCESS_YIELD();
    if (ev==serial_line_event_message && data != NULL) {    // If the event is from the serial port, we send the data to the root
        if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
          /* Send to DAG root */
          LOG_INFO("Sending data to ");
          LOG_INFO_6ADDR(&dest_ipaddr);
          LOG_INFO_("\n");
          snprintf(message_to_Z1, sizeof(message_to_Z1), "%s", (char *) data);
          simple_udp_sendto(&udp_conn, message_to_Z1, strlen(message_to_Z1)+1 , &dest_ipaddr);
        } else {
          LOG_INFO("Not reachable yet\n");
        }
    }
  }

  PROCESS_END();
}
