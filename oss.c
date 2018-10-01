#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <sys/shm.h>        /* shmat(), IPC_RMID        */
#include <errno.h>          /* errno, ECHILD            */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <stdint.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
//Sources used: https://stackoverflow.com/questions/16400820/how-to-use-posix-semaphores-on-forked-processes-in-c
//
struct Memory {
     //int  status;
     long long int  seconds;
     long long int  nanoseconds;
     long int childpid;
};

struct Memory  *ShmPTR;

int main (int argc, char **argv){
    int i;                        /*      loop variables          */
    key_t shmkey;                 /*      shared memory key       */
    int shmid;                    /*      shared memory id        */
    sem_t *sem;                   /*      synch semaphore         *//*shared */
    pid_t pid;                    /*      fork pid                */
                         /*      shared variable         *//*shared */
    unsigned int n;               /*      fork count              */
    unsigned int value;           /*      semaphore value         */

    /* initialize a shared variable in shared memory */
    shmkey = ftok ("/dev/null", 5);       /* valid directory name and a number */
    printf ("shmkey for p = %d\n", shmkey);
    shmid = shmget (shmkey, sizeof (struct Memory), 0644 | IPC_CREAT);
    if (shmid < 0){                           /* shared memory error check */
        perror ("shmget\n");
        exit (1);
    }

    ShmPTR  = (struct Memory *) shmat (shmid, NULL, 0);   
    ShmPTR->seconds = 100;
    ShmPTR->nanoseconds = 0;
    //printf ("p=%d is allocated in shared memory.\n\n", *p);

    /********************************************************/

    printf ("How many children do you want to fork?\n");
    printf ("Fork count: ");
    scanf ("%u", &n);

    printf ("What do you want the semaphore value to be?\n");
    printf ("Semaphore value: ");
    scanf ("%u", &value);

    /* initialize semaphores for shared processes */
    sem = sem_open ("pSem", O_CREAT | O_EXCL, 0644, value); 
    /* name of semaphore is "pSem", semaphore is reached using this name */
   sem_close(sem);
    printf ("semaphores initialized.\n\n");
     ShmPTR->childpid = 0;
     printf("%ld", ShmPTR->childpid);
    
    /* fork child processes */
    for (i = 0; i < n; i++){
        pid = fork ();
        if (pid < 0) {
        /* check for error      */
            sem_unlink ("pSem");   
            sem_close(sem);  
            /* unlink prevents the semaphore existing forever */
            /* if a crash occurs during the execution         */
            printf ("Fork error.\n");
         }
         else if (pid == 0){
             break;}         /* child processes */
     }

   //int counter = 0;
    /******************************************************/
    /******************   PARENT PROCESS   ****************/
    /******************************************************/
if (pid != 0){
      //  printf("hi");
        /* wait for all children to exit */
      //  while (pid = waitpid (-1, NULL, 0)){
        //  if (errno == ECHILD)
          //    break;}
      while(n  < 1000){
         
         if(ShmPTR->childpid != 0){ 
           sem = sem_open("pSem",0);
           sem_wait(sem);
           printf("Parent read childpid %ld ", ShmPTR->childpid);
           wait(NULL);
           ShmPTR->childpid = 0; 
          sem_post(sem);
           sem_close(sem);}n++;}
       
       
        //printf ("\nParent: All children have exited.\n");

        /* shared memory detach */
       
       shmdt (ShmPTR);
        shmctl (shmid, IPC_RMID, 0);

        /* cleanup semaphores */
        sem_unlink ("pSem");   
 //       sem_close(sem);  
        /* unlink prevents the semaphore existing forever */
        /* if a crash occurs during the execution         */
        exit (0);
    }
   

    /******************************************************/
    /******************   CHILD PROCESS   *****************/
    /******************************************************/
   else{
      //printf("hi");
      char *args[]={"./user",NULL}; 
       execvp(args[0],args);}
      
    }

