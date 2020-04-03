#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>
#include <limits.h>

typedef struct arg_struct {
    char asal[1000];
    char cwd[1000];
}arg_struct;

int is_regular_file(const char *path) //jika 0 bukan file
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void pindahFile(char *argv, char *cwd){
  // char string[] = "/home/rapuyy/modul3/no3.c";
      printf("stringvoid = %s\n", argv);
      printf("stringvoid = %s\n", cwd);
      char string[1000];
      strcpy(string, argv);
      int isFile = is_regular_file(string);
      
      char dot = '.'; 
      char slash = '/';
      char* tipe = strrchr(string, dot); 
      char* nama = strrchr(string, slash);

      // printf("%d\n", isFile);
      // if (tipe)
      // {
      //   printf("position of dot is %ld\n", tipe - string);
      //   printf("ekstensi file = %s\n", tipe + 1);
      //   printf("nama file = %s\n", nama + 1);
      // } 
      // else //gada tipe data
      // {
      //   if(!isFile)
      //   {
      //     printf("ini adalah folder %s\n", string);
      //     printf("nama folder = %s\n", nama + 1);
      //   }
      //   else
      //   {
      //     printf("file tidak ber ekstensi %s\n", string);
      //     printf("nama file = %s\n", nama + 1);
      //   }
      // }

  printf("Current working void dir: %s\n", cwd);
  printf("file : %s\n", argv);
  size_t len = 0 ;
  // strcpy
  char a[1000] ;
  strcpy(a, argv);
  printf("a = %s\n", a);
  char b[1000] ;
  strcpy(b, cwd);
  strcat(b, nama);
  printf("b = %s\n", b);
  char buffer[BUFSIZ] = { '\0' } ;

  FILE* in = fopen( a, "rb" ) ;
  FILE* out = fopen( b, "wb" ) ;

  if( in == NULL || out == NULL )
  {
      perror( "An error occured while opening files!!!" ) ;
      in = out = 0 ;
  }
  else    // add this else clause
  {
      while( (len = fread( buffer, BUFSIZ, 1, in)) > 0 )
      {
          fwrite( buffer, BUFSIZ, 1, out ) ;
      }
  
      fclose(in) ;
      fclose(out) ;
      if(!remove(a))
      {
         printf( "File successfully moved");
      }
      else
      {
        printf( "An error occured while moving the file!!!" ) ;
      }
      
  }
}

void *pindahf(void* arg){
  arg_struct args = *(arg_struct*) arg;
  printf("stringthr = %s\n", args.asal);
  printf("stringthr = %s\n", args.cwd);
  pindahFile(args.asal, args.cwd);
}

int main(int argc, char* argv[]) 
{ 

  // char cwd[1000];
  arg_struct args;
  getcwd(args.cwd, sizeof(args.cwd));
  pthread_t tid[500]; //max 500 thread

  //command -f--------------------------------------------------------------
  if(strcmp(argv[1],"-f")==0)
  {
    int index;
    for (int i = 2; i < argc; i++)
    {
      strcpy(args.asal, argv[i]);
      printf("string = %s\n", args.asal);
      pthread_create(&tid[index], NULL, &pindahf, (void *)&args);

      // pindahFile(argv[i], cwd);
      index++;
    }
    for (int i = 0; i < index; i++) {
        pthread_join(tid[i], NULL);
    }

    
  }
  //end -f ----------------------------------------------------------------
	// char string[] = "/home/rapuyy/modul3/soal3"; 

	return 0; 
} 
