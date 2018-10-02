
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

struct Memory{
  long long int nanoseconds;
  long long int seconds;
  long int childpid;
};
struct Memory *shmPTR;
bool signal_interrupt = false;
int shmid;
sem_t *sem;

//catch alarm 
void  ALARMhandler(int sig)
{ if(signal_interrupt == false)
  write (STDOUT_FILENO,"Alarm went off\n",16);
        signal(SIGALRM, SIG_IGN);
   signal_interrupt = true;
  signal(SIGALRM, ALARMhandler);
       
}

//SIGTERM handler
void  sigtermhandler(int sig)
{ 
  write (STDOUT_FILENO,"Process terminated",16);
           shmdt (shmPTR);
        shmctl (shmid, IPC_RMID, 0);

        /* cleanup semaphores */
        sem_unlink ("pSem3");   
        sem_close(sem);  
        /* unlink prevents the semaphore existing forever */
        /* if a crash occurs during the execution         */
    exit(0);
      

}

int main (int argc, char **argv){
    int i;                        /*      loop variables          */
    key_t shmkey;                 /*      shared memory key       */
                        
                       /*      synch semaphore         *//*shared */
    pid_t pid;                    /*      fork pid                */
    int childCount = 0;                     /*      shared variable         *//*shared */
    unsigned int n;               /*      fork count              */
    unsigned int value;
    signal(SIGALRM, ALARMhandler);

    alarm(10);
    signal(SIGTERM, sigtermhandler);
    /* initialize a shared variable in shared memory */
    shmkey = ftok (".", 'x');       /* valid directory name and a number */
    printf ("shmkey for p = %d\n", shmkey);
    shmid = shmget (shmkey, sizeof (struct Memory), 0666 | IPC_CREAT);
    if (shmid < 0){                           /* shared memory error check */
        perror ("shmget\n");
        exit (1);
    }

    shmPTR  = (struct Memory *) shmat (shmid, NULL, 0);   /* attach p to shared memory */
    shmPTR->seconds = 0;
    shmPTR->childpid = 0;
    shmPTR->nanoseconds = 0;
//    printf ("p=%d is allocated in shared memory.\n\n", *p);

    /********************************************************/

    printf ("How many children do you want to fork?\n");
    printf ("Fork count: ");
    scanf ("%u", &n);

    printf ("What do you want the semaphore value to be?\n");
    printf ("Semaphore value: ");
    scanf ("%u", &value);

    /* initialize semaphores for shared processes */
    sem = sem_open ("pSem3", O_CREAT | O_EXCL, 0644, value); 
     printf ("semaphores initialized.\n\n");
    sem_close(sem);

    /* fork child processes */
    for (i = 0; i < n; i++){
        pid = fork ();
        childCount++;
        if (pid < 0) {
        /* check for error      */
            sem_unlink ("pSem");   
            sem_close(sem);  
            /* unlink prevents the semaphore existing forever */
            /* if a crash occurs during the execution         */
            printf ("Fork error.\n");
        }
        else if (pid == 0){
             char *args[]={"./user",NULL}; 
        execvp(args[0],args);} 

                  /* child processes */
    }

    while(signal_interrupt == false){
    /******************************************************/
    /******************   PARENT PROCESS   ****************/
    /******************************************************/
    if (pid != 0){
        /* wait for all children to exit */
        //for( i = 0; i < n; i++){
//          wait(NULL);//}
  //    wait(NULL);
       while(shmPTR->nanoseconds < 2000000000){
         if(signal_interrupt == true) break;
       if (shmPTR->childpid !=0){

         sem = sem_open("pSem3",0);
         sem_wait(sem);
         printf("childpid is %ld\n" , shmPTR->childpid);
          wait(NULL);
          childCount--;        
          shmPTR->childpid = 0;
          sem_post(sem);
          sem_close(sem);}
           
          shmPTR->nanoseconds = shmPTR->nanoseconds + 5;}
       
       if (signal_interrupt == true){  break;}
       
       do{ if(signal_interrupt == true) break; 
       printf("Clock ticking..\n");
       sleep(1);
        }while (true);


       }
        
    }
   for (i =0; i <childCount; i++){
         printf("hi");  wait(NULL);
        }

        killpg(getpgid(getpid()), SIGTERM);

        exit (0);

    
}
