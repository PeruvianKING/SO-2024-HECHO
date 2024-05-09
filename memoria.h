#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include "listaMemoria.h"
#include <errno.h>
#include <sys/shm.h>

#define TAMANO 2048

#define TAMANO 2048

void mallocMemoria(int size, ListM *list);
void freeMalloc(char *modo, ListM *list, int size);
void Do_MemPmap(void);
void *ObtenerMemoriaShmget(key_t clave, size_t tam, ListM *M);
void SharedCreate(char *trozos, ListM *M);
void ShareFree(int key, ListM *M);
void SharedExistent(char *arguments, ListM *M);
void SharedDelkey(char *args);
void CmdMmap(char *argument, char *permisos, ListM *L);
void *MapearFichero(char *fichero, int protection, ListM *L);
void mmapFree(char *fichero, ListM *L);
int LeerFichero(char *f, char *p, command argumentos);
int EscribirFichero(command argumentos);
int trocearCadena(command cadenaT, char *trozos[]);
void LlenarMemoria(command argumentos);
void memDump(command argumentos);
void Recursiva(int n);
void memVars();
void memFuncs();

#endif