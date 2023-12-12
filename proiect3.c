#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

void process_input(char *director, struct dirent *input, char *director2); //declarare functie 

int main(int argc, char *argv[]) 
{
    DIR *dir = opendir(argv[1]);
    DIR *dir2 = opendir(argv[2]); //deschidere directaore

    pid_t pid; //declarare pid - process  id
    int status; //statusul procesului

    if(!dir2)
    {
        perror("eroare deschidere director 2"); //verificare deschidere director
        exit(-1);
    }

    if (!dir) 
    {
        perror("eroare deschidere director");
        exit(-1);
    }

    struct dirent *input;
    while ((input = readdir(dir)) != NULL) //parcurgere director, citire intrare cu intrare si parcurgere
    {
        if (strcmp(input->d_name, ".") == 0 || strcmp(input->d_name, "..") == 0) { //.pentru director curent .. pentru director parinte - sarim peste ele
            continue;
        }

        process_input(argv[1], input, argv[2]); 
    }

    while ((pid = wait(&status)) != -1) //astept sa se termine procesele
        if (WIFEXITED(status)) //verificare terminare proces
        {
            printf("Child with pid = %d ended with status %d\n", pid, WEXITSTATUS(status)); //mesaj cu numele procesului sin codul cu care s a terminat
        }

    closedir(dir);

    return 0;
}

void process_input(char *director, struct dirent *input, char *director2) //procesare fisier
{
    char cale_stat[500]; //cale pentru out
    char cale_str[257]; //cale pentru in
    snprintf(cale_str, sizeof(cale_str), "%s/%s", director, input->d_name); //printeaza in variabila cale str pathul catre fisier de input

    snprintf(cale_stat, sizeof(cale_stat), "%s/%s_statistica.txt", director2, input->d_name); //printeaza in variabila cale str pathul catre fisier de output

    struct stat input_stat; //salveaza date despre fisier
    if (lstat(cale_str, &input_stat) == -1) //path + variabila pt salvare info
    {
        perror("eroare stat");
        exit(-1);
    }

    int bmp_fd;
    int width, height;

    int out = open(cale_stat, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); //dechidere + creare fisier
    if (out == -1) 
    {
        perror("eroare deschidere fisier de iesire");
        exit(-1);
    }

    if (S_ISREG(input_stat.st_mode) && strstr(input->d_name, ".bmp") != NULL)  //verificare fisier regulat si e si bmp 
    {
        bmp_fd = open(cale_str, O_RDONLY); //deschidere fisier input
        if (bmp_fd == -1) 
        {
            perror("eroare deschidere");
            exit(-1);
        }

        lseek(bmp_fd, 18, SEEK_SET); //caut la pozitia byte 18 cursor 
        read(bmp_fd, &width, sizeof(int)); //citesc latime
        lseek(bmp_fd, 22, SEEK_SET);
        read(bmp_fd, &height, sizeof(int));

        close(bmp_fd);
    }

    char buff[3000];

    if (S_ISREG(input_stat.st_mode)) //ver fisier regulat
    {
        if (strstr(input->d_name, ".bmp") != NULL) //si bmp
        {
            pid_t pid; //creare proces cu fork
            pid=fork();

            pid_t pid2;
            pid2=fork(); //proces 2

            if(pid < 0)
            {
                perror("eroare proces"); 
                exit(-1);
            }
            else if(pid == 0) //daca pid ==0 proces creat - scriere date
            {
                 sprintf(buff, "nume fisier: %s\n inaltime: %d\n, lungime: %d\n, dimensiune: %ld\n, identificarea utilizatorului: %d\n, numar legaturi: %lu\n, drepturi de acces user: %s\n, drepturi de acces multiplu: %s\n, drepturi de acces others: %s\n",
                    cale_str, height, width, input_stat.st_size, input_stat.st_uid, input_stat.st_nlink,
                    (input_stat.st_mode & S_IRWXU) ? "RWX" : "---", (input_stat.st_mode & S_IRGRP) ? "R--" : "---", (input_stat.st_mode & S_IROTH) ? "R--" : "---");

                 write(out, buff, strlen(buff));

                exit(9); //exit numar de linii scrise
            }

             if(pid2 < 0)
            {
                perror("eroare proces");
                exit(-1);
            }
            else if(pid2 == 0)
            {
                //convertire poza to be continued...

                exit(0);
            }
        } 
        else 
        {
            pid_t pid;
            pid=fork();

            if(pid < 0)
            {
                perror("eroare proces");
                exit(-1);
            }
            else if(pid == 0)
            {
                sprintf(buff, "nume fisier: %s\n dimensiune: %ld\n, identificarea utilizatorului: %d\n, numar legaturi: %lu\n, drepturi de acces user: %s\n, drepturi de acces multiplu: %s\n, drepturi de acces others: %s\n",
                    cale_str, input_stat.st_size, input_stat.st_uid, input_stat.st_nlink,
                    (input_stat.st_mode & S_IRWXU) ? "RWX" : "---", (input_stat.st_mode & S_IRGRP) ? "R--" : "---", (input_stat.st_mode & S_IROTH) ? "R--" : "---");

                 write(out, buff, strlen(buff));

                exit(7);

            }
            
        }
    } 
    else if (S_ISLNK(input_stat.st_mode))  //daca e leg simbolica
    {
        char cale2[257]; 
        ssize_t dim_cale = readlink(cale_str, cale2, sizeof(cale2) - 1);
        if (dim_cale == -1) {
            perror("eroare citire legatura simbolica");
            exit(-1);
        }
        cale2[dim_cale] = '\0';

            pid_t pid;
            pid=fork();

            if(pid < 0)
            {
                perror("eroare proces");
                exit(-1);
            }
            else if(pid == 0)
            {
                sprintf(buff, "nume legatura: %s\ndimensiune legatura: %ld\ndimensiune fisier target: %ld\ndrepturi de acces user legatura: %s\ndrepturi de acces grup legatura: %s\ndrepturi de acces altii legatura: %s\n",
                cale_str, input_stat.st_size, dim_cale, (input_stat.st_mode & S_IRWXU) ? "RWX" : "---",
                (input_stat.st_mode & S_IRGRP) ? "R--" : "---", (input_stat.st_mode & S_IROTH) ? "R--" : "---");
            
                write(out, buff, strlen(buff));

                exit(6);
            
            }

    } 
    else if (S_ISDIR(input_stat.st_mode)) 
    {
         pid_t pid;
            pid=fork();

            if(pid < 0)
            {
                perror("eroare proces");
                exit(-1);
            }
            else if(pid == 0)
            {
                sprintf(buff, "nume director: %s\n identificatorul utilizatorului: %d\n drepturi de acces user: %s\n drepturi de acces grup: %s\n drepturi de acces altii: %s\n",
                cale_str, input_stat.st_uid,
                (input_stat.st_mode & S_IRWXU) ? "RWX" : "---", (input_stat.st_mode & S_IRGRP) ? "R--" : "---", (input_stat.st_mode & S_IROTH) ? "R--" : "---");
            
                write(out, buff, strlen(buff));

                exit(5);

            }
    }

    if (close(out) == -1)
    {
        perror("error close");
        exit(EXIT_FAILURE);
    }
}
