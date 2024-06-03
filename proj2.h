/* ***************************************************************************************
* projekt: IOS 2. projekt (Santa Claus problem)
* autor: Simona Ceskova (xcesko00)
* datum: 02.05.2021
*************************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

FILE *out_file; //soubor do ktereho zapisuji

//sdilene promenne
int *elves_counter = NULL;  //pocet elfu
int *RD_counter = NULL;     //pocet sobu
int *processCounter = NULL; //pocet vsech procesu
int *closed = NULL;         //cedule "zavreno"
int *numberOfHelp = NULL;   //pocet dostanych "get help"

sem_t *sem;                 //povoluje postupne scitani elfu
sem_t *lastRDhitched;       //posledni sob zaprahnuty, vanoce muzou zacit
sem_t *sem2;                //povoluje postupne odcitani elfu
sem_t *elvesHelp;           //Santa pomuze elfum z oni vzpisou "get help"
sem_t *afterHelpingElves;   //po tom co Santa pomuze elfum jde spat
sem_t *RDwaitingForHitch;   //povoluje sobum ze se uz muzou zaprahnout
sem_t *santa;               //vzbudi santu
sem_t *printOutputFile;     //kontroluje jestli nekdo nezapisuje do output filu naraz

/* ******** deklarovani funkci ****** */
void check();
void outputPrint(char *processName, char *processState, int processNumber);
void outputPrintSanta(char *processName, char *processState);
void reindeerProcess(int TR, int TE);
void elvesProcess(int NR, int NE);
void santaProcess(int NR);
void endProcess();
void semaphoreStart();
void semaphoreInit();
void semaphoreEnd();
void errorPrint();
void semaphoreDestroy();
/* ********************************** */