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

Saat sedang berada di **Normal Mode**, kita bisa memilih 3 menu:
1. Cari Pokemon
2. Pokedex
3. Store
(4. Keluar)

#

### 1. Cari Pokemon
Untuk menu **1. Cari Pokemon**, ada beberapa ketentuan yaitu sebagai berikut:
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
    pthread_create(&th1, NULL, findPokemon, NULL);
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
Untuk menu **2. Pokedex**, kami diminta menampilkan list Pokemon yang sudah tertangkap beserta nilai AP-nya, kemudian ada opsi untuk melepas Pokemon ("menjual") atau memberi Berry kepada semua Pokemon yang sudah tertangkap. Source code penyelesaian bagian ini:
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
Agar mempermudah penyimpanan dan akses pokemon-pokemon yang dimiliki (7 slot), kami menggunakan array yang menampung tipe data struct yang berisi ID Pokemon, AP Pokemon, serta Thread Pokemon itu. Saat memasuki opsi **Pokedex**, otomatis mencetak semua isi 7 slot Pokemon yang dimiliki. Kemudian apabila ingin melepas Pokemon, tinggal memilih no berapa dan kemudian slot pokemon itu akan direset dan threadnya dihentikan, serta Poke Dollar bertambah sesuai harga Pokemon yang dilepas. Untuk opsi **Memberi Berry**, maka akan dicek dulu apakah berry-nya tersedia (kita punya/tidak). Jika ada, maka dilakukan looping untuk mengecek masing-masing slot Pokemon, apabila bertemu ID Pokemon selain -1 (tanda kosong), maka AP nya akan ditambah 10. Tidak lupa harus menggunakan library ```pthread_mutex_lock``` dan ```pthread_mutex_unlock```.

#

### 3. Shop

#

## Soal2

## Soal3

## Soal4
