#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 500

int multiply(int x, int res[], int res_size);
void fac(int n) 
{ 
    int res[MAX]; 
  
    // Initialize result 
    res[0] = 1; 
    int res_size = 1; 
  
    // Apply simple factorial formula n! = 1 * 2 * 3 * 4...*n 
    for (int x=2; x<=n; x++) 
        res_size = multiply(x, res, res_size); 
  
    // cout << "Factorial of given number is \n"; 
    // printf("\nFactorial of given number is \n");
    for (int i=res_size-1; i>=0; i--){
        printf("%d", res[i]);
    }     
    printf("\n");    
} 
int multiply(int x, int res[], int res_size) 
{ 
    int i;
    int carry = 0;  // Initialize carry 
  
    // One by one multiply n with individual digits of res[] 
    for (i = 0; i < res_size; i++) 
    { 
        int prod = res[i] * x + carry; 
  
        // Store last digit of 'prod' in res[]   
        res[i] = prod % 10;   
  
        // Put rest in carry 
        carry  = prod/10;     
    } 
  
    // Put carry in res and increase result size 
    while (carry) 
    { 
        res[res_size] = carry%10; 
        carry = carry/10; 
        res_size++; 
    } 
    return res_size; 
}

void *faktorial(int angka){
    fac(angka);
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