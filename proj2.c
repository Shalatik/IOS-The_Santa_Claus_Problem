/* ***************************************************************************************
* projekt: IOS 2. projekt (Santa Claus problem)
* autor: Simona Ceskova (xcesko00)
* datum: 02.05.2021
*************************************************************************************** */

#include "proj2.h"

int main(int argc, char **argv)
{
    //kontroluji spravnost argumentu
    check(argc, argv);
    int a = argc;
    a++;
    char *next;
    int NE = strtol(argv[1], &next, 10); //elf
    int NR = strtol(argv[2], &next, 10); //sob RD
    int TE = strtol(argv[3], &next, 10); //elf pracuje
    int TR = strtol(argv[4], &next, 10); //dovolena

    if ((out_file = fopen("proj2.out", "w+")) == NULL)
    {
        fprintf(stderr, "Chyba pri zapisu do proj2.out .\n");
        exit(1);
    }

    //vytvoreni semaforu a sdilenych promennych
    semaphoreStart();
    semaphoreInit();

    //vytvoreni procesu
    pid_t fork1 = fork();
    if (fork1 == 0)
    {
        santaProcess(NR);
    }
    else if (fork1 > 0)
    {
        elvesProcess(TE, NE);
        reindeerProcess(NR, TR);
    }
    else
    {
        //v pripade chyby
        semaphoreEnd();
        semaphoreDestroy();
        endProcess();
    }

    pid_t wpid;
    while ((wpid = wait(NULL)) > 0)
        ;

    //ukonceni procesu a semaforu
    semaphoreEnd();
    semaphoreDestroy();

    fclose(out_file);
    exit(0);
}

void check(int argc, char **argv)
{
    //kontrola vztupnich parametru

    char *next;
    if (argc != 5)
    {
        fprintf(stderr, "Chyby pocet parameu\n");
        exit(1);
    }
    for (int i = 1; i < argc; i++)
    {
        long x = strtol(argv[i], &next, 10);
        if ((x < 0) || (x > 1000) || (*next != '\0'))
        {
            fprintf(stderr, "Chybny parametr\n");
            exit(1);
        }
    }
    int x = strtol(argv[2], argv, 10);
    if ((x > 20) || (x == 0))
    {
        fprintf(stderr, "Chybny parametr\n");
        exit(1);
    }
    x = strtol(argv[1], argv, 10);
    if ((x > 1000) || (x == 0))
    {
        fprintf(stderr, "Chybny parametr\n");
        exit(1);
    }
}

void errorPrint()
{
    fprintf(stderr, "Error\n");
    exit(1);
}

void semaphoreInit()
{
    //deklarovani vsech semaforu

    if ((sem_init(sem, 1, 1)) == -1)
        errorPrint();
    if ((sem_init(lastRDhitched, 1, 0)) == -1)
        errorPrint();
    if ((sem_init(sem2, 1, 1)) == -1)
        errorPrint();
    if ((sem_init(elvesHelp, 1, 0)) == -1)
        errorPrint();
    if ((sem_init(RDwaitingForHitch, 1, 0)) == -1)
        errorPrint();
    if ((sem_init(santa, 1, 0)) == -1)
        errorPrint();
    if ((sem_init(printOutputFile, 1, 1)) == -1)
        errorPrint();
    if ((sem_init(afterHelpingElves, 1, 0)) == -1)
        errorPrint();
}

void semaphoreStart()
{
    //alokovani pameni pro sdilene promenne a semafory

    if ((sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((lastRDhitched = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((sem2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((elvesHelp = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((elves_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((RD_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((RDwaitingForHitch = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((santa = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((printOutputFile = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((processCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((closed = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((afterHelpingElves = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
    if ((numberOfHelp = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
        errorPrint();
}

void semaphoreEnd()
{
    //uvolneni pameti semaforu a sdilenych promenych

    if (munmap(sem, sizeof(sem_t)) == -1)
        errorPrint();
    if (munmap(lastRDhitched, sizeof(sem_t)) == -1)
        errorPrint();
    if (munmap(elvesHelp, sizeof(sem_t)) == -1)
        errorPrint();
    if (munmap(RDwaitingForHitch, sizeof(sem_t)) == -1)
        errorPrint();
    if (munmap(printOutputFile, sizeof(sem_t)) == -1)
        errorPrint();
    if (munmap(santa, sizeof(sem_t)) == -1)
        errorPrint();
    if (munmap(processCounter, sizeof(int)) == -1)
        errorPrint();
    if (munmap(elves_counter, sizeof(int)) == -1)
        errorPrint();
    if (munmap(RD_counter, sizeof(int)) == -1)
        errorPrint();
    if (munmap(closed, sizeof(int)) == -1)
        errorPrint();
    if (munmap(afterHelpingElves, sizeof(sem_t)) == -1)
        errorPrint();
    if (munmap(numberOfHelp, sizeof(int)) == -1)
        errorPrint();
}

void semaphoreDestroy()
{
    //zniceni semaforu

    sem_destroy(sem);
    sem_destroy(lastRDhitched);
    sem_destroy(sem2);
    sem_destroy(elvesHelp);
    sem_destroy(RDwaitingForHitch);
    sem_destroy(printOutputFile);
    sem_destroy(santa);
}

void endProcess()
{
    //pri chybovym vystupu

    fprintf(stderr, "Error\n");
    fclose(out_file);
    exit(1);
}

void outputPrint(char *processName, char *processState, int processNumber)
{
    //pojistka aby do souboru nezapisovaly dva procesy zaraz
    sem_wait(printOutputFile);
    //pocitac procesu
    *processCounter += 1;
    fprintf(out_file, "%i: %s %i: %s\n", *processCounter, processName, processNumber, processState);
    fflush(out_file);
    //jakmile dopise do souboru tak zase uvolni sem_wait() pro dalsi proces
    sem_post(printOutputFile);
}

void outputPrintSanta(char *processName, char *processState)
{
    //vypis specialne jen pro santu, protoze je jen jeden

    //pojistka aby do souboru nezapisovaly dva procesy zaraz
    sem_wait(printOutputFile);
    //pocitac procesu
    *processCounter += 1;
    fprintf(out_file, "%i: %s: %s\n", *processCounter, processName, processState);
    fflush(out_file);
    //jakmile dopise do souboru tak zase uvolni sem_wait() pro dalsi proces
    sem_post(printOutputFile);
}

void santaProcess(int NR)
{
    outputPrintSanta("Santa", "going to sleep");

    while (1)
    {
        //cekani na podmet od sobu nebo elfu
        sem_wait(santa);
        //posledni sob je doma
        if (*RD_counter == NR)
        {
            break;
        }
        //tri elfove stoji pred dilnou
        if (*elves_counter == 3)
        {
            outputPrintSanta("Santa", "helping elves");
            *elves_counter = 0;

            //3 elfove dostanou pomoc
            sem_post(elvesHelp);
            sem_post(elvesHelp);
            sem_post(elvesHelp);

            //ceka nez dostanou 3 elfove pomoc
            sem_wait(afterHelpingElves);
            *numberOfHelp = 3;

            outputPrintSanta("Santa", "going to sleep");
        }
    }
    outputPrintSanta("Santa", "closing workshop");
    //cedule "zavreno"
    *closed = 1;

    sem_post(elvesHelp);
    //zaprahuje soby
    sem_post(RDwaitingForHitch);
    //az se posledni sob zaprahne zacinaji vanoce
    sem_wait(lastRDhitched);
    outputPrintSanta("Santa", "Christmas started");
    exit(0);
}

void elvesProcess(int TE, int NE)
{
    *numberOfHelp = 3;
    for (int i = 0; i < NE; i++)
    {
        pid_t fork_id = fork();
        if (fork_id == 0)
        {
            outputPrint("Elf", "started", i + 1);
            while (1)
            {
                //cas kdy pracuji elfove sami
                if (TE != 0)
                {
                    int runTime = rand() % TE + 1;
                    usleep(runTime * 1000);
                }
                outputPrint("Elf", "need help", i + 1);

                sem_wait(sem);

                //zavrena dilna
                if (*closed == 1) 
                {
                    sem_post(sem);
                    sem_post(elvesHelp);
                    break;
                }

                *elves_counter += 1;
                if (*elves_counter == 3)
                    sem_post(santa); //vzbudi santu a vypise "heping elves"
                else
                    sem_post(sem); //pusti dalsiho elfa
                //tady se nahromadi vsichni 3 a cekaji na santu
                //cekaji dokud santa nevypise "helping elves"
                sem_wait(elvesHelp);

                //zavrena dilna
                if (*closed == 1) 
                {
                    sem_post(sem);
                    sem_post(elvesHelp);
                    break;
                }
                outputPrint("Elf", "get help", i + 1);
                //postupne se zacinaji odcitat
                sem_wait(sem2);
                *numberOfHelp -= 1;
                if (*numberOfHelp == 0)
                {
                    sem_post(afterHelpingElves); //vypise ze jde spat Santa
                    //doslo k odbaveni jedne skupinky elfu a posledni pousti dalsi
                    sem_post(sem);
                }
                sem_post(sem2); 
            }
            outputPrint("Elf", "taking holidays", i + 1);

            exit(0);
        }
    }
}

void reindeerProcess(int NR, int TR)
{
    for (int i = 0; i < NR; i++)
    {
        pid_t fork_id = fork();
        if (fork_id == 0)
        {
            outputPrint("RD", "rstarted", i + 1);

            //uspani sobu behem dovolene
            if (TR != 0)
            {
                usleep(rand() % (TR / 2 * 1000) + TR / 2);
            }

            outputPrint("RD", "return home", i + 1);
            *RD_counter += 1;
            //hledam posledniho soba na vzbuzeni Santy
            if ((*RD_counter) == NR)
            {
                sem_post(santa);
            }

            //cekani na santu az zavre dilnu a zaprahne je
            sem_wait(RDwaitingForHitch);
            sem_post(RDwaitingForHitch);
            outputPrint("RD", "get hitched", i + 1);
            //cekam na posledniho soba az se zaprahne
            if ((*RD_counter -= 1) == 0)
            {
                //Vanoce muzou zacit
                sem_post(lastRDhitched);
            }
            exit(0);
        }
    }
}
