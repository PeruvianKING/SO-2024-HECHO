
#include "list.h"
#include <string.h>

void createEmptyList(List *L)
{
    *L = NULL;
}

bool isEmptyList(List L)
{
    return (L == NULL);
}

Pos first(List L)
{
    return L;
}

Pos last(List L)
{
    Pos p;

    p = L;
    if (p == NULL)
        return NULL;
    while (p->next != NULL)
        p = p->next;
    return p;
}

Pos next(Pos p, List L)
{
    return p->next;
}

Pos previous(Pos p, List L)
{
    Pos q;
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

bool insertItem(Item d, List *L)
{
    Pos q, r;

    q = malloc(sizeof(*q));

    if (q == NULL)
        return false;

    q->data = d;
    q->next = NULL;

    if (*L == NULL)
    {
        q->data.posicion = 0;
        *L = q;
    }
    else
    {
        r = *L;
        while (r->next != NULL)
            r = r->next;

        q->data.posicion = r->data.posicion + 1;
        r->next = q;
        }
    return true;
}

void deleteAtPosition(Pos p, List *L)
{
    Pos q;

    if (p == *L)
        *L = (*L)->next;
    else if (p->next == NULL)
    {
        q = previous(p, *L);
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

Item getItem(Pos p, List L)
{
    return p->data;
}

void updateItem(Item d, Pos p, List *L)
{
    p->data = d;
}

Pos findItem(command d, List L)
{
    Pos p;

    for (p = L; (p != NULL) && (strcmp(p->data.name, d) < 0); p = p->next)
        ;
    if (p != NULL &&
        (strcmp(p->data.name, d) == 0))
        return p;
    else
        return NULL;
}

Pos findPosition(int posicion, List L)
{
    Pos p;

    p = L;
    if (p == NULL)
        return p;

    while (p->data.posicion != posicion && p->next != NULL)
        p = p->next;

    if (posicion == p->data.posicion)
        return p;
    else
        return NULL;
}

Pos findFileDescriptor(int df, List L)
{
    Pos p, temp;

    p = L;
    if (p == NULL)
        return p;

    while (p->data.fileDescriptor != df && p->next != NULL) // busca hasta encontrar el descriptor del fichero
        p = p->next;

    if (df == p->data.fileDescriptor)
        return p;

    else // si llega a la ultima posición y no contiene el descriptor de fichero que buscamos, devuelve -1
    {
        temp = malloc(sizeof(*temp)); // inicializamos temp para devolverlo, ya que si modificamos p estaríamos modificando nuestra lista
        temp->data.fileDescriptor = (-1);
        return temp;
    }
}