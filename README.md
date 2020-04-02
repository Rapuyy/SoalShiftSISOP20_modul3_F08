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

Untuk spurce code ```soal1_traizone.c``` terdiri dari 2 mode, yaitu:
1. Normal Mode
2. Capture Mode

#

### Normal Mode
Saat sedang berada di **Normal Mode**, kita bisa memilih 4 menu:
1. Cari Pokemon
2. Pokedex
3. Store
4. Go to Capture Mode
(5. Keluar)

Kami tambahkan 1 menu, **Go to Capture Mode** agar mempermudah pemindahan mode.
```c
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
Dari fungsi atas (sebagian file ```soal1_pokezone.c```), didapatkan bahwa variabel ```*vpoke``` berisi ID pokemon yang secara random didapatkan sesuai dengan encounter ratenya, yaitu 1/8000 untuk tipe Shiny (15 <= ID <= 29), 5/100 untuk tipe Legendary (10 <= ID <= 14), 15/100 untuk tipe Rare (5 <= ID <= 9), dan 80/100 tipe Normal (0 <= ID <= 4).

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
Saat sedang berada di **Capture Mode**, kita bisa memilih 2 menu:
1. Tangkap
2. Item
(3. Keluar)
#

### 

## Soal2

## Soal3

## Soal4
