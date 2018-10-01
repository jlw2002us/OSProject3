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
  long long int seconds;
  long int childpid;
};
struct Memory *shmPTR;

int main(){
  sem_t* sem;
  key_t  ShmKEY;
  int ShmID;
   printf("%ld\n", (long)getpid()); 
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
   //int x = 0;
   while(1){   
   sem = sem_open("pSem3",0);
   sem_wait(sem);
    if(shmPTR->childpid == 0){
     shmPTR->childpid = (long)getpid();
          
     
    sem_post(sem);
     sem_close(sem);
     break;}
     sem_post(sem);
     sem_close(sem);}
   exit(0);
}
    
  
