#ifndef PROCESOS_H_
#define PROCESOS_H_

#include "listaProcesos.h"
#include "procesos.h"
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "memoria.h"
#include <sys/resource.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAXVAR 256
extern char **environ;

void uidGET();
void uidSetName(char *trozos);
void uidSetID(char *id);
void showEveryVarENV(char *env[]);
int BuscarVariable(char *var, char *e[]);
void showvar(char *var, char *env[]);
void changevar(char *argumentos, char *env[]);
void subsvar(char *argumentos, char *env[]);
void showenvADDR(char *env[]);
void showEveryVarENVIRON();
void Cmd_fork();
void exec(char *argumentos);
void defaultExec(char *argumentos);
void defaultExecBACKGROUND(char *argumentos, ListP *list);
void defaultElegir(char *argumentos, ListP *listaProcesos);
void clearListP(ListP *L);
void refreshList(ListP *list);
void printListP(ListP *list);
void printOne(ListP *list, pid_t pid);
void fg(ListP *list, char *argumentos);
void del(ListP *list, int status);
#endif