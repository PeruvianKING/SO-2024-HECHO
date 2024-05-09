#include "memoria.h"
#include "listaMemoria.h"
#include <stdio.h>

int *a, *b, *c, *d, *e, *f;

int trocearCadena(command cadenaT, char *trozos[])
{
    int i = 1;

    if ((trozos[0] = strtok(cadenaT, " \n\t")) == NULL)
        return 0;
    while ((trozos[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

void mallocMemoria(int size, ListM *list)
{
    ItemM item;
    struct tm *fecha;
    time_t tiempo;

    time(&tiempo);
    fecha = localtime(&tiempo);

    item.memoryAddress = malloc(size);
    if (item.memoryAddress == NULL)
    {
        perror("No se pudo reservar memoria");
        return;
    }

    item.size = size;
    strcpy(item.mode, "malloc");
    item.hora = fecha;
    item.fd = -1;
    item.key = -1;
    strcpy(item.name, "");

    insertItemM(item, list);
    printf("Asignados %d bytes en %p\n", size, item.memoryAddress);
}

void freeMalloc(char *modo, ListM *list, int size)
{
    PosM p;

    p = *list;

    while (p != NULL)
    {
        if (p->data.size == size && !strcmp(p->data.mode, "malloc"))
        {
            deleteAtPositionM(p, list);
            return;
        }
        p = p->next;
    }
    printf("No hay bloque de ese tamaño\n");
}

void Do_MemPmap(void) /*sin argumentos*/
{
    pid_t pid; /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *argv[4] = {"pmap", elpid, NULL};

    sprintf(elpid, "%d", (int)getpid());
    if ((pid = fork()) == -1)
    {
        perror("Imposible crear proceso");
        return;
    }
    if (pid == 0)
    { /*proceso hijo*/
        if (execvp(argv[0], argv) == -1)
            perror("cannot execute pmap (linux, solaris)");

        argv[0] = "vmmap";
        argv[1] = "-interleave";
        argv[2] = elpid;
        argv[3] = NULL;
        if (execvp(argv[0], argv) == -1) /*probamos vmmap Mac-OS*/
            perror("cannot execute vmmap (Mac-OS)");

        argv[0] = "procstat";
        argv[1] = "vm";
        argv[2] = elpid;
        argv[3] = NULL;
        if (execvp(argv[0], argv) == -1) /*No hay pmap, probamos procstat FreeBSD */
            perror("cannot execute procstat (FreeBSD)");

        argv[0] = "procmap", argv[1] = elpid;
        argv[2] = NULL;
        if (execvp(argv[0], argv) == -1) /*probamos procmap OpenBSD*/
            perror("cannot execute procmap (OpenBSD)");

        exit(1);
    }
    waitpid(pid, NULL, 0);
}

void *ObtenerMemoriaShmget(key_t clave, size_t tam, ListM *M)
{
    void *p;
    int aux, id, flags = 0777;
    struct shmid_ds s;
    ItemM item;
    struct tm *fecha;
    time_t tiempo;

    time(&tiempo);
    fecha = localtime(&tiempo);

    if (tam) /*tam distito de 0 indica crear */
        flags = flags | IPC_CREAT | IPC_EXCL;
    if (clave == IPC_PRIVATE) /*no nos vale*/
    {
        errno = EINVAL;
        return NULL;
    }
    if ((id = shmget(clave, tam, flags)) == -1)
        return (NULL);
    if ((p = shmat(id, NULL, 0)) == (void *)-1)
    {
        aux = errno;
        if (tam)
            shmctl(id, IPC_RMID, NULL);
        errno = aux;
        return (NULL);
    }
    shmctl(id, IPC_STAT, &s);

    item.key = clave;
    item.size = s.shm_segsz;
    item.fd = -1;
    item.hora = fecha;
    item.memoryAddress = p;
    strcpy(item.mode, "shared");
    strcpy(item.name, "");
    insertItemM(item, M);
    /** Guardar en la lista, p.e.  InsertarNodoShared (&L, p, s.shm_segsz, clave); */
    return (p);
}

void SharedExistent(char *arguments, ListM *M)
{
    key_t cl;
    size_t tam;
    void *p;
    char *tr[COMMAND_LENGTH];

    trocearCadena(arguments, tr);

    cl = (key_t)strtoul(tr[1], NULL, 10);
    tam = 0;

    if ((p = ObtenerMemoriaShmget(cl, tam, M)) != NULL)
        printf("Memoria compartida de clave %lu en %p\n", (unsigned long)cl, p);
    else
        printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long)cl, strerror(errno));
}

void ShareFree(int key, ListM *M)
{
    PosM pos = *M;

    while (pos->data.key != key)
        pos = pos->next;

    deleteAtPositionM(pos, M);
}

void SharedDelkey(char *args)
{
    key_t clave;
    int id;
    char *key, *trozos[COMMAND_LENGTH];

    trocearCadena(args, trozos);

    key = trozos[2];

    if (key == NULL || (clave = (key_t)strtoul(key, NULL, 10)) == IPC_PRIVATE)
    {
        printf("      delkey necesita clave_valida\n");
        return;
    }
    if ((id = shmget(clave, 0, 0666)) == -1)
    {
        perror("shmget: imposible obtener memoria compartida");
        return;
    }
    if (shmctl(id, IPC_RMID, NULL) == -1)
        perror("shmctl: imposible eliminar id de memoria compartida\n");
}

void SharedCreate(char *trozos, ListM *M)
{
    key_t cl;
    size_t tam;
    void *p;
    char *tr[COMMAND_LENGTH];

    trocearCadena(trozos, tr);
    if (tr[3] == NULL)
    {
        printListM(*M, "shared");
        return;
    }

    cl = (key_t)strtoul(tr[2], NULL, 10);
    tam = (size_t)strtoul(tr[3], NULL, 10);
    if (tam == 0)
    {
        printf("No se asignan bloques de 0 bytes\n");
        return;
    }
    if ((p = ObtenerMemoriaShmget(cl, tam, M)) != NULL)
        printf("Asignados %lu bytes en %p\n", (unsigned long)tam, p);
    else
        printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long)cl, strerror(errno));
}

void *MapearFichero(char *fichero, int protection, ListM *L)
{
    int df, map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat s;
    void *p;
    ItemM item;
    struct tm *fecha;
    time_t tiempo;

    time(&tiempo);
    fecha = localtime(&tiempo);

    if (protection & PROT_WRITE)
        modo = O_RDWR;
    if (stat(fichero, &s) == -1 || (df = open(fichero, modo)) == -1)
        return NULL;
    if ((p = mmap(NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED)
        return NULL;

    item.hora = fecha;
    item.memoryAddress = p;
    strcpy(item.mode, "mmaped");
    item.size = s.st_size;
    item.fd = df;
    strcpy(item.name, fichero);

    insertItemM(item, L);
    /* Guardar en la lista    InsertarNodoMmap (&L,p, s.st_size,df,fichero); */
    return p;
}

void CmdMmap(char *argument, char *permisos, ListM *L)
{
    char *perm;
    void *p;
    int protection = 0;

    if ((perm = permisos) != NULL && strlen(perm) < 4)
    {
        if (strchr(perm, 'r') != NULL)
            protection |= PROT_READ;
        if (strchr(perm, 'w') != NULL)
            protection |= PROT_WRITE;
        if (strchr(perm, 'x') != NULL)
            protection |= PROT_EXEC;
    }
    if ((p = MapearFichero(argument, protection, L)) == NULL)
        perror("Imposible mapear fichero");
    else
        printf("fichero %s mapeado en %p\n", argument, p);
}

void mmapFree(char *fichero, ListM *L)
{
    PosM p = *L;

    while (strcmp(p->data.name, fichero))
        p = p->next;

    if (munmap(p->data.memoryAddress, p->data.size))
    {
        perror("Fichero no mapeado");
        return;
    }
    close(p->data.fd);
    deleteAtPositionM(p, L);
}

int LeerFichero(char *f, char *p, command argumentos)
{
    struct stat s;
    ssize_t n;
    int df;
    ssize_t cont;
    void *punt;
    char *trozos[COMMAND_LENGTH];
    trocearCadena(argumentos, trozos);
    sscanf(p, "0x%p", &punt);

    if (trozos[3] == NULL)
        cont = -1;
    else
        cont = atoi(trozos[3]);

    if (stat(f, &s) == -1 || (df = open(f, O_RDONLY)) == -1)
    {
        perror("Imposible leer fichero: ");
        return -1;
    }
    if (cont == -1) /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont = s.st_size;
    if ((n = read(df, punt, cont)) == -1)
    {
        perror("Imposible leer fichero: ");
        close(df);
        return -1;
    }
    printf("Leídos %ld bytes de %s en %p\n", n, f, punt);
    close(df);
    return 0;
}

int EscribirFichero(command argumentos)
{
    ssize_t n, cont;
    int df, flags = O_CREAT | O_EXCL | O_WRONLY;
    void *punt;
    char *trozos[COMMAND_LENGTH], f[COMMAND_LENGTH];
    int palabras = trocearCadena(argumentos, trozos);

    if (palabras < 4 || (!strcmp(trozos[1], "-o") && palabras < 5))
    {
        printf("Faltan parámetros\n");
        return -1;
    }

    if (!strcmp(trozos[1], "-o"))
    {
        flags = O_CREAT | O_WRONLY | O_TRUNC;
        cont = atoi(trozos[4]);
        sscanf(trozos[3], "0x%p", &punt);
        strcpy(f, trozos[2]);
    }
    else
    {
        cont = atoi(trozos[3]);
        sscanf(trozos[2], "0x%p", &punt);
        strcpy(f, trozos[1]);
    }

    if ((df = open(f, flags, 0777)) == -1)
    {
        perror("No se pudo escribir: ");
        return -1;
    }

    if ((n = write(df, punt, cont)) == -1)
    {
        perror("No se pudo escribir: ");
        close(df);
        return -1;
    }

    printf("Escritos %ld bytes en %s desde %p\n", n, f, punt);
    close(df);
    return 0;
}

void LlenarMemoria(command argumentos)
{
    void *p;
    unsigned char byte;
    char *trozos[COMMAND_LENGTH];
    int palabras = trocearCadena(argumentos, trozos);

    if (palabras != 4)
    {
        printf("Parámetros inadecuados\n");
        return;
    }

    ssize_t cont = atoi(trozos[2]), i;

    sscanf(trozos[1], "0x%p", &p);
    sscanf(trozos[3], "%hhu", &byte);
    unsigned char *arr = (unsigned char *)p;

    for (i = 0; i < cont; i++)
        arr[i] = byte;

    printf("Llenando %ld bytes de memoria con el byte (%hhu) a partir de la dirección %p\n", cont, byte, p);
}

void memDump(command argumentos)
{
    void *p;
    char *trozos[COMMAND_LENGTH];
    int palabras = trocearCadena(argumentos, trozos);

    if (palabras != 3)
    {
        printf("Parámetros inadecuados\n");
        return;
    }

    ssize_t cont = atoi(trozos[2]), i;

    sscanf(trozos[1], "0x%p", &p);
    unsigned char *arr = (unsigned char *)p;

    printf("Volcando %lu bytes desde la dirección %p\n", cont, p);
    for (i = 0; i < cont; i++)
    {
        printf("%hhu ", arr[i]);
    }
    puts("");
}

void Recursiva(int n)
{
    char automatico[TAMANO];
    static char estatico[TAMANO];

    printf("parametro:%3d(%p) array %p, arr estatico %p\n", n, &n, automatico, estatico);

    if (n > 0)
        Recursiva(n - 1);
}

void memVars()
{
    int *i = malloc(sizeof(i)), *j = malloc(sizeof(j)), *k = malloc(sizeof(k));
    int x, y, z, x2 = 1, y2 = 2, z2 = 3;
    a = malloc(sizeof(a));
    b = malloc(sizeof(b));
    c = malloc(sizeof(c));

    printf("%15s\t%15p,%15p,%15p\n", "Variables locales", &i, &j, &k);
    printf("%15s\t%15p,%15p,%15p\n", "Variables globales", &a, &b, &c);
    printf("%15s\t%15p,%15p,%15p\n", "Var (N.I) globales", &d, &e, &f);
    printf("%15s\t%15p,%15p,%15p\n", "Variables estaticas", &x2, &y2, &z2);
    printf("%15s\t%15p,%15p,%15p\n", "Var (N.I) estaticas", &x, &y, &z);

    free(i);
    free(j);
    free(k);
    free(a);
    free(b);
    free(c);
}

void memFuncs()
{
    printf("%15s\t%15p,%15p,%15p\n", "Funciones programa", *Recursiva, *memVars, *memDump);
    printf("%15s\t%15p,%15p,%15p\n", "Funciones libreria", *strcmp, *malloc, *getpid);
}