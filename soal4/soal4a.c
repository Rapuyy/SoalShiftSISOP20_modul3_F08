#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
 
int main()
{
    key_t key = 1234;
    int *value;
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    value = shmat(shmid, NULL, 0);

    int multiply[4][5];

    int baris, kolom, tengah;
    int sum = 0;
    int first[4][2] = {{1, 2},
                    {3, 4},
                    {5, 6},
                    {7, 8}};
    int second[2][5] = {{1, 2, 3, 4, 5},
                      {6, 7, 8, 9, 10}};



    for (baris = 0; baris < 4; baris++) {
      for (kolom = 0; kolom < 5; kolom++) {
        for (tengah = 0; tengah < 2; tengah++) {
          sum = sum + first[baris][tengah]*second[tengah][kolom];
        }
 
        multiply[baris][kolom] = sum;
        sum = 0;
      }
    }
 
    printf("Hasil Matriks:\n");
     for (baris = 0; baris < 4; baris++) {
      for (kolom = 0; kolom < 5; kolom++){
        printf("%d\t", multiply[baris][kolom]);
      }
      printf("\n");
    }
    for (baris = 0; baris < 4; baris++) {
      for (kolom = 0; kolom < 5; kolom++){
      *value = multiply[baris][kolom];
      sleep(1);
      printf("terkirimz %d \n", *value);
      }
    }
    shmdt(value);
    shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
