#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  ChildProcess(int SharedMem[]) {
  int i, accountID, random_num;
  srand(getpid());
  
  for(i = 0; i < 30; i++) {
    sleep(rand() % 6);
    accountID = SharedMem[0];
    
    while (SharedMem[1] != 1);
    random_num = rand() % 51;
    printf("Poor Student needs $%d\n", random_num);
    
    if (SharedMem[0] <= accountID) {
      accountID -= SharedMem[0];
      printf("Poor Student: Withdraws $%d / Balance = $%d\n", SharedMem[0], accountID);
      SharedMem[0] = accountID;
      
    }
    
    else {
      printf("Poor Student: Not Enough Cash ($%d)\n", accountID );
    }
    SharedMem[1] = 0;
  }
}

void ParentProcess(int SharedMem[]) {
  int i, accountID, random_num;
  srand(getpid()); 
  
  for(i = 0; i < 30; i++){
    sleep(rand() % 6);
    accountID = SharedMem[0];
    
    while(SharedMem[1] != 0);
    
    if (accountID <= 100) { 
      random_num = rand() % 101; 
      
      if (random_num % 2 == 0) { 
        accountID += random_num;
        
        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", SharedMem[0], accountID);
        
      }
      else {
        printf("Dear old Dad: Doesn't have any money to give\n");
      }
      SharedMem[0] = accountID; 
      SharedMem[1] = 1;
    } 
    else  {
      printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", accountID);
    }
  }
}

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;

     ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Server has received a shared memory of four integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Server has attached the shared memory...\n");
  
     ShmPTR[0] = 0; 
     ShmPTR[1] = 0; 
    
     printf("Main is about to fork a child process...\n");
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (Main) ***\n");
          exit(1);
     }
     else if (pid == 0) {
          ChildProcess(ShmPTR);
          exit(0);
     }
     else {
          ParentProcess(ShmPTR);
     }
     wait(&status);
     printf("Main has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("Main has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Main has removed its shared memory...\n");
     printf("Main exits...\n");
     exit(0);
}
