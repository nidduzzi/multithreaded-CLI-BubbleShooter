#include "queue.h"

void copyContainer(container *dest, container *src)
{
    strcpy(dest->str, src->str);
    strcpy(dest->n, src->n);
}

elemen *getPrev(elemen *ptr, queue *Q)
{
    elemen *cptr = Q->first, *prev = NULL;
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

void createEmpty(queue *Q)
{
    (*Q).first = NULL;
    (*Q).last = NULL;
}

int isEmpty(queue Q)
{
    int hasil = 0;
    if (Q.first == NULL)
    {
        hasil = 1;
    }
    return hasil;
}

int countElement(queue Q)
{
    int hasil = 0;
    if (Q.first != NULL)
    {
        /* queue tidak kosong */
        elemen *bantu;
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

void add(container src, queue *Q)
{
    elemen *baru;
    baru = (elemen *)malloc(sizeof(elemen));
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

void addPriority(container src, int priority, queue *Q)
{
    elemen *ptr = Q->first, *prev = NULL;
    if (ptr == NULL || countElement(*Q) < priority)
    {
        add(src, Q);
    }
    else
    {
        int i = 1;
        while (ptr != NULL)
        {
            if (i == priority)
            {
                elemen *newPtr = (elemen *)malloc(sizeof(elemen));
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

void transferElement(elemen *src, queue *srcQ, queue *destQ)
{
    elemen *prev = getPrev(src, srcQ);
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

void del(queue *Q)
{
    if ((*Q).first != NULL)
    {
        /* jika queue bukan queue kosong */
        elemen *hapus = (*Q).first;
        if (countElement(*Q) == 1)
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
        elemen *bantu = Q.first;
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
