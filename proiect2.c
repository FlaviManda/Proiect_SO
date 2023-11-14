#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        printf("eroare argumente");
        exit(-1);
    }

    struct stat file_info;
    struct stat dir_info;
    int temp
    char output_file[] = "statistica.txt";
    char buff[1000];  
    
    int out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (out == -1) 
    {
        printf("eroare deschidere");
        exit(-1);
    }


    if (!S_ISDIR(dir_info.st_mode)) 
    {
        printf("nu e director");
        close(out);
        exit(-1);
    }

    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
        printf("eroare director");
        close(out);
        exit(-1);
    }

    while (1) 
    {
        struct dirent *entry = readdir(dir);
        if (entry == NULL) 
        {
            break;
        }

        sprintf(buff, "%s/%s", argv[1], entry->d_name);
       

            temp = sprintf(buff,"nume: %s\n"
            "dimensiune: %ld octeti\n"
            "identificatorul utilizatorului: %d\n"
            "contorul de legaturi: %ld\n"
            "drepturi de acces user: %s\n"
            "drepturi de acces grup: %s\n"
            "drepturi de acces altii: %s\n",
            entry->d_name, file_info.st_size, file_info.st_uid, file_info.st_nlink,
            (file_info.st_mode & S_IRUSR) ? "R" : "-",
            (file_info.st_mode & S_IWUSR) ? "W" : "-",
            (file_info.st_mode & S_IXUSR) ? "X" : "-",
            (file_info.st_mode & S_IRGRP) ? "R" : "-",
            (file_info.st_mode & S_IWGRP) ? "W" : "-",
            (file_info.st_mode & S_IXGRP) ? "X" : "-",
            (file_info.st_mode & S_IROTH) ? "R" : "-",
            (file_info.st_mode & S_IWOTH) ? "W" : "-",
            (file_info.st_mode & S_IXOTH) ? "X" : "-"
        );

        if (S_ISREG(file_info.st_mode) && strstr(entry->d_name, ".bmp") != NULL) 
        {
            int bmp_fd = open(buff, O_RDONLY);

            if (bmp_fd != -1) 
            {
                lseek(bmp_fd, 18, SEEK_SET);
                int width, height;
                read(bmp_fd, &width, sizeof(int));

                lseek(bmp_fd, 22, SEEK_SET);
                read(bmp_fd, &height, sizeof(int));

                sprintf(buff,
                    "inaltime: %d\n"
                    "lungime: %d\n", height, width);
                close(bmp_fd);
            }
        }

        write(out, buff, temp);
    }

    closedir(dir);
    close(out);

    return 0;
}
