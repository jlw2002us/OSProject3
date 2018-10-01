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

struct Memory {
     //int  status;
     long long int  seconds;
               long long int  nanoseconds;
     long int childpid;
               };
     
               
int main()
{
  
//    pid_t pid;
    key_t          ShmKEY;
    int            ShmID;
    struct Memory  *ShmPTR;
     ShmKEY = ftok("/dev/null",5);
    ShmID = shmget(ShmKEY, sizeof(struct Memory), 0644);
    if (ShmID < 0) {
          printf("*** shmget error (client) ***\n");
          exit(1);
     }
     ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
    if (ShmPTR ==  NULL) {
          printf("*** shmat error (client) ***\n");
          exit(1);
      }
   int x = 0;
   while(x < 10000){//printf("hi");
   sem_t *sem = sem_open("pSem", 0);
   sem_wait (sem);           /* P operation */
       printf ("  Child is in critical section.\n");
        
        if(ShmPTR->childpid == 0){
        printf("child pid %ld", (long)getpid());
        ShmPTR->childpid = (long)getpid();
        sem_post(sem);
        sem_close(sem);
        break;}
        sem_post (sem);
        sem_close(sem);
        x++;}   
   shmdt((void *) ShmPTR);
    return(0);

}
