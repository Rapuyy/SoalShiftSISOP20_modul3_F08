#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

void main()
{
    key_t key = 1234;
    int *value;
    int i, j;
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    value = shmat(shmid, NULL, 0);
    
    for(i = 0; i < 20; i++){
        printf("%d\n", *value);
        sleep(1);
    }

    // printf("Program 1 : %d\n", *value);
    // *value = 30;

    // sleep(5);

    // printf("Program 1: %d\n", *value);
    shmdt(value);
    shmctl(shmid, IPC_RMID, NULL);

}