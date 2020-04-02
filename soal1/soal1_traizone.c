#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

typedef struct slot {
	pthread_t poketh;
	int pokeid, pokeap;
} slot;

slot pokeslot[10];
int *vpoke, *vpow, *vball, *vberry;
int dollar, powder, ball, berry, cmd, pokecapt;
bool powder_active, normal, cari;
pthread_mutex_t poke_cek, pokecapt_cek;
pthread_t th, th1, th2, th3;
char *pokename[35] = {
	"Bulbasaur", "Charmander", "Squirtle", "Rattata", "Caterpie", 
	"Pikachu", "Eevee", "Jiggypuff", "Snorlax", "Dragonite", 
	"Mew", "Mewtwo", "Moltres", "Zapdos", "Articuno",
	"Bulbasaur (Shiny)", "Charmander (Shiny)", "Squirtle (Shiny)", "Rattata (Shiny)", "Caterpie (Shiny)", 
	"Pikachu (Shiny)", "Eevee (Shiny)", "Jiggypuff (Shiny)", "Snorlax (Shiny)", "Dragonite (Shiny)", 
	"Mew (Shiny)", "Mewtwo (Shiny)", "Moltres (Shiny)", "Zapdos (Shiny)", "Articuno (Shiny)"
};

void *scanInput()
{
	scanf("%d", &cmd);
}

void *capturePokemon()
{
	int esc_chance = (pokecapt % 15) / 5;
	if(esc_chance == 0) esc_chance = 1;
	else esc_chance *= 2;
	esc_chance *= 5;
	if(pokecapt >= 15) esc_chance += 5;
	while(1)
	{
		if(rand() % 100 < esc_chance)
		{
			printf("Pokemon escaped.\n");
			pthread_mutex_lock(&pokecapt_cek);
			pokecapt = -1;
			pthread_mutex_unlock(&pokecapt_cek);
			pthread_exit(0);
		}
		sleep(20);
	}
}

void *findPokemon()
{
	while(1)
	{
		if(rand() % 100 < 60)
		{
			cari = 0;
			pthread_mutex_lock(&pokecapt_cek);
			pokecapt = *vpoke;
			pthread_mutex_unlock(&pokecapt_cek);
			pthread_create(&th2, NULL, capturePokemon, NULL);
			normal = 0;
			pthread_exit(0);
		}
		sleep(10);
	}
}

void *pokeEscape(void *arg)
{
	int s = *(int *)arg;
	while(1)
	{
		if(!normal) continue;
		pthread_mutex_lock(&poke_cek);
		pokeslot[s].pokeap -= 10;
		if(pokeslot[s].pokeap == 0)
		{
			if(rand() % 100 < 10) pokeslot[s].pokeap += 50;
			else
			{
				printf("Pokemon %s lepas.\n", pokename[pokeslot[s].pokeid]);
				pokeslot[s].pokeap = -1;
				pokeslot[s].pokeid = -1;
				pthread_exit(0);
			}
		}
		pthread_mutex_unlock(&poke_cek);
		sleep(10);
	}
}

void *lullabyPowder()
{
	while(10) sleep(10);
	powder_active = 0;
	printf("Lullaby Powder sudah tidak aktif.\n");
	pthread_exit(0);
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
	for(int i = 0; i < 7; i++)
	{
		pokeslot[i].pokeid = -1;
		pokeslot[i].pokeap = -1;
		pokeslot[i].poketh = 0;
	}
    pthread_mutex_init(&poke_cek, NULL);
    pthread_mutex_init(&pokecapt_cek, NULL);
    
    normal = 1;
    srand(time(NULL));
	dollar = 100; powder = 0; ball = 0; berry = 0;
	powder_active = 0; cari = 0;
	
	while(1)
	{
		int cmd2;
		if(normal)
		{
			printf("Normal Mode\n");
			if(!cari) printf("1. Cari Pokemon\n2. Pokedex\n3. Shop\n4. Go to Capture Mode\nPick a number: ");
			else printf("1. Berhenti Mencari\n2. Pokedex\n3. Shop\n4. Go to Capture Mode\nChoice : ");
			pthread_create(&th, NULL, scanInput, NULL);
			pthread_join(th, NULL);
			if(cmd == -1)
			{
				printf("\n");
				continue;
			}
			else if(cmd == 1)
			{
				cari = cari ^ 1;
				if(cari) pthread_create(&th1, NULL, findPokemon, NULL);
				else pthread_cancel(th1);
			}
			else if(cmd == 2)
			{
				printf("Pokedex\n");
				printf("No  Pokemon               AP\n");
				for(int i = 0; i < 7; i++)
				{
					printf("%d)  %-22s%d\n", i + 1, pokename[pokeslot[i].pokeid], pokeslot[i].pokeap);
				}
				printf("1. Lepas Pokemon\n2. Beri Berry\n3. Kembali\nChoice : ");
				scanf("%d", &cmd2);
				if(cmd2 == 1)
				{
					int no;
					while(1)
					{
						printf("Pilih Nomor Pokemon : ");
						scanf("%d", &no);
						if(no > 0 && no < 8 && pokeslot[no - 1].pokeid != -1) break;
						else printf("Pilih slot yang benar.\n");
					}
					pthread_mutex_lock(&poke_cek);
					pokeslot[no - 1].pokeap = -1;
					pthread_cancel(pokeslot[no - 1].poketh);
					int id = pokeslot[no - 1].pokeid;
					char nama[50];
					strcpy(nama, pokename[pokeslot[no - 1].pokeid]);
					if(id >= 15) dollar += 5000;
					else if(id < 5) dollar += 80;
					else if(id < 10) dollar += 100;
					else if(id < 15) dollar += 200;
					pokeslot[no - 1].pokeid = -1;
					pthread_mutex_unlock(&poke_cek);
					printf("Pokemon %s sudah dilepas, nilai poke dollar sekarang %d.\n", nama, dollar);
				}
				else if(cmd2 == 2)
				{
					if(berry <= 0)
					{
						printf("Tidak mempunyai berry.\n");
					}
					else
					{
						berry--;
						pthread_mutex_lock(&poke_cek);
						for(int i = 0; i < 7; i++)
						{
							if(pokeslot[i].pokeap != -1) pokeslot[i].pokeap += 10;
						}
						pthread_mutex_unlock(&poke_cek);
						printf("Pokemon sudah diberi berry.\n");
					}
				}
				else if(cmd2 == 3) printf("Keluar dari Pokedex.\n");
			}
			else if(cmd == 3)
			{
				printf("Shop\n");
				printf("No  Item            Harga  Stok  Available\n");
				printf("==========================================\n");
				printf("1)  Lullaby Powder  60     %-3d  %-3d\n", *vpow, powder);
				printf("2)  Pokeball        5      %-3d  %-3d\n", *vball, ball);
				printf("3)  Berry           15     %-3d  %-3d\n", *vberry, berry);
				printf("4)  Kembali\n");
				printf("Pokedollar yang dimiliki : %d\nChoice : ", dollar);
				scanf("%d", &cmd2);
				if(cmd2 == 4) printf("Keluar dari Shop.\n");
				else
				{
					int n;
					printf("Banyak : ");
					scanf("%d", &n);
					if(n > 99)
					{
						printf("Max item yang dibeli 99. Gagal membeli.\n");
					}
					else if(cmd2 == 1)
					{
						if((powder + n) > 99)
						{
							printf("Max item yang dimiliki 99. Gagal membeli.\n");
						}
						else if(*vpow < n)
						{
							printf("Stok tidak mencukupi.\n");
						}
						else if(dollar < n * 60)
						{
							printf("Pokedollar tidak mencukupi.\n");
						}
						else
						{
							dollar -= (n * 60);
							*vpow -= n;
							powder += n;
							printf("%d Lullaby Powder sudah terbeli.\n", n);
						}
					}
					else if(cmd2 == 2)
					{
						if((ball + n) > 99)
						{
							printf("Max item yang dimiliki 99. Gagal membeli.\n");
						}
						else if(*vball < n)
						{
							printf("Stok tidak mencukupi.\n");
						}
						else if(dollar < n * 5)
						{
							printf("Pokedollar tidak mencukupi.\n");
						}
						else
						{
							dollar -= (n * 5);
							*vball -= n;
							ball += n;
							printf("%d Pokeball sudah terbeli.\n", n);
						}
					}
					else if(cmd2 == 3)
					{
						if((berry + n) > 99)
						{
							printf("Max item yang dimiliki 99. Gagal membeli.\n");
						}
						else if(*vberry < n)
						{
							printf("Stok tidak mencukupi.\n");
						}
						else if(dollar < n * 15)
						{
							printf("Pokedollar tidak mencukupi.\n");
						}
						else
						{
							dollar -= (n * 15);
							*vberry -= n;
							berry += n;
							printf("%d Berry sudah terbeli.\n", n);
						}
					}
				}
			}
			else if(cmd == 4)
			{
				printf("Keluar dari Normal Mode.\n");
				normal = 0;
			}
			//pthread_join(th, NULL);
		}
		else
		{
			printf("Capture Mode\n");
			if(pokecapt == -1) printf("1. Tangkap\n2. Item\n3. Keluar\nChoice : ");
			else printf("Pokemon %s ditemukan.\n1. Tangkap\n2. Item\n3. Keluar\nChoice : ", pokename[pokecapt]);
			pthread_create(&th, NULL, scanInput, NULL);
			pthread_join(th, NULL);
			if(cmd == -1)
			{
				printf("\n");
				continue;
			}
			else if(cmd == 1)
			{
				if(ball == 0)
				{
					printf("Tidak mempunyai pokeball.\n");
				}
				else if(pokecapt == -1)
				{
					printf("Belum ada pokemon yang ditemukan.\n");
				}
				else
				{
					ball--;
					int chance = 0;
					if(powder_active) chance += 20;
					if(pokecapt >= 15) chance -= 20;
					int x = (pokecapt % 15) / 5;
					if(x == 0) chance += 70;
					else if(x == 1) chance += 50;
					else if(x == 2) chance += 30;
					if(rand() % 100 < chance)
					{
						printf("Pokemon %s tertangkap.\n", pokename[pokecapt]);
						bool empty = 0;
						int id;
						for(int i = 0; i < 7; i++)
						{
							if(pokeslot[i].pokeid == -1)
							{
								empty = 1;
								id = i;
								break;
							}
						}
						if(!empty)
						{
							if(pokecapt >= 15) dollar += 5000;
							int temp = (pokecapt % 15) / 5;
							if(temp == 0) dollar += 80;
							else if(temp == 1) dollar += 100;
							else if(temp == 2) dollar += 200;
							printf("Slot penuh, pokemon %s terlepas, poke dollar sekarang %d.\n", pokename[pokecapt], dollar);
						}
						else
						{
							pthread_mutex_lock(&poke_cek);
							pokeslot[id].pokeid = pokecapt;
							pokeslot[id].pokeap = 100;
							pthread_mutex_unlock(&poke_cek);
							pthread_create(&pokeslot[id].poketh, NULL, pokeEscape, (void *)&id);
							pthread_cancel(th2);
							pthread_mutex_lock(&pokecapt_cek);
							pokecapt = -1;
							pthread_mutex_unlock(&pokecapt_cek);
						}
					}
					else printf("Pokemon %s tidak tertangkap.\n", pokename[pokecapt]);
				}
			}
			else if(cmd == 2)
			{
				if(powder_active)
				{
					printf("Lullaby Power masih aktif.\n");
				}
				if(powder < 1)
				{
					printf("Lullaby Powder kosong.\n");
				}
				else
				{
					powder--;
					powder_active = 1;
					pthread_create(&th3, NULL, lullabyPowder(), NULL);
				}
			}
			else if(cmd == 3)
			{
				printf("Keluar dari Capture Mode.\n");
				normal = 1;
			}
		}
	}
    pthread_mutex_destroy(&poke_cek);
    pthread_mutex_destroy(&pokecapt_cek);
	shmdt(vpoke);
    shmctl(shmidpoke, IPC_RMID, NULL);
	shmdt(vpow);
    shmctl(shmidpow, IPC_RMID, NULL);
	shmdt(vball);
    shmctl(shmidball, IPC_RMID, NULL);
	shmdt(vberry);
    shmctl(shmidberry, IPC_RMID, NULL);
    exit(1);
	return 0;
}
