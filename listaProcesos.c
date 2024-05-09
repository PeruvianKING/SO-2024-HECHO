#include "listaProcesos.h"
#include <stdlib.h>

void createEmptyListP(ListP *L)
{
    *L = NULL;
}

bool isEmptyListP(ListP L)
{
    return (L == NULL);
}

PosP firstP(ListP L)
{
    return L;
}

PosP lastP(ListP L)
{
    PosP p;

    p = L;
    if (p == NULL)
        return NULL;
    while (p->next != NULL)
        p = p->next;
    return p;
}

PosP nextP(PosP p, ListP L)
{
    return p->next;
}

PosP previousP(PosP p, ListP L)
{
    PosP q;
    if (p == L)
        return NULL;
    else
    {
        q = L;
        while (q->next != p)
            q = q->next;

        return q;
    }
}

bool insertItemP(ItemP d, ListP *L)
{
    PosP q, r;

    q = malloc(sizeof(*q));

    if (q == NULL)
        return false;

    q->data = d;
    q->next = NULL;

    r = *L;
    if (r == NULL)
        (*L) = q;
    else
    {
        while (r->next != NULL)
            r = r->next;

        r->next = q;
    }

    return true;
}

void deleteAtPositionP(PosP p, ListP *L)
{
    PosP q;

    if (p == *L)
        *L = (*L)->next;
    else if (p->next == NULL)
    {
        q = previousP(p, *L);
        q->next = NULL;
    }
    else
    {
        q = p->next;
        p->data = q->data;
        p->next = q->next;
        p = q;
    }
    free(p);
}

ItemP getItemP(PosP p, ListP L)
{
    return p->data;
}

void updateItemP(ItemP d, PosP p, ListP *L)
{
    p->data = d;
}

PosP findItemP(int pid, ListP L)
{
    PosP p;

    for (p = L; (p != NULL); p = p->next)
        ;

    if (p != NULL && (p->data.pid == pid))
        return p;
    else
        return NULL;
}


