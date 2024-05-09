#include "procesos.h"

char *userName(__uid_t uid)
{
    struct passwd *d;

    if ((d = getpwuid(uid)) == NULL)
        perror("No se pudo realizar");

    return d->pw_name;
}

void uidGET()
{
    printf("Credencial real: %d, (%s)\n", getuid(), userName(getuid()));
    printf("Credencial efectiva: %d, (%s)\n", geteuid(), userName(geteuid()));
}

void uidSetName(char *arguments)
{
    struct passwd *d;
    char *trozos[COMMAND_LENGTH];

    trocearCadena(arguments, trozos);

    if (trozos[2] == NULL || trozos[3] == NULL)
    {
        uidGET();
        return;
    }
    if ((d = getpwnam(trozos[3])) == NULL)
        perror("No se pudo efectuar el cambio:");
    if (setuid(d->pw_uid))
        perror("No se pudo efectuar el cambio:");
}

void uidSetID(char *id)
{
    if (id == NULL)
    {
        uidGET();
        return;
    }
    if (setuid((uid_t)(atoi(id))))
        perror("No se pudo realizar:");
}

void showenvADDR(char *env[])
{
    printf("environ: %p (almacenado en %p)\n", environ, &environ);
    printf("main arg3: %p (almacenado en %p)\n", env, &env);
}

void showEveryVarENVIRON()
{
    int i = 0;
    while (environ[i] != NULL)
    {
        printf("%p->%s[%d]=(%p) %s\n", &environ[i], "environ", i, environ[i], environ[i]);
        i++;
    }
}

void showEveryVarENV(char *env[])
{
    int i = 0;
    while (env[i] != NULL)
    {
        printf("%p->%s[%d]=(%p) %s\n", &env[i], "main arg3", i, env[i], env[i]);
        i++;
    }
}

int BuscarVariable(char *var, char *e[]) /*busca una variable en el entorno que se le pasa como parámetro*/
{
    int pos = 0;
    char aux[MAXVAR];

    strcpy(aux, var);
    strcat(aux, "=");

    while (e[pos] != NULL)
        if (!strncmp(e[pos], aux, strlen(aux)))
            return (pos);
        else
            pos++;
    errno = ENOENT; /*no hay tal variable*/
    perror("No se pudo realizar:");
    return (-1);
}

void showvar(char *var, char *env[])
{
    int pos;
    char *p;

    if (!(pos = BuscarVariable(var, env)))
        printf(" ");
    else
        printf("Con arg3 main: %s(%p) @%p\n", env[pos], env[pos], &env);

    if (!(pos = BuscarVariable(var, environ)))
        printf(" ");
    else
        printf("Con environ: %s(%p) @%p\n", environ[pos], environ[pos], &environ);

    if ((p = getenv(var)) == NULL)
        printf(" ");
    else
        printf("Con getenv: %s(%p)\n", p, p);
}

int CambiarVariable(char *var, char *valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parámetro*/
{                                                      /*lo hace directamente, no usa putenv*/
    int pos;
    char *aux;

    if ((pos = BuscarVariable(var, e)) == -1)
        return (-1);

    if ((aux = (char *)malloc(strlen(var) + strlen(valor) + 2)) == NULL)
        return -1;
    strcpy(aux, var);
    strcat(aux, "=");
    strcat(aux, valor);
    e[pos] = aux;
    return (pos);
}

void changevar(char *argumentos, char *env[])
{
    char *trozos[COMMAND_LENGTH];
    char *newvar;

    trocearCadena(argumentos, trozos);

    if (trozos[1] == NULL || trozos[2] == NULL || trozos[3] == NULL)
    {
        printf("Uso: changevar [-a|-e|-p] var valor\n");
        return;
    }

    if (!strcmp(trozos[1], "-a"))
        CambiarVariable(trozos[2], trozos[3], env);
    if (!strcmp(trozos[1], "-e"))
        CambiarVariable(trozos[2], trozos[3], environ);
    if (!strcmp(trozos[1], "-p"))
    {
        if ((newvar = (char *)malloc(strlen(trozos[2]) + strlen(trozos[3]) + 2)) == NULL)
        {
            perror("Fallo:");
            return;
        }
        strcpy(newvar, trozos[2]);
        strcat(newvar, "=");
        strcat(newvar, trozos[3]);
        strcat(newvar, "\0");

        if (putenv(newvar))
            perror("No se pudo realizar:");
    }
}

int sustituirVar(char *var, char *newvar, char *valor, char *e[])
{
    int pos;
    char *aux;

    if ((pos = BuscarVariable(var, e)) == -1)
        return (-1);

    if ((aux = (char *)malloc(strlen(newvar) + strlen(valor) + 2)) == NULL)
        return -1;
    strcpy(aux, newvar);
    strcat(aux, "=");
    strcat(aux, valor);
    e[pos] = aux;
    return (pos);
}

void subsvar(char *argumentos, char *env[])
{
    char *trozos[COMMAND_LENGTH];

    trocearCadena(argumentos, trozos);

    if (trozos[1] == NULL || trozos[2] == NULL || trozos[3] == NULL || trozos[4] == NULL)
    {
        printf("Uso: subsvar [-a|-e] var1 var2 valor\n");
        return;
    }
    if (!strcmp(trozos[1], "-a"))
        sustituirVar(trozos[2], trozos[3], trozos[4], env);
    if (!strcmp(trozos[1], "-e"))
        sustituirVar(trozos[2], trozos[3], trozos[4], environ);
}

void Cmd_fork()
{
    pid_t pid;

    if ((pid = fork()) == 0)
    {
        printf("ejecutando proceso %d\n", getpid());
    }
    else if (pid != -1)
        waitpid(pid, NULL, 0);
}

void exec(char *argumentos)
{
    char *trozos[COMMAND_LENGTH];
    int i = 0;

    strtok(argumentos, " \n\t");

    while ((trozos[i] = strtok(NULL, " \n\t")) != NULL)
        i++;

    if (execvp(trozos[0], trozos))
        perror("Error:");
}

void defaultExec(char *argumentos)
{
    char *trozos[COMMAND_LENGTH];
    pid_t pid;

    trocearCadena(argumentos, trozos);

    if ((pid = fork()) == -1)
        perror("Error:");
    else if (pid > 0)
        waitpid(pid, NULL, 0);
    else
    {
        if (execvp(trozos[0], trozos))
            perror("Error:");
        exit(0);
    }
}

void defaultExecBACKGROUND(char *argumentos, ListP *list)
{
    char *trozos[COMMAND_LENGTH];
    ItemP item;
    struct tm *fecha;
    time_t tiempo;
    int i = 1;

    time(&tiempo);
    fecha = localtime(&tiempo);
    strcpy(item.command, argumentos);

    trozos[0] = strtok(argumentos, " &\n\t");

    while ((trozos[i] = strtok(NULL, " &\n\t")) != NULL)
        i++;

    if ((item.pid = fork()) == -1)
        perror("Error:");
    else if (item.pid > 0)
    {
        item.fecha = fecha;
        item.status = ACTIVE;
        insertItemP(item, list);
    }
    else
    {
        if (execvp(trozos[0], trozos) == -1)
            perror("Error:");
        exit(0);
    }
}

void defaultElegir(char *argumentos, ListP *listaProcesos)
{
    char *trozos[COMMAND_LENGTH], comando[COMMAND_LENGTH];
    int i;

    strcpy(comando, argumentos);
    i = trocearCadena(argumentos, trozos);

    if (i == 0)
        return;
    if (trozos[0] == NULL)
    {
        printf("Invalid command\n");
        return;
    }

    if (!strcmp(trozos[i - 1], "&"))
        defaultExecBACKGROUND(comando, listaProcesos);
    else
        defaultExec(comando);
}

void refreshList(ListP *list)
{
    ItemP item;
    PosP p;
    int status, pid;

    p = *list;

    while (p != NULL)
    {
        item = getItemP(p, *list);
        status = 0;

        pid = waitpid(item.pid, &status, WNOHANG | WUNTRACED);
        if (pid == item.pid)
        {
            if (WIFEXITED(status))
                item.status = FINISHED;
            else if (WIFSIGNALED(status))
                item.status = SIGNAL;
            else if (WIFSTOPPED(status))
                item.status = STOPPED;
            else if (WIFCONTINUED(status))
                item.status = ACTIVE;
            else
                item.status = ACTIVE;
        }

        updateItemP(item, p, list);
        p = p->next;
    }
}

void del(ListP *list, int status)
{
    ItemP item;
    PosP p = *list;

    while (p != NULL)
    {
        item = getItemP(p, *list);

        if (item.status == status)
            deleteAtPositionP(p, list);
        p = p->next;
    }
}

void fg(ListP *list, char *argumentos)
{
    char *trozos[COMMAND_LENGTH];
    pid_t pid;

    trocearCadena(argumentos, trozos);

    if (strcmp(trozos[1], "-fg") == 0)
    {
        if (isdigit(trozos[2]))
        {
            pid = strtol(trozos[2], NULL, 10);
            waitpid(pid, NULL, 0);
        }
    }
    else
    {
        pid = strtol(trozos[1], NULL, 10);
        printOne(list, pid);
    }
}

void printOne(ListP *list, pid_t pid)
{
    PosP p = *list;
    ItemP item;

    while (p != NULL)
    {
        item = getItemP(p, *list);

        if (item.pid == pid)
            printf("%7d%15s%4s%d %7d/%d/%d %d:%d:%d %s (%d) %s\n",
                   item.pid, userName(getuid()), "p=", getpriority(PRIO_PROCESS, item.pid),
                   item.fecha->tm_year + 1900, item.fecha->tm_mon, item.fecha->tm_mday,
                   item.fecha->tm_hour, item.fecha->tm_min, item.fecha->tm_sec,
                   item.status == FINISHED ? "TERMINADO" : (item.status == ACTIVE ? "ACTIVO" : (item.status == STOPPED ? "PARADO" : "SIGNAL")),
                   item.status == FINISHED ? WEXITSTATUS(item.status) : (item.status == ACTIVE ? 000 : (item.status == STOPPED ? WSTOPSIG(item.status) : WTERMSIG(item.status))),
                   item.command);

        p = p->next;
    }
}

void printListP(ListP *list)
{
    PosP p = *list;
    ItemP item;

    while (p != NULL)
    {
        item = getItemP(p, *list);

        printf("%7d%15s%4s%d %7d/%d/%d %d:%d:%d %s (%d) %s\n",
               item.pid, userName(getuid()), "p=", getpriority(PRIO_PROCESS, item.pid),
               item.fecha->tm_year + 1900, item.fecha->tm_mon, item.fecha->tm_mday,
               item.fecha->tm_hour, item.fecha->tm_min, item.fecha->tm_sec,
               item.status == FINISHED ? "TERMINADO" : (item.status == ACTIVE ? "ACTIVO" : (item.status == STOPPED ? "PARADO" : "SIGNAL")),
               item.status == FINISHED ? WEXITSTATUS(item.status) : (item.status == ACTIVE ? 000 : (item.status == STOPPED ? WSTOPSIG(item.status) : WTERMSIG(item.status))),
               item.command);

        p = p->next;
    }
}

void clearListP(ListP *L)
{
    PosP p, temp;
    p = *L;

    if (*L == NULL)
        return;

    while (p != NULL)
    {
        temp = p;
        p = p->next;

        deleteAtPositionP(temp, L);
    }
    createEmptyListP(L);
}