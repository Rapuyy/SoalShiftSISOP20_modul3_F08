#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 500

int jumlah(int n) {
    if (n != 0)
        return n + jumlah(n - 1);
    else
        return n;
}


void *faktorial(int angka){
    printf("%d ", jumlah(angka));
}

void main()
{
    int arr[4][5], angka;
    key_t key = 1234;
    int *value;
    int i, j;
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    value = shmat(shmid, NULL, 0);
    
    pthread_t tid[20];
    int index;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 5; j++){
            arr[i][j] = *value;
            angka = arr[i][j];
            pthread_create(&tid[index], NULL, &faktorial, (void*) angka);
            // printf("\nnilai index [%d][%d] = %d", i, j, *value);
            printf("\t");
            index++;
            sleep(1);
        }
        printf("\n");
    }
    for (int i = 0; i < index; i++) {
        pthread_join(tid[i], NULL);
    }

    shmdt(value);
    shmctl(shmid, IPC_RMID, NULL);

}