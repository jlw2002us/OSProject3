#include<string.h>
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
 sem_t* sem;



int main(){
  long long int Userseconds;
  long long int Usernanoseconds;
  key_t  ShmKEY;
  int ShmID;

  ShmKEY = ftok(".",'x');
  ShmID = shmget(ShmKEY, sizeof(struct Memory), 0666);
  if (ShmID < 0){
   printf("*** shmget error (client) ***\n");
   exit(1);
  }
  shmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
  if(ShmID == -1){
    printf("*** shmat error(client) ***\n");
    exit(1);
   }

    
    srand(getpid());
    long long int value = 0;
    value = 1 + (rand()%1000000);
    Userseconds = shmPTR->seconds;
    Usernanoseconds = 1000000000*Userseconds;
    Userseconds = 0; 
    Usernanoseconds = shmPTR->nanoseconds + value + Usernanoseconds;
    
    while(1){      
    //while(Usernanoseconds >= (1000000000*shmPTR->seconds)+shmPTR->nanoseconds){
                
      sem = sem_open("pSem3",0);
      sem_wait(sem);
      if((shmPTR->shmmsg.childpid == 0)&&(Usernanoseconds <= (1000000000*shmPTR->seconds)+shmPTR->nanoseconds)){
        shmPTR->shmmsg.childpid = (long)getpid();

        shmPTR->shmmsg.Usernanoseconds = Usernanoseconds;  
//        printf("nanoseconds %lld\n", shmPTR->shmmsg.Usernanoseconds);       
        sem_post(sem);
        sem_close(sem);
       shmdt((void *) shmPTR);
       break;}
      
      sem_post(sem);
      sem_close(sem);
      }
   exit(0);
}
    
