#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>
#define PORT 8080

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

akun list_akun[100];
bool akun_cek, file_cek, conn_cek;
int conn, akun_n;

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
        read(cid, data, sizeof(data)); //kode username password
        kode = data[0];
        int id = 0;
        for(int i = 2; i < strlen(data); i++)
        {
            if(data[i] == ' ') break;
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
        printf("debug: %s - %s\n", un, ps);
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
            fprintf(fptr, "%s %s\n", un, ps);
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
        send(p.cide, &he, sizeof(he), 0);
    }
}

int main(int argc, char const *argv[])
{
    int c = 0;
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(0);
    }

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(0);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(0);
    }

    if(listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(0);
    }

    FILE *fp;
    char ch, un[1024], ps[1024];
    int id = 0;
    fp = fopen("akun.txt", "a+");
    if(fp == NULL) exit(0);
    while(fscanf(fp, "%s %s\n", un, ps) != EOF)
    {
        akun akun_baru;
        strcpy(akun_baru.un, un);
        strcpy(akun_baru.ps, ps);
        list_akun[id] = akun_baru;
        id++;
    }
    fclose(fp);
    akun_n = id;
    akun_cek = 0;
    file_cek = 0;
    conn = 0;
    conn_cek = 0;

    pthread_t th[2];
    int client[2];
    for(int i = 0; i < 2; i++)
    {
        client[i] = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
        if(client[i] < 0)
        {
            perror("can't accept client\n");
            i--;
            continue;
        }
        printf("client accepted\n");
        client_serv client_baru;
        client_baru.cid = client[i];
        client_baru.login = 1;
        pthread_create(&th[i], NULL, ready, (void *) &client_baru);
    }

    while(1)
    {
        pthread_join(th[0], NULL);
        pthread_join(th[1], NULL);
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
    }
    return 0;
}
