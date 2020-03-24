// to compile:
// g++ -I mrlock/src/ mrlock-queue.cpp mrlock/src/strategy/*.o --std=c++11 -pthread

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <thread>
#include <iostream>
#include <atomic>
#include <cmath>
#include <time.h>
#include <strategy/lockablebase.h>
#include <strategy/mrlockable.h>

#define NUM_THREADS 1
#define NUM_OPS 100

class Node
{
  public:
   int val;
   Node* next;

  Node(int newVal)
  {
    val = newVal;
  }
};

class Queue
{
  public:
  Node* head;
  Node* tail;

  ResourceAllocatorBase* resourceAlloc;
  LockableBase* lock;


  // adds an element to the queue. (inserts at tail)
  void enqueue(Node* newNode)
  {
    lock->Lock();

    if (newNode == NULL)
    {
      printf("Out of pre-allocated nodes. \n");
      lock->Unlock();
      return;
    }

    printf("Enqueued %d\n", newNode->val);

    // if queue is empty
    if (tail == NULL)
    {
      tail = newNode;
      head = tail;
      lock->Unlock();
      return;
    }
    // if queue is not empty
    else
    {
      Node* newTail = new Node(newNode->val);
      tail->next = newTail;
      tail = newTail;
      lock->Unlock();
      return;
    }
  }

  // returns the head of the queue
  Node* dequeue(void)
  {
    Node* temp;
    Node* newHead;

    lock->Lock();

    if (head == NULL)
    {
      printf("The queue is empty. No node was dequeued.\n");
      lock->Unlock();
      return NULL;
    }

    printf("Dequeued %d\n", head->val);

    // queue will now be empty
    if (head == tail)
    {
      temp = head;
      head = NULL;
      tail = NULL;
      lock->Unlock();
      return temp;
    }
    else
    {
      newHead = head->next;
      temp = head;
      head = newHead;
      lock->Unlock();
      return temp;
    }
  }

  Queue()
  {
    head = tail;
    resourceAlloc = new MRResourceAllocator(1);
    lock = resourceAlloc->CreateLockable({0});
  }
};


// each thread gets its own SimpleQueue, which contains pre-allocated nodes
class SimpleQueue
{
  public:
  Node* head;
  Node* tail;

  // adds an element to the queue. (inserts at tail)
  void push(Node* newNode)
  {
    if (newNode == NULL)
      return;

    // if queue is empty
    if (tail == NULL)
    {
      tail = newNode;
      head = tail;
      return;
    }
    // if queue is not empty
    else
    {
      tail->next = newNode;
      tail = tail->next;
      return;
    }
  }

  // returns the head of the queue
  Node* pop(void)
  {
    Node* temp;
    Node* newHead;

    if (head == NULL)
    {
      return NULL;
    }

    // queue will now be empty
    if (head == tail)
    {
      temp = head;
      head = NULL;
      tail = NULL;
      return temp;
    }
    else
    {
      newHead = head->next;
      temp = head;
      head = newHead;
      return temp;
    }
  }
  SimpleQueue()
  {
    head = tail;
  }
};

Queue* mrlockQueue = new Queue();


void executeRandomOps(SimpleQueue* preallocated)
{
  for (int i = 0; i < NUM_OPS; i++)
  {
    if (rand() % 2)
    {
      // enqueues a node from the thread's pre-allocated nodes
      mrlockQueue->enqueue(preallocated->pop());
    }
    else
    {
      // dequeues a node back onto the thread's pre-allocated nodes
      preallocated->push(mrlockQueue->dequeue());
    }
  }
}

void preallocateNodes(SimpleQueue* preallocated)
{
  for (int i = 0; i < NUM_OPS; i++)
  {
    preallocated->push(new Node(rand() % 100));
  }
}

int main(void)
{
  std::thread threads[NUM_THREADS];
  SimpleQueue* preallocated[NUM_THREADS];
  srand(time(0));

  // preallocate each thread's nodes
  for (int i = 0; i < NUM_THREADS; i++)
  {
    preallocated[i] = new SimpleQueue();
    preallocateNodes(preallocated[i]);
  }

  // start the threads
  for (int i = 0; i < NUM_THREADS; i++)
    threads[i] = std::thread(executeRandomOps, preallocated[i]);

  // ensure that all threads have finished before continuing
  for (int i = 0; i < NUM_THREADS; i++)
    threads[i].join();

  return 0;
}
