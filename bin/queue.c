#include "queue.h"

void copyContainer(q_container *dest, q_container *src)
{
    strcpy(dest->str, src->str);
    strcpy(dest->n, src->n);
}

q_elemen *getPrev(q_elemen *ptr, queue *Q)
{
    q_elemen *cptr = Q->first, *prev = NULL;
    while (cptr != NULL)
    {
        if(cptr == ptr)
        {
            break;
        }
        prev = cptr;
        cptr = cptr->next;
    }
    return prev;
}

void createEmptyQueue(queue *Q)
{
    (*Q).first = NULL;
    (*Q).last = NULL;
}

int isQueueEmpty(queue Q)
{
    int hasil = 0;
    if (Q.first == NULL)
    {
        hasil = 1;
    }
    return hasil;
}

int countQueueElement(queue Q)
{
    int hasil = 0;
    if (Q.first != NULL)
    {
        /* queue tidak kosong */
        q_elemen *bantu;
        /* inisialisasi */
        bantu = Q.first;

        while (bantu != NULL)
        {
            /* proses */
            hasil = hasil + 1;

            /* iterasi */
            bantu = bantu->next;
        }
    }

    return hasil;
}

void addQelement(q_container src, queue *Q)
{
    q_elemen *baru;
    baru = (q_elemen *)malloc(sizeof(q_elemen));
    copyContainer(&(baru->kontainer), &(src));
    baru->next = NULL;
    if ((*Q).first == NULL)
    {
        (*Q).first = baru;
    }
    else
    {
        (*Q).last->next = baru;
    }
    (*Q).last = baru;
    baru = NULL;
}

void addQelementPriority(q_container src, int priority, queue *Q)
{
    q_elemen *ptr = Q->first, *prev = NULL;
    if (ptr == NULL || countQueueElement(*Q) < priority)
    {
        addQelement(src, Q);
    }
    else
    {
        int i = 1;
        while (ptr != NULL)
        {
            if (i == priority)
            {
                q_elemen *newPtr = (q_elemen *)malloc(sizeof(q_elemen));
                copyContainer(&(newPtr->kontainer), &(src));
                if(i == 1)
                {
                    Q->first = newPtr;
                }
                else
                {
                    prev->next = newPtr;
                }
                newPtr->next = ptr;
                break;
            }
            ++i;
            prev = ptr;
            ptr = ptr->next;
        }
    }
}

void transferQelement(q_elemen *src, queue *srcQ, queue *destQ)
{
    q_elemen *prev = getPrev(src, srcQ);
    if(prev == NULL)
    {
        if(src->next == NULL) srcQ->first = NULL;
        else srcQ->first = src->next;
    }
    else
    {
        if(src->next == NULL) prev->next = NULL;
        else prev->next = src->next;
    }
    if(destQ->last == NULL && destQ->first == NULL)
    {
        destQ->first = src;
    }
    else if(destQ->first != NULL)
    {
        destQ->last->next = src;
    }
    destQ->last = src;
    src->next = NULL;
}

void delqueue(queue *Q)
{
    if ((*Q).first != NULL)
    {
        /* jika queue bukan queue kosong */
        q_elemen *hapus = (*Q).first;
        if (countQueueElement(*Q) == 1)
        {
            (*Q).first = NULL;
            (*Q).last = NULL;
        }
        else
        {
            (*Q).first = (*Q).first->next;
            hapus->next = NULL;
        }
        free(hapus);
    }
}

void printQueue(queue Q)
{
    if (Q.first != NULL)
    {
        q_elemen *bantu = Q.first;
        int i = 1;
        while (bantu != NULL)
        {
            printf("%s ", bantu->kontainer.str);
            printf("%s\n", bantu->kontainer.n);
            /* iterasi */
            bantu = bantu->next;
            i = i + 1;
        }
        printf("******************\n");
    }
    else
    {
        /* proses jika queue kosong */
        printf("queue kosong\n");
    }
}
