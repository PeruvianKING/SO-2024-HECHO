#ifndef ListMMEMORIA_H_
#define ListMMEMORIA_H_

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>

#define COMMAND_LENGTH 2000

typedef char command[COMMAND_LENGTH];

typedef struct ItemM
{
    void *memoryAddress;
    int size;
    struct tm *hora;
    command mode;
    int key;
    int fd;
    command name;
    // int posicion;
} ItemM;

typedef struct NodeM *PosM;
struct NodeM
{
    PosM next;
    ItemM data;
};
typedef PosM ListM;

void createEmptyListM(ListM *L);

bool insertItemM(ItemM d, ListM *L);

void updateItemM(ItemM d, PosM p, ListM *L);

void deleteAtPositionM(PosM p, ListM *L);

bool isEmptyListM(ListM L);

PosM firstM(ListM L);

PosM lastM(ListM L);

PosM nextM(PosM p, ListM L);

PosM previousM(PosM p, ListM L);

ItemM getItemM(PosM p, ListM L);

PosM findItemM(command d, ListM L);

void clearListM(ListM *L);

void printListM(ListM L, char *modo);

// PosM findPositionM(int posicion, ListM L);

#endif