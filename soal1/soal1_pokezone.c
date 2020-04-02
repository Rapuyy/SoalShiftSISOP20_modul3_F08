#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<dirent.h>
#include<stdbool.h>
#include<signal.h>
#include<string.h>

int *vpoke, *vpow, *vball, *vberry;
pthread_t th1, th2;

void *restockShop()
{
	while(1)
	{
		*vpow += 10; *vball += 10; *vberry += 10;
		if(*vpow > 200) *vpow = 200;
		if(*vball > 200) * vball = 200;
		if(*vberry > 200) *vberry = 200;
		sleep(10);
	}
}

void *randomPoke()
{
	while(1)
	{
		int newpoke = rand() % 100;
		int shiny = rand() % 8000;
		if(newpoke < 5) newpoke = (newpoke % 5) + 10;
		else if(newpoke < 20) newpoke = (newpoke % 5) + 5;
		else newpoke = (newpoke % 5);
		if(shiny == 1) newpoke += 15;
		*vpoke = newpoke;
		sleep(1);
	}
}

int main()
{
	key_t keypoke = 1234;
    key_t keypow = 1235;
    key_t keyball = 1236;
    key_t keyberry = 1237;
    int shmidpoke = shmget(keypoke, sizeof(int), IPC_CREAT | 0666);
    vpoke = shmat(shmidpoke, NULL, 0);
    int shmidpow = shmget(keypow, sizeof(int), IPC_CREAT | 0666);
    vpow = shmat(shmidpow, NULL, 0);
    int shmidball = shmget(keyball, sizeof(int), IPC_CREAT | 0666);
    vball = shmat(shmidball, NULL, 0);
    int shmidberry = shmget(keyberry, sizeof(int), IPC_CREAT | 0666);
    vberry = shmat(shmidberry, NULL, 0);
    srand(time(NULL));
    *vpoke = 100; *vpow = 100; *vball = 100; *vberry = 100;
    pthread_create(&th1, NULL, restockShop, NULL);
    pthread_create(&th2, NULL, randomPoke, NULL);
	printf("Quit Game? [Y]\nChoice : ");
	char c;
	scanf("%c", &c);
	if(c == 'Y')
	{
		DIR *d = opendir("/proc");
		struct dirent *e;
		char *endp;
		while((e = readdir(d)) != NULL)
		{
			long pid = strtol(e->d_name, &endp, 10);
			if(*endp != '\0') continue;
			char buffer[1024];
			snprintf(buffer, sizeof(buffer), "/proc/%ld/cmdline", pid);
			FILE *f = fopen(buffer, "r");
			if(f)
			{
				if(fgets(buffer, sizeof(buffer), f) != NULL) 
				{
					char *awal = strtok(buffer, " ");
					if(strcmp(awal, "./soal1_traizone") == 0)
					{
						pid_t cid;
						cid = fork();
						if(cid < 0) exit(0);
						if(cid == 0)
						{
							char killc[1024];
							sprintf(killc, "kill -INT %ld", pid);
							char *ag[] = {"sh", "-c", killc, NULL};
							execv("/bin/sh", ag);
						}
					}
				}
				fclose(f);
			}
		}
		closedir(d);
		int stat;
		while(wait(&stat) > 0);
		shmdt(vpoke);
		shmctl(shmidpoke, IPC_RMID, NULL);
		shmdt(vpow);
		shmctl(shmidpow, IPC_RMID, NULL);
		shmdt(vball);
		shmctl(shmidball, IPC_RMID, NULL);
		shmdt(vberry);
		shmctl(shmidberry, IPC_RMID, NULL);
		exit(1);	
	}
}
