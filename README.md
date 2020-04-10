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
Digunakan pula fork-exec untuk menghentikan berjalannya game ini. Dengan menggunakan command ```bash kill``` yang dipanggil dengan mencari tahu dulu pid proses ```soal1_traizone.c``` yang ingin dihentikan. Dengan cara masuk ke dir ```/proc``` dan mencari file ```soal1_traizone.c```, kemudian akan digunakan perintah ```fork``` dan ```execv``` untuk meng-kill PID proses tersebut. Setelah selesai meng-kill traizone, maka proses pokezone juga sudah selesai dan langsung berhenti juga.
Source code:
```c
struct dirent *dp;
DIR *dfd;
if((dfd = opendir("/proc")) == NULL)
{
	fprintf(stderr, "Can't open /proc\n");
	return 0;
}
char *endp;
while((dp = readdir(dfd)) != NULL)
{
	long pid = strtol(dp->d_name, &endp, 10);
	if(*endp != '\0') continue;
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "/proc/%ld/cmdline", pid);
	FILE *f = fopen(buffer, "r");
	if(f != NULL)
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
closedir(dfd);
int stat;
while(wait(&stat) > 0);
```

#

## Soal2
Soal ini meminta kami untuk membuat online game berbasis text console bernama Tap Tap Game.
Solusi soal ini terdiri dari 2 file yaitu ```soal2_server.c``` dan ```soal2_client.c```.
Untuk menyelesaikan soal ini, kami menggunakan socket dan threading untuk mengkomunikasikan antara server dan client, serta thread agar game dapat berjalan secara paralel untuk masing-masing player.
Soal ini terdiri dari 2 sisi, yaitu:
1. Client Side
2. Server Side

#

## Client Side
Pada Client Side, terdapat 2 screen tampilan, yaitu **Screen 1** untuk menangani login dan register akun, sedangkan **Screen 2** untuk mencari player lawan agar game dapat dimulai.

#

### Screen 1
Berikut source code Screen 1, untuk menangani login dan register:
```c
int main()
    ....
    screen1:
    printf("1. Login\n2. Register\nChoices : ");
    scanf("%s", cmd);
    if(strcmp(cmd, "login") == 0)
    {
        strcpy(username, "l ");
        printf("Username : ");
        getchar();
        scanf("%[^\n]", username);
        strcat(username, temp);
        printf("Password : ");
        getchar();
        scanf("%[^\n]", pass);
        strcat(username, "\t");
        strcat(username, pass);
        send(sock, username, strlen(username), 0);
        int feedback;
        read(sock, &feedback, sizeof(feedback));
        if(feedback)
        {
            printf("login success\n");
            send(sock, "sukses", 6, 0);
            screen2:
	    ...
        }
        else
        {
            printf("login failed\n");
            goto screen1;
        }
    }
    else if(strcmp(cmd, "register") == 0)
    {
        strcpy(username, "r ");
        printf("Username : ");
        getchar();
        scanf("%[^\n]", temp);
        strcat(username, temp);
        printf("Password : ");
        getchar();
        scanf("%[^\n]", pass);
        strcat(username, "\t");
        strcat(username, pass);
        send(sock, username, strlen(username), 0);
        printf("register success\n");
        goto screen1;
    }
    else
    {
        printf("invalid input\n");
        goto screen1;
    }
    ...
}
```
Dikarenakan menggunakan bahasa C, untuk memberikan value string pada suatu variabel, harus menggunakan fungsi ```strcpy``` yang berarti string copy, sedangkan untuk menggabungkan string, digunakan fungsi ```strcat``` yang berarti string concat. Dikarenakan pada contoh soal no 2, memungkinkan username dan password memiliki spasi, tidak dapat diterima menggunakan ```scanf("%s", &str);``` biasa, sehingga harus menggunakan ```scanf("%[\n]", &str);``` yang berarti scan terus inputan tersebut sampai menemui karakter ```\n``` yang berarti enter. Hal yang sama dilakukan untuk men-scan input password.

Di sini, setiap mendapatkan input username dan password baik dari login maupun register, akan kami passing (send) melalui socket ke server side, dengan command ```send(sock, str, strlen(str), 0)```. Untuk membedakan string yang dipassing merupakan dari menu login atau register, maka kami tambahkan karakter **l** yang menandakan itu dari menu login, dan karakter **r** yang menandakan dari menu register saat ingin dipassing.

#

### Screen 2
Berikut source code Screen 2, untuk menangani find player agar dapat memulai game:
```c
void *finding()
{
    while(1)
    {
        printf("Waiting for player ...\n");
        sleep(1);
    }
}

void *playing(void *arg)
{
    while(1)
    {
        char ch = getchar();
        if(ch == ' ') 
        {
            printf("hit !!\n");
            send(*(int*) arg, &ch, sizeof(ch), 0);
        }
    }
}

int main()
{
	screen2:
	printf("1. Find Match\n2. Logout\nChoices : ");
	scanf("%s", cmd2);
	if(strcmp(cmd2, "logout") == 0)
	{
		send(sock, cmd2, strlen(cmd2), 0);
		goto screen1;
	}
	else if(strcmp(cmd2, "find") == 0)
	{
		int start;
		send(sock, cmd2, strlen(cmd2), 0);
		pthread_t th;
		pthread_create(&th, NULL, finding, NULL);
		read(sock, &start, sizeof(start));
		pthread_cancel(th);
		printf("Game dimulai silahkan \e[3mtap tap\e[0m secepat mungkin !!\n");
		struct termios prev, cur;
		tcgetattr(0, &prev);
		cur = prev;
		cur.c_lflag &= -ICANON;
		cur.c_lflag &= -ECHO;
		tcsetattr(0, TCSANOW, &cur);
		pthread_t th2;
		pthread_create(&th2, NULL, playing, (void *) &sock);
		int health = 100;
		while(1)
		{
		    read(sock, &health, sizeof(health));
		    if(health == -1) break;
		    printf("Health: %d\n", health);
		}
		pthread_cancel(th2);
		bool cek;
		read(sock, &cek, sizeof(cek));
		if(cek) printf("Game berakhir kamu menang\n");
		else printf("Game berakhir kamu kalah\n");
		tcsetattr(0, TCSANOW, &prev);
		goto screen2;
	}
	else
	{
		printf("invalid input\n");
		goto screen2;
	}
	...
}
```
Untuk screen 2, akan ada 2 thread yang dijalankan thread pertama untuk mencetak "waiting for player ..." (```pthread_create(&th, NULL, finding, NULL);```) ketika belum ditemukan client lain yang sedang mencari match juga. Thread ini akan terus berjalan setiap detik, sampai dicancel saat sudah menemukan player lain. Untuk mengecek apakah sudah ada player lain yang dapat bermain, digunakan fungsi ```read(sock, &start, sizeof(start));```, di mana variabel start akan dipassing dari server side yang menandakan bahwa game sudah siap dimulai karena sudah ada 2 client yang terkoneksi ke server di waktu yang sama. Kemudian di sini kami juga menggunakan fungsi ```termios``` agar saat bermain, saat user menginputkan karakter spasi, langsung terbaca menembak musuh tanpa harus menekan enter. Saat  bermain maka akan dijalankan thread yang berfungsi menunggu player saat menginput spasi untuk menembak musuh. Saat health salah satu player sudah habis (<= 0), maka server akan mengirimkan socket value health -1, yang menandakan game dihentikan, kemudian server mengirimkan kode ke masing-masing player apakah ia menang atau kalah.

#

## Server Side
Pada Server Side, digunakan untuk verifikasi client yang sedang login/register, untuk penyimpanan data ke ```akun.txt```, serta pengecekan data saat login apakah akun sudah terdaftar. Kemudian pada server side, juga berfungsi untuk mencari player-player yang dapat saling terkoneksi, ketika banyak yang terkoneksi ada 2 player, maka game akan segera dimulai. Ketika sudah selesai, maka koneksi mereka akan diputus menjadi bukan yang sedang bermain. Ruang bermain hanya dibatasi 2 player saja, yang masing-masing mempunyai thread sendiri.

Untuk menyimpan data suatu player, yaitu thread sendiri, thread musuh, health sendiri, dan health musuh, digunakan struct player yang menyimpan itu semua, agar saat menjalankan thread player, dapat langsung melakukan passing semua data player.

Source code:
```c
typedef struct akun {
    char un[1024];
    char ps[1024];
} akun;

typedef struct client_serv {
    int cid;
    int login;
} client_serv;

typedef struct player {
    int cidp, cide;
    int *healthp, *healthe;
} player;

void *ready(void *arg)
{
    client_serv cl = *(client_serv *) arg;
    int cid = cl.cid;
    int log = cl.login;
    if(log)
    {
        screen1:;
        char data[1024], un[1024], ps[1024], kode;
        memset(data, 0, sizeof(data));
        read(cid, data, sizeof(data)); //kode username	password
        kode = data[0];
        int id = 0;
        for(int i = 2; i < strlen(data); i++)
        {
            if(data[i] == '\t') break;
            un[id] = data[i];
            id++;
        }
        //un[id] = '\0';
        int id2 = 0;
        id += 3;
        for(int j = id; j < strlen(data); j++)
        {
            ps[id2] = data[j];
            id2++;
        }
        //ps[id2] = '\0';
        //printf("debug: %s - %s\n", un, ps);
        if(kode == 'l')
        {
            bool done = 0;
            while(akun_cek);
            for(int i = 0; i < akun_n; i++)
            {
                if((strcmp(un, list_akun[i].un) == 0) && (strcmp(ps, list_akun[i].ps) == 0))
                {
                    done = 1;
                    break;
                }
            }
            send(cid, &done, sizeof(done), 0);
            if(done) printf("Auth success\n");
            else
            {
                printf("Auth Failed\n");
                goto screen1;
            }
        }
        else if(kode == 'r')
        {
            akun_cek = 1;
            akun akun_reg;
            strcpy(akun_reg.un, un);
            strcpy(akun_reg.ps, ps);
            list_akun[akun_n] = akun_reg;
            akun_n++;
            while(file_cek);
            file_cek = 1;
            FILE *fptr = fopen("akun.txt", "a");
            fprintf(fptr, "%s\t%s\n", un, ps);
            fclose(fptr);
            file_cek = 0;
            akun_cek = 0;
            printf("Username\tPassword\n");
            for(int i = 0; i < akun_n; i++)
            {
                akun cur = list_akun[i];
                printf("%s\t%s\n", cur.un, cur.ps);
            }
        }
    }
    log = 1;
    char cmdt;
    read(cid, &cmdt, strlen(&cmdt));
    if(strcmp(&cmdt, "logout") == 0) goto screen1;
    else if(strcmp(&cmdt, "find") == 0)
    {
        while(conn_cek);
        conn_cek = 1;
        conn++;
        conn_cek = 0;
        while(conn < 2);
    }
    pthread_exit(0);
}

void *play(void *arg)
{
    player p = *(player *) arg;
    int *he = p.healthe;
    int start = 1;
    send(p.cidp, &start, sizeof(start), 0);
    while(1)
    {
        char ch;
        read(p.cidp, &ch, sizeof(ch));
        *he -= 10;
        send(p.cide, he, sizeof(*he), 0);
    }
}

int main()
{
    ...
    while(1)
    {
        bool player1_cek = 0;
        bool player2_cek = 0;
        int health1 = 100;
        int health2 = 100;
        player player1, player2;
        player1.cidp = client[0];
        player1.cide = client[1];
        player1.healthp = &health1;
        player1.healthe = &health2;
        player2.cidp = client[1];
        player2.cide = client[0];
        player2.healthp = &health2;
        player2.healthe = &health1;
        pthread_create(&th[0], NULL, play, (void *) &player1);
        pthread_create(&th[1], NULL, play, (void *) &player2);
        while(1) if(health1 <= 0 || health2 <= 0) break;
        pthread_cancel(th[0]);
        pthread_cancel(th[1]);
        int fin = -1, win = 1, lose = 0;
        send(client[0], &fin, sizeof(fin), 0);
        send(client[1], &fin, sizeof(fin), 0);
        conn -= 2;
        if(health1 <= 0)
        {
            send(client[0], &lose, sizeof(lose), 0);
            send(client[1], &win, sizeof(win), 0);
        }
        else if(health2 <= 0)
        {
            send(client[0], &win, sizeof(win), 0);
            send(client[1], &lose, sizeof(lose), 0);
        }
        for(int i = 0; i < 2; i++)
        {
            client_serv p;
            p.cid = client[i];
            p.login = 1;
            pthread_create(&th[i], NULL, ready, (void *) &p);
        }
        pthread_join(th[0], NULL);
        pthread_join(th[1], NULL);
    }
    ...
}
```
#

## Soal3
Soal ini meminta kami untuk mengkategorikan file-file ke dalam folder sesuai ekstensinya. Terdapat 3 mode input yakni -f, -d, dan \*. Dimana pada mode -f user dapat menambahkan argumen yang berupa full path file sebanyak yang user inginkan, pada mode -d user hanya bisa input 1 directory saja, lalu pada mode \* user akan memindahkan seluruh file berdasarkan ekstensinya. Menggunakan library ```dirent.h``` untuk melakukan looping pada tiap direktori dan mengecek setiap ekstensi file. Pada soal ini kami membuat fungsi untuk memindahkan file ke folder cwd (current working directory) per file. 
```c
void pindahFile(char *argv, char *cwd){
//   printf("stringvoid = %s\n", argv);
//   printf("stringvoid = %s\n", cwd);
  
  char string[1000];
  strcpy(string, argv);
  int isFile = is_regular_file(string);
  char dot = '.'; 
  char slash = '/';
  char* tipe = strrchr(string, dot); 
  char* nama = strrchr(string, slash);
  
  char tipeLow[100];
  if(tipe)
  {
    if((tipe[strlen(tipe)-1] >= 'a' && tipe[strlen(tipe)-1] <= 'z') || (tipe[strlen(tipe)-1] >= 'A' && tipe[strlen(tipe)-1] <= 'Z'))
    {
      strcpy(tipeLow, tipe);
      for(int i = 0; tipeLow[i]; i++){
        tipeLow[i] = tolower(tipeLow[i]);
      }
    }
  }
  else
  {
    if(!isFile){
      printf("ini adalah folder, salah argumen\n");
    //   mkdir(nama, 0777);
      return;
    }
    else
    {
      strcpy(tipeLow, " Unknown"); //tanpa ekstensi
    }
  }
    mkdir((tipeLow + 1), 0777); //bikin folder ekstensi

    size_t len = 0 ;
    // strcpy
    char a[1000] ; //res
    strcpy(a, argv);
    char b[1000] ; //des
    strcpy(b, cwd);
    strcat(b, "/");
    strcat(b, tipeLow+1);
    strcat(b, nama);
    printf("a = %s\n", a);

    printf("b = %s\n", b);

    rename(a, b);
    remove(a);
}
```
untuk file banyak, yaitu command -d dan \*, ada lagi fungsi untuk memasukan setiap file yang ada di dalam folder tersebut ke sebuah string lalu di iterasi untuk semua file yang ada didalam directory tersebut.
```c
void sortHere(char *asal){
  arg_struct args;
  // args.cwd = "/home/rapuyy/modul3";
  strcpy(args.cwd,"/home/rapuyy/modul3");
  DIR *dirp;
    struct dirent *entry;
    dirp = opendir(asal);
    int index = 0;
    while((entry = readdir(dirp)) != NULL)
    {
      if(entry->d_type == DT_REG)
      {
        char namafile[105];
        sprintf(namafile, "%s/%s", asal, entry->d_name);
        strcpy(args.asal, namafile);
        if(strcmp(namafile, "/home/rapuyy/modul3/no3.c")!=0)
        {
            pthread_create(&tid[index], NULL, pindahf, (void *)&args);
            printf("%s\n", namafile);
            sleep(1);
            index++;    
        }
        
      }
    }
}
```

#

## Soal4
Soal ini meminta kami untuk menghitung perkalian matriks kemudian mengolah angka-angka tersebut menjadi  faktorial penjumlahan. dan terakhir memberikan list.

#
4a. Di bagian ini kita diminta untuk mengalikan matriks dengan ukuran 4x2 dan 2x5, yang mana hasilnya adalah matriks 4x5, dan isi dari matriks awalnya kita isi sendiri.
```c
.....
for (baris = 0; baris < 4; baris++) {
      for (kolom = 0; kolom < 5; kolom++) {
        for (tengah = 0; tengah < 2; tengah++) {
          sum = sum + first[baris][tengah]*second[tengah][kolom];
        }
 
        multiply[baris][kolom] = sum;
        sum = 0;
      }
    }
.....
```
#
4b. setelah kita menghitung perkalian matriks tersebut, hasilnya akan dikirim ke proses *soal4b.c* menggunakan shared memory. setelah *soal4b.c* mendapat kiriman dari *soal4a.c*, matriks hasil akan dilakukan penjumlahan dari n sampai 1. lalu ditampilkan hasilnya dengan tampilan seperti matriks.
```c
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
```
#
4c. di sub-soal bagian ini kita diminta untuk menjalankan mengetahui jumlah file dan folder di direktori saat ini dengan command "ls | wc -l" menggunakan IPC Pipe
```c
.....
    else if(pp !=  0) //parent process
    {
        close(0);
        dup2(p[0], 0);
        close(p[0]);
        close(p[1]);
        char *argv[] = {"wc", "-l", NULL};
        execv("/usr/bin/wc", argv);
    }
    else //child
    {
        close(0);
        dup2(p[1], 1);
        close(p[0]);
        close(p[1]);
        char *argv[] = {"ls", NULL};
        execv("/bin/ls", argv);
    }
.....
```
