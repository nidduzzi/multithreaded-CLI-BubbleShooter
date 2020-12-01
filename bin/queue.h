#ifndef LIB_H
#define LIB_H
#include <stdio.h>
#include <malloc.h>
#include <string.h>
typedef struct
{
    char str[50];
    char n[50];
} q_container;
typedef struct q_elm *q_elmtptr;
typedef struct q_elm
{
    q_container kontainer;
    q_elmtptr next;
} q_elemen;
typedef struct
{
    q_elemen *first;
    q_elemen *last;
} queue;

void copyContainer(q_container *dest, q_container *src);
void createEmptyQueue(queue *Q);
int isQueueEmpty(queue Q);
int countQueueElement(queue Q);
void addQelement(q_container src, queue *Q);
void addQelementPriority(q_container src, int priority, queue *Q);
void transferQelement(q_elemen *src, queue *srcQ, queue *destQ);
void delqueue(queue *Q);
void printQueue(queue Q);

#endif
