#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../gc-7.2/include/gc.h"
#include "../utils/hashTable.h"
#include "../utils/array.h"
#include "../protocol/protocol.h"
#include "initialize.h"
#include "event.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <stdint.h>

#define MAXMICRO 1E3
#define MINMICRO 1
#define MAXSMALL 1E8
#define MINSMALL 1E3
#define MAXMEDIUM 1E11
#define MINMEDIUM 1E8

long eventIndex;
Heap* events;
uint64_t simulatorTime;
Array *payments;

void initializeEvents(long nPayments, double paymentMean) {
  long i, senderID, receiverID;
  uint64_t  paymentAmount=0, eventTime=0 ;
  uint32_t nextEventInterval;
  unsigned int paymentClass;
  double paymentClassP[]= {0.65, 0.2, 0.1, 0.05}, randomDouble;
  gsl_ran_discrete_t* discrete;
  Payment *payment;
  Event* event;

  discrete = gsl_ran_discrete_preproc(4, paymentClassP);


  for(i=0;i<nPayments;i++) {


    do{
      senderID = gsl_rng_uniform_int(r,peerIndex);
      receiverID = gsl_rng_uniform_int(r, peerIndex);
    } while(senderID==receiverID);


    paymentClass = gsl_ran_discrete(r, discrete);
    randomDouble = gsl_rng_uniform(r);
    switch(paymentClass) {
    case 0:
      paymentAmount = randomDouble*gsl_pow_uint(10, gsl_rng_uniform_int(r, 3) + 1);
      break;
    case 1:
      paymentAmount = randomDouble*gsl_pow_uint(10, gsl_rng_uniform_int(r, 3) + 3);
       break;
    case 2:
      paymentAmount = randomDouble*gsl_pow_uint(10, gsl_rng_uniform_int(r, 3) + 6);
      break;
    case 3:
      paymentAmount = randomDouble*gsl_pow_uint(10, gsl_rng_uniform_int(r, 3) + 9);
      break;
    }
    payment = createPayment(paymentIndex, senderID, receiverID, paymentAmount);
    //hashTablePut(payments, payment->ID, payment);
    arrayInsert(payments, payment);

    nextEventInterval = 1000*gsl_ran_exponential(r, paymentMean);
    eventTime += nextEventInterval;
    event = createEvent(eventIndex, eventTime, FINDROUTE, senderID, payment->ID);
    events = heapInsert(events, event, compareEvent);
  }

}

void initializeEventsPreproc(long nPayments, double paymentMean) {
  long i, senderID, receiverID;
  uint64_t  paymentAmount=0, eventTime=0 ;
  uint32_t nextEventInterval;
  unsigned int paymentClass;
  double paymentClassP[]= {0.65, 0.2, 0.1, 0.05}, randomDouble;
  gsl_ran_discrete_t* discrete;
  long paymentIDIndex=0;

  csvPayment = fopen("payment.csv", "w");
  if(csvPayment==NULL) {
    printf("ERROR cannot open file payment.csv\n");
    return;
  }
  fprintf(csvPayment, "ID,Sender,Receiver,Amount,Time\n");


  discrete = gsl_ran_discrete_preproc(4, paymentClassP);


  for(i=0;i<nPayments;i++) {


    do{
      senderID = gsl_rng_uniform_int(r,peerIndex);
      receiverID = gsl_rng_uniform_int(r, peerIndex);
    } while(senderID==receiverID);


    paymentClass = gsl_ran_discrete(r, discrete);
    randomDouble = gsl_rng_uniform(r);
    switch(paymentClass) {
    case 0:
      paymentAmount = randomDouble*gsl_pow_uint(10, gsl_rng_uniform_int(r, 3) + 1);
      break;
    case 1:
      paymentAmount = randomDouble*gsl_pow_uint(10, gsl_rng_uniform_int(r, 3) + 3);
       break;
    case 2:
      paymentAmount = randomDouble*gsl_pow_uint(10, gsl_rng_uniform_int(r, 3) + 6);
      break;
    case 3:
      paymentAmount = randomDouble*gsl_pow_uint(10, gsl_rng_uniform_int(r, 3) + 9);
      break;
    }
    nextEventInterval = 1000*gsl_ran_exponential(r, paymentMean);
    eventTime += nextEventInterval;

    fprintf(csvPayment, "%ld,%ld,%ld,%ld,%ld\n", paymentIDIndex++, senderID, receiverID, paymentAmount, eventTime );

  }

  fclose(csvPayment);

  //printf("change payments and press enter\n");
  //scanf("%*c");

}

void createPaymentsFromCsv(unsigned int isPreproc) {
  Payment* payment;
  Event* event;
  char row[256], filePayment[64];
  long ID, sender, receiver;
  uint64_t amount, time;

  if(isPreproc)
    strcpy(filePayment, "payment.csv");
  else
    strcpy(filePayment, "payment_output.csv");

  csvPayment = fopen("payment.csv", "r");
  if(csvPayment==NULL) {
    printf("ERROR cannot open file payment.csv\n");
    return;
  }

  fgets(row, 256, csvPayment);
  while(fgets(row, 256, csvPayment) != NULL) {
    sscanf(row, "%ld,%ld,%ld,%ld,%ld", &ID, &sender, &receiver, &amount, &time);
    payment = createPayment(ID, sender, receiver, amount);
    //hashTablePut(payments, payment->ID, payment);
    arrayInsert(payments, payment);
    event = createEvent(eventIndex, time, FINDROUTE, sender, payment->ID);
    events = heapInsert(events, event, compareEvent);
  }

  fclose(csvPayment);
}

void initializeSimulatorData(long nPayments, double paymentMean, unsigned int isPreproc ) {
  eventIndex = 0;
  simulatorTime = 1;

  payments = arrayInitialize(nPayments);
  events = heapInitialize(nPayments*10);

  if(isPreproc)
    initializeEventsPreproc(nPayments, paymentMean);

  createPaymentsFromCsv(isPreproc);
}



/*
void initialize() {
  long i;
  Peer* peer;
  Channel* channel;
  ChannelInfo* channelInfo;
  

  gsl_rng *r;
  const gsl_rng_type * T;

  gsl_rng_env_setup();

  T = gsl_rng_default;
  r = gsl_rng_alloc (T);

  nPeers=5;
  nChannels=2;



  peers = hashTableInitialize(2);
  channels = hashTableInitialize(2);
  channelInfos= hashTableInitialize(2);


  for(i=0; i<nPeers; i++) {
    peer = createPeer(nChannels);
    hashTablePut(peers, peer->ID, peer);
  }


  long j, counterpartyID;
  Peer* counterparty;
  Policy policy;
  policy.feeBase=0.1;
  policy.timelock=5;
  policy.feeProportional=0.0;

  srand(time(NULL));
  for(i=0; i<nPeers; i++) {
    peer = arrayGet(peers, i);
    for(j=0; j<nChannels && (arrayLen(peer->channel) < nChannels); j++){


    do {
          counterpartyID = gsl_rng_uniform_int(r,nPeers);
      }while(counterpartyID==peer->ID);


      counterparty = arrayGet(peers, counterpartyID);
      if(arrayLen(counterparty->channel)>=nChannels) continue;

      channelInfo=createChannelInfo(peer->ID, counterparty->ID, 1.0);
      hashTablePut(channelInfos, channelInfo->ID,channelInfo);

      channel=createChannel(channelInfo->ID, counterparty->ID, policy);
      hashTablePut(channels, channel->ID, channel);
      peer->channel=arrayInsert(peer->channel, &(channel->ID));

      channel = createChannel(channelInfo->ID, peer->ID, policy);
      hashTablePut(channels, channel->ID, channel);
      counterparty->channel=arrayInsert(counterparty->channel, &(channel->ID));

    } 
  }

  long *currChannelID;
  for(i=0; i<nPeers; i++) {
    peer = arrayGet(peers, i);
    for(j=0; j<nChannels; j++) {
      currChannelID=arrayGet(peer->channel, j);
      if(currChannelID==NULL) continue;
      channel = arrayGet(channels, *currChannelID);
      printf("Peer %ld is connected to peer %ld through channel %ld\n", i, channel->counterparty, channel->ID);
    } 
  }

}
*/
