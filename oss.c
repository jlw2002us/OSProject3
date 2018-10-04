#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <sys/shm.h>        /* shmat(), IPC_RMID        */
#include <errno.h>          /* errno, ECHILD            */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <time.h>
#include <unistd.h>
#include <wait.h>

struct shmMsg{

   long long int Usernanoseconds;
   long int childpid;
      };

struct Memory{
  long long int nanoseconds;
  long long int seconds;
  struct shmMsg shmmsg;

};
struct Memory *shmPTR;
bool signal_interrupt = false;
int shmid;
sem_t *sem;
bool ChildExceeded = false;


void  ALARMhandler(int sig)
{ if(signal_interrupt == false)
  write (STDOUT_FILENO,"Alarm went off\n",16);
        signal(SIGALRM, SIG_IGN);
   signal_interrupt = true;
  signal(SIGALRM, ALARMhandler);

}

void  sigtermhandler(int sig)
{
  write (STDOUT_FILENO,"Process terminated\n",16);
  shmdt (shmPTR);
  shmctl (shmid, IPC_RMID, 0);
  sem_unlink ("pSem3");
 sem_close(sem);

exit(0);


}

void helpoptions()
{
  printf("options for running Project 3:  \n");
  printf("type ./parent -h for help options\n");
  printf("type ./parent -s with value for how many processes you want to fork");

}

int main (int argc, char **argv){
    int i;                        /*      loop variables          */
    key_t shmkey;
    FILE *fp;                 /*      shared memory key       */
    int Userseconds = 0;
    pid_t pid;                    /*      fork pid                */
    int childCount = 0;                     /*      shared variable         *//*shared */

 signal(SIGALRM, ALARMhandler);
 int x;
    //            //alarm(4);
 signal(SIGTERM, sigtermhandler);
 int c,z;
 int hflag;
 int index;
 char *xvalue = NULL; //how many child processes user enters
 char *zvalue = NULL; //how many seconds for the alarm 
 int status = 2;
 char* filename;
 opterr = 0;

while ((c = getopt (argc, argv, "hs:t:l:")) != -1){
      switch (c)
       {
         case 'h':
         hflag = 1;
         break;
 case 'l':
            filename = optarg;
            break;

         case 't':
           zvalue = optarg;
           break;
         case 's':
           xvalue = optarg;
           break;
         case '?':
          if (optopt == 'l')
           fprintf (stderr, "Option -%c requires an argument.\n", optopt);
          else if (isprint (optopt))
           fprintf (stderr, "Unknown option `-%c'.\n", optopt);
          else
           fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
          return 1;
          default:
            abort ();
      }
    }

    for (index = optind; index < argc; index++)
      printf ("Non-option argument %s\n", argv[index]);

     if(hflag == 1)
     {
       helpoptions();
       return 1;
     }
if(xvalue != NULL){

         x = atoi(xvalue);
         if (x == 0){
           printf("Error:  non-numerical data entered\n");
           return 1;}
       }
      else{
        printf("%s","Error: no s value  entered\n");
        return 1;}
 if(zvalue!= NULL){

         z  = atoi(zvalue);
         if (z == 0){
           printf("Error:  non-numerical data entered\n");
           return 1;}
          }
         else{
          printf("%s","Error: no t value entered\n");
         return 1;
         }

    alarm(z);
    fp= fopen(filename, "a");//open the logfile for writing
    if (fp == NULL)
    {
       printf("Error opening file!\n");
       exit(1);
    }

     shmkey = ftok (".", 'x');       /* valid directory name and a number */
    //printf ("shmkey for p = %d\n", shmkey);
    shmid = shmget (shmkey, sizeof (struct Memory), 0666 | IPC_CREAT);
    if (shmid < 0){                           /* shared memory error check */
                   perror ("shmget\n");
                            exit (1);
                                }
    //
    shmPTR  = (struct Memory *) shmat (shmid, NULL, 0);   /* attach p to shared memory */
    shmPTR->seconds = 0;
    shmPTR->nanoseconds = 0;

    shmPTR->shmmsg.Usernanoseconds = 0;
    shmPTR->shmmsg.childpid = 0;
    sem = sem_open ("pSem3", O_CREAT | O_EXCL, 0644, 1);
     printf ("semaphores initialized.\n\n");
    sem_close(sem);
    for (i = 0; i < x; i++){
        pid = fork ();
        childCount++;
        if (pid < 0) {
            sem_unlink ("pSem3");
            sem_close(sem);
            printf ("Fork error.\n");
        }
  else if (pid == 0){
             char *args[]={"./user",NULL};
             execvp(args[0],args);}
    }
    while(signal_interrupt == false){
    if (pid != 0){

      while(shmPTR->seconds < 2){

        if(signal_interrupt == true)break;
        if(childCount > 100)
          { ChildExceeded = true;
           printf("child count exceeded\n");
             break;}
         sem = sem_open("pSem3",0);
        sem_wait(sem);
        if (shmPTR->shmmsg.childpid != 0){
             fprintf(fp, "OSS: Child %ld is terminating at my time %lld s %lld ns", shmPTR->shmmsg.childpid,shmPTR->seconds, shmPTR->nanoseconds);
           //sem = sem_open("pSem3",0);
           //sem_wait(sem);
           printf("OSS: Child %ld is terminating at my time %lld s %lld ns", shmPTR->shmmsg.childpid,shmPTR->seconds, shmPTR->nanoseconds);
           while(shmPTR->shmmsg.Usernanoseconds >= 1000000000)
           {
               Userseconds++;
               shmPTR->shmmsg.Usernanoseconds = shmPTR->shmmsg.Usernanoseconds - 1000000000;
           }
            fprintf(fp, "because it reached %d s %lld ns in user\n", Userseconds, shmPTR->shmmsg.Usernanoseconds);


           printf(" because it reached %d s %lld ns in user\n", Userseconds, shmPTR->shmmsg.Usernanoseconds);
           Userseconds = 0;
           waitpid(shmPTR->shmmsg.childpid, &status, 0);
           //printf("%d\n",status);         
           shmPTR->shmmsg.childpid = 0;

           shmPTR->shmmsg.Usernanoseconds = 0;
           sem_post(sem);
           sem_close(sem);
           pid = fork();childCount++;
           if (pid == 0){
             char *args[]={"./user",NULL};
              execvp(args[0],args); printf("exec error");}}
      else{ sem_post(sem); sem_close(sem);}
      sem = sem_open("pSem3",0);
      sem_wait(sem);
      shmPTR->nanoseconds = shmPTR->nanoseconds + 15000;
           if(shmPTR->nanoseconds>= 1000000000)
           {
             shmPTR->seconds = shmPTR->seconds + 1;
             shmPTR->nanoseconds = shmPTR->nanoseconds - 1000000000;}
        sem_post(sem);
        sem_close(sem);}
      
       if((ChildExceeded == false)&&(signal_interrupt == false)) { printf("Simulated time exceeded..\n");}
       if (signal_interrupt == true){  break;}

     do{ if(signal_interrupt == true) break;
       printf("Clock ticking..\n");
       sleep(1);
        }while (true);


      }

     }

         fclose(fp);

        killpg(getpgid(getpid()), SIGTERM);

        exit (0);


}

