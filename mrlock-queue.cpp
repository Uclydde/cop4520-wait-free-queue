// to compile:
// g++ -std=c++11 -pthread mrlock-queue.cpp

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

#define NUM_THREADS 4
#define NUM_OPS 10

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
  void push(Node* newNode)
  {
    if (newNode == NULL)
      return;

    lock->Lock();

    printf("Pushed %d\n", newNode->val);

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
  Node* pop(void)
  {
    Node* temp;
    Node* newHead;

    lock->Lock();

    if (head == NULL)
    {
      lock->Unlock();
      return NULL;
    }

    printf("Popped %d\n", head->val);

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

Queue* mrlockQueue = new Queue();

void executeRandomOps()
{
  for (int i = 0; i < NUM_OPS; i++)
  {
    if (rand() % 2)
    {
      mrlockQueue->push(new Node(0));
    }
    else
    {
      mrlockQueue->pop();
    }
  }
}

int main(void)
{
  srand(time(0));
  std::thread threads[NUM_THREADS];

  // start the threads
  for (int i = 0; i < NUM_THREADS; i++)
    threads[i] = std::thread(executeRandomOps);

  // ensure that all threads have finished before continuing
  for (int i = 0; i < NUM_THREADS; i++)
    threads[i].join();

  return 0;
}
