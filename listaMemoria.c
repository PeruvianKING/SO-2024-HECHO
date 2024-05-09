#include "listaMemoria.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void createEmptyListM(ListM *L)
{
    *L = NULL;
}

bool isEmptyListM(ListM L)
{
    return (L == NULL);
}

PosM firstM(ListM L)
{
    return L;
}

PosM lastM(ListM L)
{
    PosM p;

    p = L;
    if (p == NULL)
        return NULL;
    while (p->next != NULL)
        p = p->next;
    return p;
}

PosM nextM(PosM p, ListM L)
{
    return p->next;
}

PosM previousM(PosM p, ListM L)
{
    PosM q;
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

bool insertItemM(ItemM d, ListM *L)
{
    PosM q, r;

    q = malloc(sizeof(*q));

    if (q == NULL)
        return false;

    q->data = d;
    q->next = NULL;

    if (*L == NULL)
    {
        // q->data.posicion = 0;
        *L = q;
    }
    else
    {
        r = *L;
        while (r->next != NULL)
            r = r->next;

        // q->data.posicion = r->data.posicion + 1;
        r->next = q;
    }
    return true;
}

void deleteAtPositionM(PosM p, ListM *L)
{
    PosM q;

    if (p == *L)
        *L = (*L)->next;
    else if (p->next == NULL)
    {
        q = previousM(p, *L);
        q->next = NULL;
    }
    else
    {
        q = p->next;
        p->data = q->data;
        p->next = q->next;
        p = q;
    }
    if (!strcmp(p->data.mode, "shared"))
    {
        if (shmdt(p->data.memoryAddress))
            perror("No se pudo liberar: ");
    }
    if (!strcmp(p->data.mode, "malloc"))
        free(p->data.memoryAddress);
    if (!strcmp(p->data.mode, "mmaped"))
    {
        munmap(p->data.memoryAddress, p->data.size);
        close(p->data.fd);
    }

    free(p);
}

ItemM getItemM(PosM p, ListM L)
{
    return p->data;
}

void updateItemM(ItemM d, PosM p, ListM *L)
{
    p->data = d;
}

PosM findItemM(command d, ListM L)
{
    PosM p;

    for (p = L; (p != NULL) && (strcmp(p->data.memoryAddress, d) < 0); p = p->next)
        ;
    if (p != NULL &&
        (strcmp(p->data.memoryAddress, d) == 0))
        return p;
    else
        return NULL;
}

void clearListM(ListM *L)
{
    PosM p, temp;
    p = *L;

    if (*L == NULL)
        return;

    while (p != NULL)
    {
        temp = p;
        p = p->next;

        deleteAtPositionM(temp, L);
    }
    createEmptyListM(L);
}

void printListM(ListM L, char *modo)
{
    PosM p;
    bool mmap = false, malloc = false, shared = false, all = false;
    const char *month[12] = {
        "Jan", "Feb", "Mar", "Apr",
        "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec"};

    if (!strcmp("malloc", modo))
        malloc = true;
    else if (!strcmp("mmaped", modo))
        mmap = true;
    else if (!strcmp("shared", modo))
        shared = true;
    else
        all = true;

    printf("******Lista de bloques asignados %s para el proceso %d\n", modo, getpid());

    p = L;

    if (malloc)
    {
        while (p != NULL)
        {
            if (!strcmp(p->data.mode, modo))
                printf(" %10p%15d%5s%5d:%d %.15s\n", p->data.memoryAddress, p->data.size, month[p->data.hora->tm_mon], p->data.hora->tm_hour, p->data.hora->tm_min, modo);
            p = p->next;
        }
    }
    else if (mmap)
    {
        while (p != NULL)
        {
            if (!strcmp(p->data.mode, modo))
                printf(" %10p%15d%5s%5d:%d%15s  (descriptor %d)\n", p->data.memoryAddress, p->data.size, month[p->data.hora->tm_mon], p->data.hora->tm_hour, p->data.hora->tm_min, p->data.name, p->data.fd);
            p = p->next;
        }
    }
    if (shared)
    {
        while (p != NULL)
        {
            if (!strcmp(p->data.mode, modo))
                printf(" %10p%15d%5s%5d:%d%5s shared  (key %d)\n", p->data.memoryAddress, p->data.size, month[p->data.hora->tm_mon], p->data.hora->tm_hour, p->data.hora->tm_min, p->data.name, p->data.key);
            p = p->next;
        }
    }
    else if (all)
    {
        while (p != NULL)
        {
            if (!strcmp(p->data.mode, "shared"))
                printf(" %10p%15d%5s%5d:%d%5s shared  (key %d)\n", p->data.memoryAddress, p->data.size, month[p->data.hora->tm_mon], p->data.hora->tm_hour, p->data.hora->tm_min, p->data.name, p->data.key);
            if (!strcmp(p->data.mode, "malloc"))
                printf(" %10p%15d%5s%5d:%d%12s\n", p->data.memoryAddress, p->data.size, month[p->data.hora->tm_mon], p->data.hora->tm_hour, p->data.hora->tm_min, "malloc");
            if (!strcmp(p->data.mode, "mmaped"))
                printf(" %10p%15d%5s%5d:%d%12s  (descriptor %d)\n", p->data.memoryAddress, p->data.size, month[p->data.hora->tm_mon], p->data.hora->tm_hour, p->data.hora->tm_min, p->data.name, p->data.fd);
            p = p->next;
        }
    }
}

/*PosM findPositionM(int posicion, ListM L)
{
    PosM p;

    p = L;
    if (p == NULL)
        return p;

    while (p->data.posicion != posicion && p->next != NULL)
        p = p->next;

    if (posicion == p->data.posicion)
        return p;
    else
        return NULL;
}*/