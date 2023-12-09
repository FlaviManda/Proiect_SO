#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

void process_input(char *director, struct dirent *input, char *director2);

int main(int argc, char *argv[]) 
{
    DIR *dir = opendir(argv[1]);
    DIR *dir2 = opendir(argv[2]);

    pid_t pid;
    int status;

    if(!dir2)
    {
        perror("eroare deschidere director 2");
        exit(-1);
    }

    if (!dir) 
    {
        perror("eroare deschidere director");
        exit(-1);
    }

    struct dirent *input;
    while ((input = readdir(dir)) != NULL) 
    {
        if (strcmp(input->d_name, ".") == 0 || strcmp(input->d_name, "..") == 0) {
            continue;
        }

        process_input(argv[1], input, argv[2]);
    }

    while ((pid = wait(&status)) != -1)
        if (WIFEXITED(status))
        {
            printf("Child with pid = %d ended with status %d\n", pid, WEXITSTATUS(status));
        }

    closedir(dir);

    return 0;
}

void process_input(char *director, struct dirent *input, char *director2) 
{
    char cale_stat[500];
    char cale_str[257];
    snprintf(cale_str, sizeof(cale_str), "%s/%s", director, input->d_name);

    snprintf(cale_stat, sizeof(cale_stat), "%s/%s_statistica.txt", director2, input->d_name);

    struct stat input_stat;
    if (lstat(cale_str, &input_stat) == -1) 
    {
        perror("eroare stat");
        exit(-1);
    }

    int bmp_fd;
    int width, height;

    int out = open(cale_stat, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (out == -1) 
    {
        perror("eroare deschidere fisier de iesire");
        exit(-1);
    }

    if (S_ISREG(input_stat.st_mode) && strstr(input->d_name, ".bmp") != NULL) 
    {
        bmp_fd = open(cale_str, O_RDONLY);
        if (bmp_fd == -1) 
        {
            perror("eroare deschidere");
            exit(-1);
        }

        lseek(bmp_fd, 18, SEEK_SET);
        read(bmp_fd, &width, sizeof(int));
        lseek(bmp_fd, 22, SEEK_SET);
        read(bmp_fd, &height, sizeof(int));

        close(bmp_fd);
    }

    char buff[3000];

    if (S_ISREG(input_stat.st_mode)) 
    {
        if (strstr(input->d_name, ".bmp") != NULL) 
        {
            pid_t pid;
            pid=fork();

            pid_t pid2;
            pid2=fork();

            if(pid < 0)
            {
                perror("eroare proces");
                exit(-1);
            }
            else if(pid == 0)
            {
                 sprintf(buff, "nume fisier: %s\n inaltime: %d\n, lungime: %d\n, dimensiune: %ld\n, identificarea utilizatorului: %d\n, numar legaturi: %lu\n, drepturi de acces user: %s\n, drepturi de acces multiplu: %s\n, drepturi de acces others: %s\n",
                    cale_str, height, width, input_stat.st_size, input_stat.st_uid, input_stat.st_nlink,
                    (input_stat.st_mode & S_IRWXU) ? "RWX" : "---", (input_stat.st_mode & S_IRGRP) ? "R--" : "---", (input_stat.st_mode & S_IROTH) ? "R--" : "---");

                 write(out, buff, strlen(buff));

                exit(9);
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
    else if (S_ISLNK(input_stat.st_mode)) 
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
