#ifndef ListPROCESOS_H_
#define ListPROCESOS_H_

#include <time.h>
#include <signal.h>
#include <stdbool.h>

#define COMMAND_LENGTH 2000
#define FINISHED 0
#define ACTIVE 1
#define SIGNAL 2
#define STOPPED 3

typedef struct ItemP
{
    int pid;
    struct tm *fecha;
    int status;
    char command[COMMAND_LENGTH];
} ItemP;

typedef struct NodeP *PosP;

struct NodeP
{
    PosP next;
    ItemP data;
};
typedef PosP ListP;

struct SEN
{
    char *nombre;
    int signal;
};

void createEmptyListP(ListP *L);

bool insertItemP(ItemP d, ListP *L);

void updateItemP(ItemP d, PosP p, ListP *L);

void deleteAtPositionP(PosP p, ListP *L);

bool isEmptyListP(ListP L);

PosP firstP(ListP L);

PosP lastP(ListP L);

PosP nextP(PosP p, ListP L);

PosP previousP(PosP p, ListP L);

ItemP getItemP(PosP p, ListP L);

PosP findItemP(int pid, ListP L);

#endif