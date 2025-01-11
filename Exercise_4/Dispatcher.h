#pragma once
#include "Producer.h"
class Dispatcher
{
private:
    BoundedQueue *S_dispatcher_queue;
    BoundedQueue *N_dispatcher_queue;
    BoundedQueue *W_dispatcher_queue;
    BoundedQueue *ready;
    int numOfProducers;
    //pointer to vector of pointers to the producers queue
    std::vector<Producer*>* producerVector;


Dispatcher::Dispatcher(BoundedQueue *S_dispatcher_queue, BoundedQueue *N_dispatcher_queue, 
                       BoundedQueue *W_dispatcher_queue, std::vector<Producer*>* producerVector, int numOfProducers)
    : S_dispatcher_queue(S_dispatcher_queue), N_dispatcher_queue(N_dispatcher_queue), 
      W_dispatcher_queue(W_dispatcher_queue), producerVector(producerVector), numOfProducers(numOfProducers) { }


Dispatcher::
