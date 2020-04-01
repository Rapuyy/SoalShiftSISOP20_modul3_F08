#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
 
int main()
{
    key_t key = 1234;
    int *value;
    int multiply[4][5];
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    value = shmat(shmid, NULL, 0);

    int c, d, k;
    int sum = 0;
    int first[4][2] = {{1, 2},
                    {3, 4},
                    {5, 6},
                    {7, 8}};
    int second[2][5] = {{1, 2, 3, 4, 5},
                      {6, 7, 8, 9, 10}};
    // int multiply[4][5];



    for (c = 0; c < 4; c++) {
      for (d = 0; d < 5; d++) {
        for (k = 0; k < 2; k++) {
          sum = sum + first[c][k]*second[k][d];
        }
 
        multiply[c][d] = sum;
        sum = 0;
      }
    }
 
    printf("Product of the matrices:\n");
 
    for (c = 0; c < 4; c++) {
      for (d = 0; d < 5; d++){
      *value = multiply[c][d];
      sleep(1);
      printf("terkirimz %d \n", *value);
      }
    }
    shmdt(value);
    shmctl(shmid, IPC_RMID, NULL);

	return 0;
}