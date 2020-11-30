#ifndef LIB_H
#define LIB_H
#include <stdio.h>
#include <malloc.h>
#include <string.h>
typedef struct
{
    char str[50];
    char n[50];
} container;
typedef struct elm *alamatelmt;
typedef struct elm
{
    container kontainer;
    alamatelmt next;
} elemen;
typedef struct
{
    elemen *first;
    elemen *last;
} queue;

void copyContainer(container *dest, container *src);
void createEmpty(queue *Q);
int isEmpty(queue Q);
int countElement(queue Q);
void add(container src, queue *Q);
void addPriority(container src, int priority, queue *Q);
void transferElement(elemen *src, queue *srcQ, queue *destQ);
void del(queue *Q);
void printQueue(queue Q);

#endif
