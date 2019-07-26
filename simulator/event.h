#ifndef EVENT_H
#define EVENT_H
#include "../utils/heap.h"
#include <stdint.h>

/*
typedef struct payment {
  Peer sender;
  Peer receiver;
  double amount;
  List route;
} Payment;
*/

extern long event_index;
extern Heap* events;

typedef enum event_type {
  FINDROUTE,
  SENDPAYMENT,
  FORWARDPAYMENT,
  RECEIVEPAYMENT,
  FORWARDSUCCESS,
  FORWARDFAIL,
  RECEIVESUCCESS,
  RECEIVEFAIL
} Event_type;

typedef struct event {
  long id;
  uint64_t time;
  Event_type type;
  long peer_id;
  long payment_id;
} Event;

Event* create_event(long id, uint64_t time, Event_type type, long peer_id, long payment_id);

int compare_event(Event* e1, Event *e2);

void print_event(Event*e);


#endif
