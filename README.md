# SoalShiftSISOP20_modul3_F08
Soal Shift Sistem Operasi 2020
#
1. Sheinna Yendri (05111840000038)
2. Muhammad Rafi Yudhistira (05111840000115)
#
1. [Soal1](#soal1)
2. [Soal2](#soal2)
3. [Soal3](#soal3)
4. [Soal4](#soal4)
#

## Soal1
Soal ini meminta kami untuk membuat game berbasis text terminal mirip game Pokemon GO.
Solusi soal ini terdiri dari 2 file yaitu ```soal1_traizone.c``` dan ```soal1_pokezone.c```.
Untuk menyelesaikan soal ini, kami menggunakan multithreading untuk menjalankan beberapa proses secara paralel (misal meng-handle perhitungan masing-masing Pokemon yang dimiliki), shared memory untuk menyimpan value stok Shop yang diakses baik di file ```soal1_traizone.c``` maupun ```soal1_pokezone.c```, serta fork-exec agar saat mengeksekusi ```soal1_pokezone.c``` dan memberikan command Quit, maka program ```soal1_traizone.c``` akan berhenti (di-kill).

Untuk source code ```soal1_traizone.c``` terdiri dari 2 mode, yaitu:
1. Normal Mode
2. Capture Mode

#

### Normal Mode
Saat sedang berada di **Normal Mode**, kita bisa memilih 5 menu:
1. Cari Pokemon
2. Pokedex
3. Store
4. Go to Capture Mode
5. Keluar

Kami tambahkan 1 menu, **Go to Capture Mode** agar mempermudah pemindahan mode.
Source code:
```c
printf("Normal Mode\n");
if(!cari) printf("1. Cari Pokemon\n2. Pokedex\n3. Shop\n4. Go to Capture Mode\nPick a number: ");
else printf("1. Berhenti Mencari\n2. Pokedex\n3. Shop\n4. Go to Capture Mode\nChoice : ");
```
Kemudian opsi pertama dapat berubah sesuai kondisi sekarang, jika sedang mencari Pokemon, maka opsi pertama menjadi **Berhenti Mencari**. Untuk mengetahui kondisi sedang mencari atau tidak, digunakan variabel boolean ```cari``` sebagai penanda. 1 berarti sedang mencari, 0 berarti sedang tidak mencari.

#

### 1. Cari Pokemon
Untuk menu **Cari Pokemon**, ada beberapa ketentuan yaitu sebagai berikut:
1. Tiap 10 detik (```sleep(10)```) ada chance 60% untuk menemukan Pokemon. Yang tiap-tiap kelompok Pokemon (Normal, Rare, dan Legendary) memiliki tingkat encounter yang berbeda-beda (80%, 15%, dan 5%). Untuk menangani masalah ini, kami menggunakan perintah ```srand(time(NULL));``` untuk mendapatkan angka random yang dipanggil dengan fungsi ```rand()```. Sehingga untuk mendapatkan kemungkinan sekian persen dapat menggunakannya dalam syntax sebagai berikut:
```c
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

int main()
{
    ...
    cari = cari ^ 1;
    if(cari) pthread_create(&th1, NULL, findPokemon, NULL);
    else pthread_cancel(th1);
    ...
}

```
Fungsi ```findPokemon``` dijalankan pada thread th1, di mana fungsi thread ini adalah untuk mencari pokemon secara random dengan 60% encounter rate sehingga digunakan syntax ```rand() % 100 < 60```, di mana kemungkinan memenuhi persamaan ini adalah 60 dari 100 kemungkinan (dapat disebut 60%). Kemudian jika kita menemukan pokemon, maka akan digunakan fungsi ```pthread_mutex_lock(&pokecapt_cek);``` dan ```pthread_mutex_unlock(&pokecapt_cek);``` untuk mengunci agar tidak ada proses lain yang mengakses memory yang sedang diakses (mutual exclusion). Kemudian syntax ```pokecapt = *vpoke;``` untuk meng-assign nilai ID pokemon yang secara random didapatkan dari source code bagian ```soal1_pokezone.c``` fungsi ```randomPoke()```:
```c
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
```
Dari fungsi atas (sebagian file ```soal1_pokezone.c```), didapatkan bahwa variabel ```*vpoke``` berisi ID pokemon yang secara random didapatkan sesuai dengan encounter ratenya, yaitu 1/8000 untuk tipe Shiny (15 <= ID <= 29), 5/100 untuk tipe Legendary (10 <= ID <= 14), 15/100 untuk tipe Rare (5 <= ID <= 9), dan 80/100 tipe Normal (0 <= ID <= 4). Dan ada juga fungsi untuk restock stok setiap 10 detik.

Kemudian kembali ke bagian file ```soal1_traizone.c```, saat di fungsi findPokemon sudah menemukan pokemon, maka membuat thread baru dengan nama th2, yang bertugas untuk meng-handle chance tertangkapnya suatu pokemon tersebut dengan escape rate yang berbeda-beda. Solusi penanganan mirip dengan penanganan encounter rate.

#

### 2. Pokedex
Untuk menu **Pokedex**, kami diminta menampilkan list Pokemon yang sudah tertangkap beserta nilai AP-nya, kemudian ada opsi untuk melepas Pokemon ("menjual") atau memberi Berry kepada semua Pokemon yang sudah tertangkap. Source code penyelesaian bagian ini:
```c
typedef struct slot {
	pthread_t poketh;
	int pokeid, pokeap;
} slot;

slot pokeslot[10];

int main()
{
        ...
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
        ...
}
```
Agar mempermudah penyimpanan dan akses pokemon-pokemon yang dimiliki (7 slot), kami menggunakan array yang menampung tipe data struct yang berisi ID Pokemon, AP Pokemon, serta Thread Pokemon itu. Saat memasuki opsi **Pokedex**, otomatis mencetak semua isi 7 slot Pokemon yang dimiliki. Kemudian apabila ingin **Melepas Pokemon**, tinggal memilih no berapa dan kemudian slot pokemon itu akan direset dan threadnya dihentikan, serta Poke Dollar bertambah sesuai harga Pokemon yang dilepas. Untuk opsi **Memberi Berry**, maka akan dicek dulu apakah berry-nya tersedia (kita punya/tidak). Jika ada, maka dilakukan looping untuk mengecek masing-masing slot Pokemon, apabila bertemu ID Pokemon selain -1 (tanda kosong), maka AP nya akan ditambah 10. Tidak lupa harus menggunakan library ```pthread_mutex_lock``` dan ```pthread_mutex_unlock```.

#

### 3. Shop
Untuk menu **Shop**, kami diminta untuk menampilkan item-item yang tersedia untuk dibeli dengan **Poke Dollar** yang dimiliki (inisiasi kami beri 100). Item yang ada adalah **Lullaby Powder** yang jika diaktifkan, dapat bertahan selama 10 detik untuk meningkatkan capture chance saat ingin menangkap Pokemon. Item kedua adalah **Pokeball** yang digunakan untuk menangkap Pokemon, dan yang ketiga adalah **Berry** untuk menambah AP setiap Pokemon sebanyak 10. Masing-masing item akan ditampilkan harga, stok, dan available (yang kita punya). Source code penyelesaian bagian ini:
```c
int main()
{
	...
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
	...
}
```
Terlihat bahwa saat memasuki Shop, maka akan langsung dicetak semua item yang dijual beserta dengan harga, stok, dan availablenya. Kemudian jika memilih untuk membeli suatu item, maka akan ditanyakan banyaknya item yang mau dibeli. Kemudian akan dicek terlebih dahulu apakah Poke Dollar yang dimiliki cukup untuk membeli item dengan banyak yang diinginkan, dan apakah stok ada, serta apakah jika melakukan pembelian, maka item melebihi 99 (max diizinkan memiliki 99 untuk setiap item). Apabila memenuhi semua syarat, barulah Poke Dollar akan terpotong, stok di shared memory akan berkurang, sedangkan available yang dimiliki pemain akan ditambah. Hal yang sama terjadi untuk setiap item.

## Capture Mode
Saat sedang berada di **Capture Mode**, kita bisa memilih 3 menu:
1. Tangkap
2. Item
3. Keluar
Source code:
```c
printf("Capture Mode\n");
if(pokecapt == -1) printf("1. Tangkap\n2. Item\n3. Keluar\nChoice : ");
else printf("Pokemon %s ditemukan.\n1. Tangkap\n2. Item\n3. Keluar\nChoice : ", pokename[pokecapt]);
```

#

### 1. Tangkap
Untuk menu **Tangkap**, awalnya kita akan mengecek dulu apakah memiliki Pokeball untuk menangkap Pokemon, dan apakah sudah ada pokemon yang ditemukan. Jika kedua syarat terpenuhi barulah melakukan random capture chance berdasarkan keaktifan lullaby powder, dan tipe pokemon yang ditemukan itu. Apabila Pokemon tertangkap, maka akan dilakukan looping sebanyak slot Pokemon untuk mencari slot yang kosong. Apabila tidak ada, maka Pokemon tersebut otomatis terlepas dan kita mendapatkan uang sebanyak harga Pokemon tersebut langsung diakumulasi dalam Poke Dollar yang kita miliki. Jika tidak penuh, maka slot tersebut akan diisi dengan ID Pokemon baru, AP Pokemon baru (tidak lupa dengan lock dan unlock mutex), dan thread Pokemon tersebut akan dimulai, di mana bertugas untuk meng-counter apakah Pokemon akan terlepas (karena setiap 10 detik, AP masing-masing Pokemon berkurang 10 poin). Jika AP = 0, ada 10% kemungkinan AP menjadi 50, sedangkan 90% kemungkinan akan terlepas. Kemudian thread pencarian pokemon (th2) akan dihentikan. Source code:
```c
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

int main()
{
	...
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
	...
}
```
Fungsi **pokeEscape** berjalan pada masing-masing Pokemon untuk menghitung pengurangan AP Pokemon setiap 10 detik, serta meng-handle apa yang terjadi saat AP = 0, kejadian yang terjadi disesuaikan dengan peluangnya.

#

### 2. Item
Untuk menu **Item**, digunakan untuk memakai lullaby powder. Dapat digunakan apabila lullaby powder sudah tidak aktif dan memiliki lullaby powder. Source code:
```c
void *lullabyPowder()
{
	while(10) sleep(10);
	powder_active = 0;
	printf("Lullaby Powder sudah tidak aktif.\n");
	pthread_exit(0);
}

int main()
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
```
Fungsi **lullabyPowder** dijalankan oleh thread th3 untuk menangani durasi masa aktif lullaby powder setiap digunakan yaitu 10 detik. Setelah sudah melebihi 10 detik, maka thread akan di-exit (berhenti).

#

### Shared Memory & Thread_Mutex
Dikarenakan soal ini terdiri dari 2 file sedangkan mengakses beberapa value yang sama, digunakan shared memory untuk mengatasi pokemon yang dirandom (```shmidpoke```), stok lullaby powder (```shmidpow```), stok pokeball (```shmidball```), dan stok berry (```shmidberry```). Kemudian pointer ke lokasi memory ini akan disimpan dalam variabel ```*vpoke```, ```*vpow```, ```*vball```, dan ```*vberry```. Digunakan fungsi ```shmget``` untuk mengambil shared memory pada key tertentu, kemudian ```shmat``` untuk attack, dan ```shmdt``` untuk detach, serta ```smhctl```.

Kemudian untuk inisiasi pthead mutual exclusion, digunakan fungsi ```pthread_mutex_init``` dan untuk mengakhirinya, digunakan fungsi ```pthread_mutex_destroy```.

Source code:
```c
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

pthread_mutex_init(&poke_cek, NULL);
pthread_mutex_init(&pokecapt_cek, NULL);
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
```
#

### Fork-exec
Digunakan pula fork-exec untuk menghentikan berjalannya game ini. Dengan menggunakan command ```bash kill``` yang dipanggil dengan mencari tahu dulu pid proses ```soal1_traizone.c``` yang ingin dihentikan. Dengan cara masuk ke dir ```/proc``` dan mencari file ```soal1_traizone.c```, kemudian akan digunakan perintah ```fork``` dan ```execv``` untuk meng-kill PID proses tersebut.
Source code:
```c
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
```

#

## Soal2
Soal ini meminta kami untuk membuat online game berbasis text console bernama Tap Tap Game.
Solusi soal ini terdiri dari 2 file yaitu ```soal2_tapserver.c``` dan ```soal2_tapplayer.c```.
Untuk menyelesaikan soal ini, kami menggunakan socket dan threading untuk mengkomunikasikan antara server dan client, serta thread agar game dapat berjalan secara paralel untuk masing-masing player.
Soal ini terdiri dari 2 sisi, yaitu:
1. Client Side
2. Server Side

#

### Client Side
Pada Client Side, terdapat 2 screen tampilan, yaitu **Screen 1** untuk menangani login dan register akun, sedangkan **Screen 2** untuk mencari player lawan agar game dapat dimulai. 

#

## Soal3

## Soal4
