#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

#define COMMAND_LENGTH 2000

typedef char command[COMMAND_LENGTH];

typedef struct Item
{
    command name;
    int fileDescriptor;
    int posicion;
    int mode;
} Item;

typedef struct Node *Pos;
struct Node
{
    Pos next;
    Item data;
};
typedef Pos List;

void createEmptyList(List *L);

bool insertItem(Item d, List *L);

void updateItem(Item d, Pos p, List *L);

void deleteAtPosition(Pos p, List *L);

bool isEmptyList(List L);

Pos first(List L);

Pos last(List L);

Pos next(Pos p, List L);

Pos previous(Pos p, List L);

Item getItem(Pos p, List L);

Pos findItem(command d, List L);

Pos findPosition(int posicion, List L);

Pos findFileDescriptor(int df, List L);

#endif