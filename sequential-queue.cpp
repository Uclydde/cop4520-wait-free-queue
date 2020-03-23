// to compile:
// g++ sequential-queue.cpp

#include <stdio.h>
#include <stdlib.h>

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

  // adds an element to the queue. (inserts at tail)
  void push(Node* newNode)
  {
    if (newNode == NULL)
      return;

    printf("Pushed %d\n", newNode->val);

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
      Node* newTail = new Node(newNode->val);
      tail->next = newTail;
      tail = newTail;
      return;
    }
  }

  // returns the head of the queue
  Node* pop(void)
  {
    Node* temp;
    Node* newHead;

    if (head == NULL)
      return NULL;

    printf("Popped %d\n", head->val);

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
  Queue()
  {
    head = tail;
  }
};

// testing the sequential queue
int main(void)
{
  Node* temp;
  Queue* queue = new Queue();

  queue->push(new Node(1));
  queue->push(new Node(2));
  queue->push(new Node(3));

  temp = queue->pop();
  delete temp;
  temp = queue->pop();
  delete temp;
  temp = queue->pop();
  delete temp;

  return 0;
}
