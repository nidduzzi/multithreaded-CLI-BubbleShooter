
#include "lib.h"
void createList(list *L)
{
    (*L).first = NULL;//initialize the first pointer
}

int countElement(list L)
{
    int hasil = 0;

    if(L.first !=NULL)
    {
        /*list tidak kosong*/
        elemen* tunjuk;
        /*inisialisasi*/
        tunjuk = L.first;

        while(tunjuk != NULL){
            /*proses*/
            hasil = hasil + 1;

            /*iterasi*/
            tunjuk = tunjuk->next;
        }

    }

    return hasil;
}

void addFirst(bubble_t src, list *L)
{
    elemen* baru;
    baru = (elemen*) malloc (sizeof (elemen));
    copyElement(&(baru->kontainer), &src);//copy the input to the new element

    if((*L).first == NULL)
    {
        baru->next = NULL;
    }
    else
    {
        baru->next = (*L).first;
    }
    (*L).first = baru;
    baru = NULL;
}

void addAfter(elemen* prev, bubble_t src, list *L)
{
    elemen *baru;
    baru = (elemen*) malloc (sizeof (elemen));
    copyElement(&(baru->kontainer), &src);//copy the input to the new element

    if(prev->next == NULL){
        baru->next = NULL;
    }
    else
    {
        baru->next = prev->next;
    }
    prev->next = baru;
    baru = NULL;
}

void addLast(bubble_t src, list *L)
{
    if((*L).first == NULL)
    {
        /*jika list adalah list kosong*/
        addFirst(src, L);
    }
    else
    {
        /*jika list tidak kosong
        mencari elemen terakhir list*/
        elemen* prev = (*L).first;
        while(prev->next != NULL)
        {
            /*iterasi*/
            prev = prev->next;
        }
        addAfter(prev, src, L);
    }
}

void delFirst(list *L)
{
    if((*L).first != NULL)
    {
        /*jika list bukan list kosong*/
        elemen* hapus = (*L).first;
        if(countElement(*L) == 1)
        {
            (*L).first = NULL;
        }
        else
        {
            (*L).first = (*L).first->next;
            hapus->next = NULL;
        }
        free(hapus);
    }
}

void delAfter(elemen* prev, list *L)
{
    elemen* hapus = prev->next;
    if(hapus != NULL)
    {
        if(hapus->next == NULL)
        {
            prev->next = NULL;
        }
        else
        {
            prev->next = hapus->next;
            hapus->next = NULL;
        }
        free(hapus);
    }
}

void delLast(list *L)
{
    if((*L).first != NULL)
    {
        /*jika list tidak kosong*/
        if(countElement(*L) == 1)
        {
            /*list terdiri dari satu elemen*/
            delFirst(L);
            }
            else{
            /*mencari elemen terakhir list*/
            elemen* last = (*L).first;
            elemen* prev;

            while(last->next != NULL)
            {
                /*iterasi*/
                prev = last;
                last = last->next;
            }
            delAfter(prev, L);
        }
    }
}

void delAll(list *L)
{

    if(countElement(*L) != 0)
    {
        int i;
        for(i=countElement(*L);i>=1;i--)
        {
            /*proses menghapus elemen list*/
            delLast(L);
        }
    }
}

