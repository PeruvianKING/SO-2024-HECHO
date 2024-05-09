
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <features.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include "list.h"
#include "memoria.h"
#include "listaMemoria.h"
#include "procesos.h"
#include "listaProcesos.h"

void Loop(bool finish, char *env[]);
void printPrompt();
void readCommand(command *comando, List *listaHistorial); // pasamos una string por referencia para guardar sus datos
void processCommand(command *peticion, List *listaHistorial, List *listaFicheros, ListM *listaMemoria, bool *end, char *env[], ListP *listaProcesos);
void commands(int argument, List *listaHistorial, List *listaFicheros, ListM *listaMemoria, bool *end, char *argument1, char *argument2, char *argument3, command argumentos, char *env[], ListP *listaProcesos);
int chooseCommand(char *argument1, char *argument2, char *argument3);
void timeN();
void hist(List listaHistorial);
void histN(List listaHistorial, int n);
bool clearList(List *L);
void commandN(List listaHistorial, List *listaFicheros, ListM *listaMemoria, int n, bool *end, char *env[], ListP *listaProcesos);
void infosys();
void chdirActual();
bool changeDirectory(const char *pathname);
void openFile(char *tr[], List *listaFicheros);
void listOpen(List listaFicheros);
void inicializarListaFicheros(List *listaFicheros);
bool closedf(List *listaFicheros, int df);
void dupDf(List *listaFicheros, int df);
void date();
void help(char *cadena[]);
char LetraTF(mode_t m);
char *ConvierteModo2(mode_t m);
void createDir(char *name);
void createFile(char *name);
void delete(char *name);
void deleteTree(char *name);
void FileDirInformation(command argumentos);
void listContent(command argumentos, bool longg, bool recb, bool reca, bool hid, bool link, bool acc);

int main(int argc, char *argv[], char *env[])
{
    bool end = false; // inicializamos la variable para mantener el bucle

    Loop(end, env);

    return 0;
}

void Loop(bool finish, char *env[])
{
    command entrada;
    List historial, ficherosAbiertos;
    ListM listaMemoria;
    ListP listaProcesos;

    createEmptyList(&historial);
    createEmptyList(&ficherosAbiertos);
    createEmptyListM(&listaMemoria);
    createEmptyListP(&listaProcesos);
    inicializarListaFicheros(&ficherosAbiertos);

    while (!finish)
    {
        printPrompt();
        readCommand(&entrada, &historial);
        processCommand(&entrada, &historial, &ficherosAbiertos, &listaMemoria, &finish, env, &listaProcesos);
    }
    free(historial);
    free(ficherosAbiertos);
    free(listaMemoria);
}

void printPrompt()
{
    printf("-> ");
}

void readCommand(command *comando, List *listaHistorial)
{
    Item data;
    char *trozos[COMMAND_LENGTH];

    fgets(*comando, COMMAND_LENGTH, stdin); // guardamos la entrada del usuario en el string comando, pasado por referencia para guardar su contenido

    strcpy(data.name, *comando);
    insertItem(data, listaHistorial);

    trocearCadena(data.name, trozos);

    if (strcmp(trozos[0], "command") == 0)
        deleteAtPosition(last(*listaHistorial), listaHistorial);
}

void processCommand(command *peticion, List *listaHistorial, List *listaFicheros, ListM *listaMemoria, bool *end, char *env[], ListP *listaProcesos)
{
    command temp;
    char *trozo[COMMAND_LENGTH]; // creamos los parámetros donde guardaremos los datos de la petición, y un array de punteros nulos para comparar en la función trocearCadena
    int palabras;
    strcpy(temp, *peticion);
    palabras = trocearCadena(*peticion, trozo);

    if (palabras == 0)
        return;

    commands(chooseCommand(trozo[0], trozo[1], trozo[2]), listaHistorial, listaFicheros, listaMemoria, end, trozo[0], trozo[1], trozo[2], temp, env, listaProcesos);
}

int chooseCommand(char *argument1, char *argument2, char *argument3)
{
    if (argument2 != NULL && (!strcmp(argument2, "-?") || !strcmp(argument2, "-help")) && argument3 == NULL)
        return 27;
    if (strcmp(argument1, "authors") == 0) // authors
    {
        if (argument2 == NULL)
            return 1;
        else if (strcmp(argument2, "-l") == 0 && argument3 == NULL)
            return 2;
        else if (strcmp(argument2, "-n") == 0 && argument3 == NULL)
            return 3;
    }

    else if (strcmp(argument1, "time") == 0 && argument2 == NULL) // time
        return 4;

    else if (strcmp(argument1, "hist") == 0) // hist
    {
        if (argument2 == NULL)
            return 5;
        else if (strcmp(argument2, "-c") == 0 && argument3 == NULL)
            return 6;
        else if ((argument2[0] == '-' && isdigit(argument2[1])) && argument3 == NULL)
            return 7;
    }

    else if ((strcmp(argument1, "quit") == 0 || strcmp(argument1, "bye") == 0 || strcmp(argument1, "exit") == 0) && argument2 == NULL) // quit, bye, exit
        return 8;

    else if (strcmp(argument1, "command") == 0) // command N
    {
        if (argument2 == NULL)
            return 5;
        else if (isdigit(*argument2) && argument3 == NULL)
            return 9;
    }

    else if (strcmp(argument1, "pid") == 0) // pid
    {
        if (argument2 == NULL)
            return 10;
        else if (strcmp(argument2, "-p") == 0 && argument3 == NULL)
            return 11;
    }

    else if (strcmp(argument1, "infosys") == 0 && argument2 == NULL) // infosys
        return 12;

    else if (strcmp(argument1, "chdir") == 0) // chdir
    {
        if (argument2 == NULL)
            return 13;
        else if (argument3 == NULL)
            return 14;
    }

    else if (strcmp(argument1, "open") == 0) // open file mode
    {
        if (argument2 == NULL) // en caso de ser solo "open", mostramos la lista (return entero de la función openLIST)
            return 16;
        else
            return 15;
    }

    else if (strcmp(argument1, "listopen") == 0 && argument2 == NULL) // listopen
        return 16;

    else if (strcmp(argument1, "close") == 0) // close
    {
        if (argument2 == NULL)
            return 16;
        else if (isdigit(argument2[0]) && argument3 == NULL)
            return 17;
    }

    if (strcmp(argument1, "dup") == 0) // dup
    {
        if (argument2 == NULL)
            return 16;
        else if (isdigit(argument2[0]))
            return 18;
    }

    else if (strcmp(argument1, "date") == 0 && argument2 == NULL) // date
        return 19;

    else if (strcmp(argument1, "help") == 0) // help
        return 20;

    else if (strcmp(argument1, "create") == 0)
    {
        if (argument2 == NULL)
            return 13;
        else if (argument3 == NULL)
            return 21;
        else if (strcmp(argument2, "-f") == 0)
            return 22;
    }

    else if (strcmp(argument1, "delete") == 0)
    {
        if (argument2 != NULL && argument3 == NULL)
            return 23;
        else
            return 13;
    }

    else if (strcmp(argument1, "deltree") == 0)
    {
        if (argument2 != NULL && argument3 == NULL)
            return 24;
        else
            return 13;
    }

    else if (strcmp(argument1, "list") == 0)
    {
        if (argument2 != NULL)
            return 25;
        else
            return 13;
    }

    else if (strcmp(argument1, "stat") == 0)
    {
        if (argument2 == NULL)
            return 13;
        else
            return 26;
    }

    else if (!strcmp(argument1, "malloc"))
    {
        if (argument2 == NULL)
            return 29;
        else if (!strcmp(argument2, "-free"))
            return 30;
        else
            return 28;
    }

    else if (!strcmp(argument1, "mem"))
        return 31;

    else if (!strcmp(argument1, "mmap"))
        return 32;

    else if (!strcmp(argument1, "read"))
        return 33;

    else if (!strcmp(argument1, "write"))
        return 34;

    else if (!strcmp(argument1, "memfill"))
        return 35;

    else if (!strcmp(argument1, "memdump"))
        return 36;

    else if (!strcmp(argument1, "recurse"))
        return 37;

    else if (!strcmp(argument1, "shared"))
    {
        if (argument2 == NULL)
            return 38;
        else if (!strcmp(argument2, "-create"))
            return 39;
        else if (!strcmp(argument2, "-free"))
            return 40;
        else if (!strcmp(argument2, "-delkey"))
            return 41;
        else
            return 42;
    }
    else if (!strcmp(argument1, "uid"))
    {
        if (argument2 == NULL || !strcmp(argument2, "-get"))
            return 43;
        if (!strcmp(argument2, "-set") && argument3 != NULL && !strcmp(argument3, "-l"))
            return 44;
        if (!strcmp(argument2, "-set"))
            return 45;
    }
    else if (!strcmp(argument1, "showvar"))
    {
        if (argument2 == NULL)
            return 46;
        else
            return 47;
    }
    else if (!strcmp(argument1, "changevar"))
        return 48;

    else if (!strcmp(argument1, "subsvar"))
        return 49;

    else if (!strcmp(argument1, "showenv"))
    {
        if (argument2 == NULL)
            return 50;
        if (!strcmp(argument2, "-environ"))
            return 51;
        if (!strcmp(argument2, "-addr"))
            return 52;
    }

    else if (!strcmp(argument1, "fork"))
        return 53;

    else if (!strcmp(argument1, "exec"))
        return 54;

    else if (!strcmp(argument1, "jobs"))
        return 55;
    else if (!strcmp(argument1, "job"))
        return 56;
    else if (!strcmp(argument1, "deljobs"))
    {
        if (!strcmp(argument2, "-term"))
            return 57;
        if (!strcmp(argument2, "-sig"))
            return 58;
    }

    else if (argument1 != NULL)
        return 0;

    return 99999;
}

void commands(int argument, List *listaHistorial, List *listaFicheros, ListM *listaMemoria, bool *end, char *argument1, char *argument2, char *argument3, command argumentos, char *env[], ListP *listaProcesos)
{
    switch (argument)
    {
    case 1: // authors
        printf("Author1: author@udc.es\nAuthor2: author2@udc.es\n");
        break;

    case 2: // authors -l
        printf("author@udc.es\nauthor@udc.es\n");
        break;

    case 3: // authors -n
        printf("Author1\nAuthor2\n");
        break;

    case 4: // time
        timeN();
        break;

    case 5: // hist
        hist(*listaHistorial);
        break;

    case 6: // hist -c
        clearList(listaHistorial);
        break;

    case 7: // hist -n
        argument2[0] = '0';
        histN(*listaHistorial, atoi(argument2));
        break;

    case 8: // quit, bye, exit
        (*end) = true;
        clearList(listaFicheros);
        clearList(listaHistorial);
        clearListM(listaMemoria);
        clearListP(listaProcesos);
        break;

    case 9: // command N
        if (argument2 == NULL)
            hist(*listaHistorial);
        else
            commandN(*listaHistorial, listaFicheros, listaMemoria, atoi(argument2), end, env, listaProcesos);
        break;

    case 10: // pid
        printf("Pid de shell: %d\n", getpid());
        break;

    case 11: // pid -p
        printf("Pid del padre del shell: %d\n", getppid());
        break;

    case 12: // infosys
        infosys();
        break;

    case 13: // chdir mostrar
        chdirActual();
        break;

    case 14: // chdir entrar
        changeDirectory(argument2);
        break;

    case 15: // open [file] [mode]
        openFile(&argumentos, listaFicheros);
        break;

    case 16: // listopen
        listOpen(*listaFicheros);
        break;

    case 17: // close [df]
        closedf(listaFicheros, atoi(argument2));
        break;

    case 18: // dup
        dupDf(listaFicheros, atoi(argument2));
        break;

    case 19: // date
        date();
        break;

    case 20: // help
        help(&argument2);
        break;

    case 21: // create dir
        createDir(argument2);
        break;

    case 22: // create file
        createFile(argument3);
        break;

    case 23: // delete
        delete (argument2);
        break;

    case 24: // deltree
        deleteTree(argument2);
        break;

    case 25: // list
        listContent(argumentos, false, false, false, false, false, false);
        break;

    case 26: // stat
        FileDirInformation(argumentos);
        break;

    case 27: // help -?
        help(&argument1);
        break;

    case 28: // malloc [size]
        mallocMemoria(atoi(argument2), listaMemoria);
        break;

    case 29: // malloc a secas
        printListM(*listaMemoria, "malloc");
        break;

    case 30: // malloc free
        if (argument3 == NULL)
            printListM(*listaMemoria, "malloc");
        else
            freeMalloc("malloc", listaMemoria, atoi(argument3));
        break;

    case 31: // mem
        if (argument2 == NULL || !strcmp(argument2, "-all"))
        {
            memVars();
            memFuncs();
            printListM(*listaMemoria, " ");
        }
        else if (!strcmp(argument2, "-pmap"))
            Do_MemPmap();
        else if (!strcmp(argument2, "-vars"))
            memVars();
        else if (!strcmp(argument2, "-funcs"))
            memFuncs();
        else if (!strcmp(argument2, "-blocks"))
            printListM(*listaMemoria, "all");

        break;

    case 32: // mmap
        if (argument2 == NULL || (!strcmp(argument2, "-free") && argument3 == NULL))
            printListM(*listaMemoria, "mmaped");
        else if (!strcmp(argument2, "-free") && argument3 != NULL)
            mmapFree(argument3, listaMemoria);
        else
            CmdMmap(argument2, argument3, listaMemoria);
        break;

    case 33: // read
        if (argument2 == NULL)
            printf("Faltan parametros");
        else
            LeerFichero(argument2, argument3, argumentos);
        break;

    case 34: // write
        EscribirFichero(argumentos);
        break;

    case 35: // memfill
        LlenarMemoria(argumentos);
        break;

    case 36: // memdump
        memDump(argumentos);
        break;

    case 37: // recurse
        Recursiva(atoi(argument2));
        break;

    case 38: // shared
        printListM(*listaMemoria, "shared");
        break;

    case 39: // shared -create
        if (argument3 == NULL)
            printListM(*listaMemoria, "shared");
        else
            SharedCreate(argumentos, listaMemoria);
        break;

    case 40: // shared -free
        if (argument3 == NULL)
            printListM(*listaMemoria, "shared");
        ShareFree(atoi(argument3), listaMemoria);
        break;

    case 41: // shared -delkey
        SharedDelkey(argumentos);
        break;

    case 42: // shared key
        SharedExistent(argumentos, listaMemoria);
        break;

    case 43: // uid -get
        uidGET();
        break;

    case 44: // uid -set -l
        uidSetName(argumentos);
        break;

    case 45: // uid -set
        uidSetID(argument3);
        break;

    case 46: // showvar mostrar todo
        showEveryVarENV(env);
        break;

    case 47: // showvar
        showvar(argument2, env);
        break;

    case 48: // changevar
        changevar(argumentos, env);
        break;

    case 49: // subsvar
        subsvar(argumentos, env);
        break;

    case 50: // showenv
        showEveryVarENV(env);
        break;

    case 51: // showenv -environ
        showEveryVarENVIRON();
        break;

    case 52: // showenv -addr
        showenvADDR(env);
        break;

    case 53: // fork
        Cmd_fork();
        break;

    case 54: // exec
        exec(argumentos);
        break;

    case 55: // jobs
        refreshList(listaProcesos);
        printListP(listaProcesos);
        break;

    case 0: // default
        defaultElegir(argumentos, listaProcesos);
        break;

    case 56: // job
        fg(listaProcesos, argumentos);
        break;

    case 57: // del -term
        del(listaProcesos, FINISHED);
        break;

    case 58: // del -sig
        del(listaProcesos, SIGNAL);
        break;
        
    default:
        printf("Invalid command\n");
        break;
    }
}

void timeN()
{
    time_t timeNOW = time(NULL); // creamos variable tipo time_t para inicializar el registro de tipo tm de la librería time.h, y así conseguir el tiempo actual con 'localtime'
    struct tm *tmStruct = localtime(&timeNOW);
    printf("%d:%d:%d\n", tmStruct->tm_hour, tmStruct->tm_min, tmStruct->tm_sec); // accedemos a los miembros del registro que nos interesan: horas, minutos y segundos
}

void hist(List listaHistorial)
{
    Pos p;
    int i = 0;

    for (p = listaHistorial; (p != NULL); p = p->next) // recorremos el historial y mostramos su contenido
    {
        printf("%d -> %s", i, p->data.name);
        i++;
    }
}

void histN(List listaHistorial, int n)
{
    Pos p;
    int i = 0;
    p = listaHistorial;

    while (p != NULL && i < n) //  enseña el historial hasta llegar al elemento anterior de la posición dada
    {
        printf("%d -> %s", i, p->data.name);
        p = p->next;
        i++;
    }
}

bool clearList(List *L)
{
    Pos p, temp;
    p = *L;

    if (*L == NULL)
        return false;

    while (p != NULL) // mientras haya elementos en la listaHistorial
    {
        temp = p;    // guardamos en temp el elemento de la listaHistorial
        p = p->next; // p pasa al siguiente elemento para que cuando liberemos el espacio no nos quedemos fuera de la listaHistorial
        free(temp);  // liberamos el espacio
    }
    createEmptyList(L); // inicializamos de vuelta la listaHistorial
    return true;
}

void commandN(List listaHistorial, List *listaFicheros, ListM *listaMemoria, int n, bool *end, char *env[], ListP *listaProcesos)
{
    Pos p;
    command temp, aux;
    char *trozo[COMMAND_LENGTH];

    if (isEmptyList(listaHistorial))
    {
        printf("El historial está vacío\n");
        return;
    }

    p = findPosition(n, listaHistorial); // conseguimos el elemento de la listaHistorial de la posición que nos interesa

    if (p == NULL || p->data.posicion != n) // si n es una posición que no existe en la listaHistorial entonces acaba
    {
        printf("No hay elemento %d en el historico\n", n);
        return;
    }

    strcpy(temp, p->data.name);
    printf("Ejecutando hist (%d): %s", n, temp); // en caso de que sí haya esa posición printeamos la muletilla

    strcpy(aux, p->data.name); // copiamos en temp el nombre del comando para no desconfigurar la listaHistorial

    trocearCadena(temp, trozo); // troceamos la cadena y obtenemos en el array de punteros trozo el comando separado
    commands(chooseCommand(trozo[0], trozo[1], trozo[2]), &listaHistorial, listaFicheros, listaMemoria, end, trozo[0], trozo[1], trozo[2], aux, env, listaProcesos);
}

void infosys()
{
    struct utsname unameData; // definimos un registro tipo utsname

    uname(&unameData); // llamamos a la función uname con un puntero al registro, ahora deberíamos tener la información del sistema en el registro
    printf("%s (%s), OS: %s-%s-%s\n", unameData.nodename, unameData.machine, unameData.sysname, unameData.release, unameData.version);
}

void chdirActual()
{
    char path[200]; // creamos un string donde guardaremos el camino del directorio actual

    getcwd(path, sizeof(path)); // guardamos en path el directorio actual con getcwd y le asignamos el tamaño de path
    printf("%s\n", path);
}

void openFile(char *trozo[], List *listaFicheros)
{
    int df, mode = 0;
    char *tr[COMMAND_LENGTH];
    Item fichero;

    trocearCadena(*trozo, tr); // troceamos el comando y lo guardamos en tr[]

    if (tr[2] == NULL && tr[1] != NULL)
    {
        printf("Modo no válido\n");
        return;
    }

    if (!strcmp(tr[2], "cr")) // asignamos a mode el permiso requerido
        mode |= O_CREAT;
    else if (!strcmp(tr[2], "ex"))
        mode |= O_EXCL;
    else if (!strcmp(tr[2], "ro"))
        mode |= O_RDONLY;
    else if (!strcmp(tr[2], "wo"))
        mode |= O_WRONLY;
    else if (!strcmp(tr[2], "rw"))
        mode |= O_RDWR;
    else if (!strcmp(tr[2], "ap"))
        mode |= O_APPEND;
    else if (!strcmp(tr[2], "tr"))
        mode |= O_TRUNC;

    if ((df = open(tr[1], mode, 0777)) == -1) // abrimos el fichero y guardamos su descriptor de archivo en df
        perror("Imposible abrir fichero");    // si devuelve -1 entonces no se pudo abrir y mostramos mensaje de error
    else
    {
        fichero.fileDescriptor = df; // en el caso de que el fichero se pudo abrir, guardamos sus datos en un registro tipo Item y lo insertamos
        strcpy(fichero.name, tr[1]);
        fichero.mode = mode;

        insertItem(fichero, listaFicheros);

        printf("Anadida entrada %d a la tabla ficheros abiertos\n", df); // dado que df también nos sirve como referencia a la posición de la lista, lo utilizamos en el printf
    }
}

void listOpen(List listaFicheros)
{
    Pos p;
    char modo[COMMAND_LENGTH] = ""; // inicializamos modo

    if (listaFicheros == NULL)
        return;
    else
    {
        for (p = listaFicheros; p != NULL; p = p->next) // para cada elemento de la lista miramos su modo para el printf
        {
            if (p->data.mode == O_CREAT)
                strcpy(modo, "O_CREAT");
            if (p->data.mode == O_EXCL)
                strcpy(modo, "O_EXCL");
            if (p->data.mode == O_RDONLY)
                strcpy(modo, "O_RDONLY");
            if (p->data.mode == O_WRONLY)
                strcpy(modo, "O_WRONLY");
            if (p->data.mode == O_RDWR)
                strcpy(modo, "O_RDWR");
            if (p->data.mode == O_APPEND)
                strcpy(modo, "O_APPEND");
            if (p->data.mode == O_TRUNC)
                strcpy(modo, "O_TRUNC");

            printf("descriptor: %d -> %s %s\n", p->data.fileDescriptor, p->data.name, modo);
        }
    }
}

void inicializarListaFicheros(List *listaFicheros) // metemos los descriptores de archivos reservados en la lista
{
    Item d;
    d.posicion = 0;

    d.fileDescriptor = STDIN_FILENO;
    strcpy(d.name, "entrada estandar");
    d.mode |= fcntl(d.fileDescriptor, F_GETFL); // fcntl nos devuelve el modo de apertura de un fichero (con el flag F_GETFL) dado un descriptor de fichero
    insertItem(d, listaFicheros);

    d.fileDescriptor = STDOUT_FILENO;
    strcpy(d.name, "salida estandar");
    d.mode |= fcntl(d.fileDescriptor, F_GETFL);
    insertItem(d, listaFicheros);

    d.fileDescriptor = STDERR_FILENO;
    strcpy(d.name, "error estandar");
    d.mode |= fcntl(d.fileDescriptor, F_GETFL);
    insertItem(d, listaFicheros);
}

bool closedf(List *listaFicheros, int df)
{
    Pos p;

    p = findFileDescriptor(df, *listaFicheros);

    if (close(p->data.fileDescriptor) == (-1)) // close devuelve -1 si da error, y 0 si es exitoso, findFileDescriptor nos devuelve el descriptor de fichero de la posición que buscamos
    {
        perror("Imposible cerrar descriptor");
        free(p); // liberamos p, ya que en el  caso de que el fileDescriptor sea -1 significa que hemos creado un tipo Pos temporal que debemos liberar una vez acabado su uso
        return false;
    }
    else
    {
        deleteAtPosition(p, listaFicheros);
        return true;
    }
}

bool changeDirectory(const char *pathname)
{
    if (chdir(pathname) == -1)
    {
        perror("Imposible cambiar directorio");
        return false;
    }
    else
        return true;
}

void dupDf(List *listaFicheros, int df)
{
    Pos p;
    Item ficheroDup;
    char temp[1000];

    ficheroDup.fileDescriptor = dup(df);
    p = findFileDescriptor(df, *listaFicheros);
    ficheroDup.mode = p->data.mode;

    strcpy(temp, p->data.name); // copiamos el string en otro de mayor tamaño para que sprintf pueda hacer la operación con suficiente espacio
    sprintf(ficheroDup.name, "dup %d (%s)", df, temp);

    if (ficheroDup.fileDescriptor != (-1))
        insertItem(ficheroDup, listaFicheros);
    else
        perror("Imposible duplicar fichero");
}

void date()
{
    time_t tiempo;
    tiempo = time(NULL);

    struct tm fecha = *localtime(&tiempo);
    printf("%d/%d/%d\n", fecha.tm_mday, fecha.tm_mon + 1, fecha.tm_year + 1900);
}

void help(char *cadena[])
{
    if (*cadena == NULL)
        printf("'help [cmd|-lt|-T topic]' ayuda sobre comandos\n\t\t\t\tComandos disponibles:\nauthors pid chdir date time hist command open close dup listopen infosys help quit exit bye stat list create deltree delete malloc shared mmap read write memdump memfill mem recurse\n");
    else if (strcmp(*cadena, "authors") == 0)
        printf("authors [-n|-l] Muestra los nombres y/o logins de los autores\n");

    else if (strcmp(*cadena, "pid") == 0)
        printf("pid [-p]\t\tMuestra el pid del shell o de su proceso padre\n");

    else if (strcmp(*cadena, "chdir") == 0)
        printf("chdir [dir]\t\tCambia (o muestra) el directorio actual del shell\n");

    else if (strcmp(*cadena, "date") == 0)
        printf("date\t\tMuestra la fecha actual\n");

    else if (strcmp(*cadena, "time") == 0)
        printf("time\t\tMuestra la hora actual\n");

    else if (strcmp(*cadena, "hist") == 0)
        printf("hist [-c|-N]\t\tMuestra (o borra) el historico de comandos\n\t\t-N: muestra los N primeros\n\t\t-c: borra el historico\n");

    else if (strcmp(*cadena, "command") == 0)
        printf("command [-N]\t\tRepite el comando N (del historico)\n");

    else if (strcmp(*cadena, "open") == 0)
        printf("open fich m1 m2...\t\tAbre el fichero fich. y lo anade a la lista de ficherosabiertos del shell\n\t\tm1, m2..es el modo de apertura (or bit a bit de los siguientes).\n\t\tcr: O_CREAT\t\tap: O_APPEND\n\t\tex:O_EXCL\t\tro:O_RDONLY\n\t\trw: O_RDWR\t\two: O_WRONLY\n\t\ttr: O_TRUNC\n");

    else if (strcmp(*cadena, "close") == 0)
        printf("close df\t\tCierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos\n");

    else if (strcmp(*cadena, "dup") == 0)
        printf("dup df\tDuplica el descriptor de fichero df y anade una nueva entrada a la lista ficheros abiertos\n");

    else if (strcmp(*cadena, "listopen") == 0)
        printf("listopen [n]\tLista los ficheros abiertos (al menos n) del shell\n");

    else if (strcmp(*cadena, "infosys") == 0)
        printf("infosys\t\tMuestra informacion de la maquina donde corre el shell\n");

    else if (strcmp(*cadena, "help") == 0)
        printf("help [cmd]\t\tMuestra ayuda sobre los comandos\n\t\tcmd: info sobre el comando cmd\n");

    else if (strcmp(*cadena, "quit") == 0)
        printf("quit\tTermina la ejecucion del shell\n");

    else if (strcmp(*cadena, "exit") == 0)
        printf("exit\tTermina la ejecucion del shell\n");

    else if (strcmp(*cadena, "bye") == 0)
        printf("bye\tTermina la ejecucion del shell\n");

    else if (strcmp(*cadena, "stat") == 0)
        printf("stat\t[-long][-link][-acc] name1 name2..\t\tlista ficheros;\n\t\t-long: listado largo\n\t\t-acc: acesstime\n\t\t-link: si es enlace simbolico, el path contenido\n");

    else if (strcmp(*cadena, "list") == 0)
        printf("list [-reca] [-recb] [-hid][-long][-link][-acc] n1 n2 ..	lista contenidos de directorios\n\t\t-hid:incluye los ficheros ocultos\n\t\t-recb:recursivo (antes)\n\t\t-reca: recursivo (despues)\n\t\tresto parametros como stat\n");

    else if (strcmp(*cadena, "create") == 0)
        printf("create [-f] [name]	Crea un directorio o un fichero (-f)\n");

    else if (strcmp(*cadena, "deltree") == 0)
        printf("deltree [name1 name2 ..]	Borra ficheros o directorios no vacios recursivamente\n");

    else if (strcmp(*cadena, "delete") == 0)
        printf("delete [name1 name2 ..]	Borra ficheros o directorios vacios\n");

    else if (strcmp(*cadena, "malloc") == 0)
        printf("malloc [-free] [tam]	asigna un bloque memoria de tamano tam con malloc\n\t-free: desasigna un bloque de memoria de tamano tam asignado con malloc\n");

    else if (strcmp(*cadena, "shared") == 0)
        printf("shared [-free|-create|-delkey] cl [tam]	asigna memoria compartida con clave cl en el programa\n\t-create cl tam: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam\n\t-free cl: desmapea el bloque de memoria compartida de clave cl\n\t-delkey clelimina del sistema (sin desmapear) la clave de memoria cl\n");

    else if (strcmp(*cadena, "mmap") == 0)
        printf("mmap [-free] fich prm	mapea el fichero fich con permisos prm\n\t-free fich: desmapea el ficherofich\n");

    else if (strcmp(*cadena, "read") == 0)
        printf("read fiche addr cont 	Lee cont bytes desde fich a la direccion addr\n");

    else if (strcmp(*cadena, "write") == 0)
        printf("write [-o] fiche addr cont 	  Escribe cont bytes desde la direccion addr a fich (-o sobreescribe)\n");

    else if (strcmp(*cadena, "memdump") == 0)
        printf("memdump addr cont 	  Vuelca en pantallas los contenidos (cont bytes) de la posicion de memoria addr\n");

    else if (strcmp(*cadena, "memfill") == 0)
        printf("memfill addr cont byte 	  Llena la memoria a partir de addr con byte\n");

    else if (strcmp(*cadena, "mem") == 0)
        printf("mem [-blocks|-funcs|-vars|-all|-pmap] ..	Muestra muestra detalles de la memoria del proceso\n\t\t-blocks: los bloques de memoria asignados\n\t\t-funcs: las direcciones de las funciones\n\t\t-vars: las direcciones de las variables\n\t\t-all: todo\n\t\t-pmap: muestra la salida del comando pmap(o similar)\n");

    else if (strcmp(*cadena, "recurse") == 0)
        printf("recurse [n]	     Invoca a la funcion recursiva n veces\n");

    else
        printf("%s no encontrado\n", *cadena);
}

char LetraTF(mode_t m)
{
    switch (m & __S_IFMT)
    { /*and bit a bit con los bits de formato,0170000 */
    case __S_IFSOCK:
        return 's'; /*socket */
    case __S_IFLNK:
        return 'l'; /*symbolic link*/
    case __S_IFREG:
        return '-'; /* fichero normal*/
    case __S_IFBLK:
        return 'b'; /*block device*/
    case __S_IFDIR:
        return 'd'; /*directorio */
    case __S_IFCHR:
        return 'c'; /*char device*/
    case __S_IFIFO:
        return 'p'; /*pipe*/
    default:
        return '?'; /*desconocido, no deberia aparecer*/
    }
}

char *ConvierteModo2(mode_t m)
{
    static char permisos[12];
    strcpy(permisos, "---------- ");

    permisos[0] = LetraTF(m);
    if (m & S_IRUSR)
        permisos[1] = 'r'; /*propietario*/
    if (m & S_IWUSR)
        permisos[2] = 'w';
    if (m & S_IXUSR)
        permisos[3] = 'x';
    if (m & S_IRGRP)
        permisos[4] = 'r'; /*grupo*/
    if (m & S_IWGRP)
        permisos[5] = 'w';
    if (m & S_IXGRP)
        permisos[6] = 'x';
    if (m & S_IROTH)
        permisos[7] = 'r'; /*resto*/
    if (m & S_IWOTH)
        permisos[8] = 'w';
    if (m & S_IXOTH)
        permisos[9] = 'x';
    if (m & S_ISUID)
        permisos[3] = 's'; /*setuid, setgid y stickybit*/
    if (m & S_ISGID)
        permisos[6] = 's';
    if (m & __S_ISVTX)
        permisos[9] = 't';

    return permisos;
}

void createDir(char *name)
{
    if (mkdir(name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) // los permisos son rwxrwxr-x
        perror("Imposible crear directorio");
}

void createFile(char *name)
{
    int df;

    df = open(name, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); // utilizamos el modo de o_create con o_excl para que en el caso de existir un fichero existente con ese nombre, nos salte errno

    if (df == (-1))
        perror("No se pudo crear fichero");
    else
        close(df);
}

void delete(char *name)
{
    struct stat sb;
    char tipo;

    if (lstat(name, &sb) != 0) // guardamos en sb los datos de lstat relacionados a 'name'
        perror("Imposible borrar");
    else
    {
        tipo = LetraTF(sb.st_mode); // llamamos a LetraTF para saber si es fichero o directorio

        if (tipo == 'd')
        {
            if (rmdir(name)) // rmdir: borra el directorio vacío
                perror("No se pudo borrar");
        }
        else if (tipo == '-')
        {
            if (unlink(name))
                perror("No se pudo borrar");
        }
    }
}

void deleteTree(char *name)
{
    struct stat sb;
    struct dirent *entrada;
    char tipo;
    DIR *directorio;

    if (lstat(name, &sb) != 0) // guardamos en sb los datos de lstat relacionados a 'name'
        perror("Imposible borrar");
    else
    {
        tipo = LetraTF(sb.st_mode); // llamamos a LetraTF para saber si es fichero o directorio

        if (tipo == 'd') // caso de que name sea un directorio
        {
            directorio = opendir(name); // abrimos el directorio y guardamos en 'directorio', si la función da error: directorio = NULL

            if (directorio == NULL)
                perror("Imposible borrar");
            else
            {
                while ((entrada = readdir(directorio)) != NULL) // readdir devuelve un puntero a un registro que representa la siguiente entrada del directorio, devuelve nulo si llega al final del directorio o si ocurre un error
                {
                    if (strcmp(".", entrada->d_name) == 0 || strcmp("..", entrada->d_name) == 0)
                        continue;

                    char filename[strlen(name) + strlen(entrada->d_name) + 5]; // creamos un string de tamaño suficientemente grande
                    sprintf(filename, "%s/%s", name, entrada->d_name);         // guardamos en el string que acabamos de crear el camino en el que se ubica el fichero/directorio: ej-> name = "hola", entrada->d_name = adios; filename = "hola/adios". Esto se hace ya que vamos a estar ubicados fuera del directorio que queremos borrar
                    lstat(filename, &sb);                                      // obtenemos los datos de la entrada del directorio actual
                    tipo = LetraTF(sb.st_mode);

                    if (tipo == 'd')
                        deleteTree(filename); // si es un directorio, llamamos a deleteTree de manera recursiva
                    else
                        unlink(filename); // si es un fichero hacemos unlink
                }
                rmdir(name); // una vez que llegamos al final del directorio (readdir == NULL) podemos eliminar la carpeta que ahora está vacía
            }
            closedir(directorio); // llamamos a closedir aunque la carpeta ya esté borrada, porque directorio tiene memoria dinámica asignada y closedir la liberará
        }
        else if (tipo == '-') // caso de que sea un fichero hacemos unlink
            unlink(name);
    }
}

void FileDirInformation(command argumentos)
{
    struct stat s;
    bool longg = false, acc = false, link = 0;
    struct tm *fecha;
    char *trozos[COMMAND_LENGTH], permisos[COMMAND_LENGTH];
    int i;
    char longFecha[COMMAND_LENGTH], hardlink[COMMAND_LENGTH], inode[COMMAND_LENGTH], *symbolicLink;

    trocearCadena(argumentos, trozos);

    for (i = 1; trozos[i] != NULL; i++)
    {
        if (!strcmp(trozos[i], "-long"))
            longg = true;
        if (!strcmp(trozos[i], "-link"))
            link = true;
        if (!strcmp(trozos[i], "-acc"))
            acc = true;
    }

    i = 0;
    while (trozos[i] != NULL)
    {
        if (trozos[i][0] != '-' && strcmp(trozos[i], "stat")) // Con esto indicamos que estamos en el nombre de un fichero o directorio
        {
            if (lstat(trozos[i], &s))
                perror("No se puede listar contenido");
            else
            {

                if (!longg) // Cuando solo está stat, o están las otras opciones sin este, ya que no funcionan sin -long
                    printf("%10lu  %.50s\n", s.st_size, trozos[i]);
                else
                {
                    if (!acc)
                        fecha = localtime(&s.st_mtime); // si -acc no está activado obtenemos el tiempo basado en la última modificación
                    else
                        fecha = localtime(&s.st_atime); // si -acc está activado obtenemos el tiempo basado en el último acceso

                    strcpy(permisos, ConvierteModo2(s.st_mode)); // obtenemos los permisos en formato string "rwxrwxrwxsst"

                    sprintf(longFecha, "%d/%d/%d-%d:%d", (fecha->tm_year + 1900), fecha->tm_mon + 1, fecha->tm_mday, fecha->tm_hour, fecha->tm_min); // formato de la fecha
                    sprintf(hardlink, "%ld", s.st_nlink);                                                                                            // número de hardlinks
                    sprintf(inode, "(%ld)", s.st_ino);                                                                                               // número inodo

                    if (link && LetraTF(s.st_mode) == 'l') // si la opción -link está activa
                    {
                        symbolicLink = malloc(s.st_size + 1);
                        readlink(trozos[i], symbolicLink, s.st_size + 1); // guardamos en symbolicLink el nombre del fichero al que apunta la entrada actual
                        symbolicLink[s.st_size] = '\0';
                        printf("%20s%5s%10s%10s%10s %10s%10lu  %.50s -> %s\n", longFecha, hardlink, inode, getpwuid(s.st_uid)->pw_name, getgrgid(s.st_gid)->gr_name, permisos, s.st_size, trozos[i], symbolicLink);
                        free(symbolicLink);
                    }
                    else
                        printf("%20s%5s%10s%10s%10s %10s%10lu  %.50s\n", longFecha, hardlink, inode, getpwuid(s.st_uid)->pw_name, getgrgid(s.st_gid)->gr_name, permisos, s.st_size, trozos[i]);
                }
            }
        }
        i++;
    }
}

void listContent(command argumentos, bool longg, bool recb, bool reca, bool hid, bool link, bool acc)
{
    struct stat sb;
    struct dirent *entrada;
    struct tm *fecha;
    DIR *directorio, *subdirectorio;
    char *trozos[COMMAND_LENGTH], permisos[COMMAND_LENGTH];
    int i;
    char longFecha[COMMAND_LENGTH], hardlink[COMMAND_LENGTH], inode[COMMAND_LENGTH], *symbolicLink;

    trocearCadena(argumentos, trozos);

    for (i = 1; trozos[i] != NULL; i++) // comprobamos qué opciones nos están pidiendo
    {
        if (!strcmp(trozos[i], "-long"))
            longg = true;
        if (!strcmp(trozos[i], "-recb"))
        {
            recb = true;
            reca = false;
        }
        if (!strcmp(trozos[i], "-reca"))
        {
            reca = true;
            recb = false;
        }
        if (!strcmp(trozos[i], "-hid"))
            hid = true;
        if (!strcmp(trozos[i], "-link"))
            link = true;
        if (!strcmp(trozos[i], "-acc"))
            acc = true;
    }

    i = 0;
    while (trozos[i] != NULL) // recorremos todos los trozos del comando
    {
        if (trozos[i][0] != '-' && strcmp(trozos[i], "list")) // si el primer char del trozo es - , significa que no es un nombre de fichero/directorio
        {
            if (lstat(trozos[i], &sb)) // obtenemos los datos del nombre pasado ya sea fichero o directorio
                perror("No se puede listar contenido");
            else
            {
                if ((directorio = opendir(trozos[i])) == NULL)
                {
                    i++;
                    continue;
                }

                if (recb) // recb, ya que es una función recursiva que debe printear los contenidos antes del directorio, lo situamos antes de printear las entradas del directorio
                {
                    subdirectorio = opendir(trozos[i]);
                    while ((entrada = readdir(subdirectorio)) != NULL)
                    {
                        if (!hid) // si la opción hid está activada, recorrerá los ficheros ocultos (empiezan por '.')
                        {
                            if (entrada->d_name[0] == '.')
                                continue;
                        }

                        char filename[strlen(trozos[i]) + strlen(entrada->d_name) + 5]; // creamos un string de tamaño suficientemente grande
                        sprintf(filename, "%s/%s", trozos[i], entrada->d_name);         // guardamos en el string que acabamos de crear el camino en el que se ubica el fichero/directorio: ej-> name = "hola", entrada->d_name = adios; filename = "hola/adios". Esto se hace ya que vamos a estar ubicados fuera del directorio que queremos borrar
                        lstat(filename, &sb);

                        if (LetraTF(sb.st_mode) == 'd' && entrada->d_name[0] != '.')  // caso de que la entrada sea un directorio y no sea "." o ".."
                            listContent(filename, longg, recb, reca, hid, link, acc); // llamamos a la función pasándole el subdirectorio y las opciones
                    }
                    closedir(subdirectorio);
                } // final recb

                printf("************%s\n", trozos[i]); // printeamos cabecera

                while ((entrada = readdir(directorio)) != NULL)
                {
                    if (!hid) // si opción hid está activada mostrará los ficheros ocultos, si no se los salta con "continue"
                    {
                        if (entrada->d_name[0] == '.')
                            continue;
                    }

                    char filename[strlen(trozos[i]) + strlen(entrada->d_name) + 5]; // creamos un string de tamaño suficientemente grande. Le sumamos 5 para que filename tenga un poco más del tamaño necesario, ya que es necesario esto para el sprintf.
                    sprintf(filename, "%s/%s", trozos[i], entrada->d_name);         // guardamos en el string que acabamos de crear el camino en el que se ubica el fichero/directorio: ej-> name = "hola", entrada->d_name = adios; filename = "hola/adios". Esto se hace ya que vamos a estar ubicados fuera del directorio que queremos borrar
                    lstat(filename, &sb);

                    if (!longg)
                        printf("%10lu %.50s\n", sb.st_size, entrada->d_name); // caso base en el que -long no está pedido
                    else                                                      // caso de que -long esté activado
                    {
                        if (!acc)
                            fecha = localtime(&sb.st_mtime); // si -acc no está activado obtenemos el tiempo basado en la última modificación
                        else
                            fecha = localtime(&sb.st_atime); // si -acc está activado obtenemos el tiempo basado en el último acceso

                        strcpy(permisos, ConvierteModo2(sb.st_mode)); // obtenemos los permisos en formato string "rwxrwxrwxsst"

                        sprintf(longFecha, "%d/%d/%d-%d:%d", (fecha->tm_year + 1900), fecha->tm_mon + 1, fecha->tm_mday, fecha->tm_hour, fecha->tm_min); // formato de la fecha
                        sprintf(hardlink, "%ld", sb.st_nlink);                                                                                           // número de hardlinks
                        sprintf(inode, "(%ld)", sb.st_ino);                                                                                              // número inodo

                        if (link && LetraTF(sb.st_mode) == 'l') // si la opción -link está activa y el modo indica que es un link simbólico
                        {
                            symbolicLink = malloc(sb.st_size + 1);
                            readlink(filename, symbolicLink, sb.st_size + 1); // guardamos en symbolicLink el nombre del fichero al que apunta la entrada actual
                            symbolicLink[sb.st_size] = '\0';
                            printf("%20s%5s%10s%10s%10s %10s%10lu  %.50s -> %s\n", longFecha, hardlink, inode, getpwuid(sb.st_uid)->pw_name, getgrgid(sb.st_gid)->gr_name, permisos, sb.st_size, entrada->d_name, symbolicLink);
                            free(symbolicLink);
                        }
                        else
                            printf("%20s%5s%10s%10s%10s %10s%10lu  %.50s\n", longFecha, hardlink, inode, getpwuid(sb.st_uid)->pw_name, getgrgid(sb.st_gid)->gr_name, permisos, sb.st_size, entrada->d_name);
                    }
                }
                if (reca) // reca, ya que es una función recursiva que debe printear los contenidos después del directorio, lo situamos después de printear las entradas del directorio
                {
                    subdirectorio = opendir(trozos[i]);
                    while ((entrada = readdir(subdirectorio)) != NULL)
                    {
                        if (!hid)
                        {
                            if (entrada->d_name[0] == '.')
                                continue;
                        }

                        char filename[strlen(trozos[i]) + strlen(entrada->d_name) + 5]; // creamos un string de tamaño suficientemente grande
                        sprintf(filename, "%s/%s", trozos[i], entrada->d_name);         // guardamos en el string que acabamos de crear el camino en el que se ubica el fichero/directorio: ej-> name = "hola", entrada->d_name = adios; filename = "hola/adios". Esto se hace ya que vamos a estar ubicados fuera del directorio que queremos borrar
                        lstat(filename, &sb);

                        if (LetraTF(sb.st_mode) == 'd' && entrada->d_name[0] != '.')
                            listContent(filename, longg, recb, reca, hid, link, acc);
                    }
                    closedir(subdirectorio);
                }
                closedir(directorio);
            }
        }
        i++;
    }
}
