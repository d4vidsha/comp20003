/* This is used for printing the tree. It is an implementation of an abstract 
    data structure called a queue, which we covered last week. */
struct llqueue;

/* Creates a new, empty queue. */
struct llqueue *newQueue();

/* Adds the given item to the queue, allocating if needed. */
void queue(struct llqueue **queue, void *item);

/* Takes the next item from the queue. */
void *dequeue(struct llqueue *queue);

/* Returns 1 if the queue is empty, 0 otherwise. */
int empty(struct llqueue *queue);
