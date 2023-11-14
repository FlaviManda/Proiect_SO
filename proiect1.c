#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{    
    int width;
    int height;
    int dimensiune;
    char *input=argv[1];
    char buff[1000];
    char multipleacc[3]="";
    char useracc[3];
    int bmp_fd=open(input, O_RDONLY);

    struct stat size;

    int out=open("statistica.txt",O_WRONLY);
    if( bmp_fd == -1)
    {
        perror("eroare deschidere");
        exit(-1);
    }
    
    lseek(bmp_fd, 18, SEEK_SET);
    read(out, &width, sizeof(int));
    lseek(bmp_fd, 22, SEEK_SET);
    read(out, &height, sizeof(int));

    if( stat("image.bmp", &size)==-1 )
        {
        perror("eroare stat");
        exit(-1);
        }
    
    dimensiune=size.st_size;

    int user=size.st_uid;
    
    int control_leg=size.st_nlink;

    if(size.st_mode & S_IRWXU)
    {
        useracc[0]='R';
    }
    else{
        useracc[0]='-';
    }

    if(size.st_mode & S_IWUSR)
    {
        useracc[1]='w';
    }
    else{
        useracc[1]='-';
    }

    if(size.st_mode & S_IXUSR)
    {
        useracc[2]='E';
    }
    else{
        useracc[2]='-';
    }

    
    if(size.st_mode & S_IRGRP)
    {
        multipleacc[0]='R';
    }

    else{
        multipleacc[0]='-';
    }

    if(size.st_mode & S_IWGRP )
    {
        multipleacc[1]='W';
    }
    else{
        multipleacc[1]='-';
    }

    if(size.st_mode & S_IXGRP)
    {
        multipleacc[2]='E';
    }
    else{
        multipleacc[2]='-';
    }
        char others[3]="";
        if(size.st_mode & S_IROTH)
    {
        others[0]='R';
    }
    else{
        others[0]='-';
    }

    if(size.st_mode & S_IWOTH  )
    {
        others[1]='W';
    }
    else{
        others[1]='-';
    }

    if(size.st_mode & S_IROTH)
    {
        others[2]='E';
    }
    else{
        others[2]='-';
    }
        

    sprintf(buff,"nume fisier: %s\n inaltime: %d\n, lungime: %d\n, dimensiune: %d\n, identificarea utilizatorului: %d\n, numar legaturi: %d\n, drepturi de acces user: %s\n, drepturi de acces multiplu: %s\n, drepturi de acces others: %s\n", argv[1], height, width, dimensiune, user, control_leg, useracc, multipleacc, others);
    
    write(out, buff, strlen(buff));
    
    if(close(bmp_fd)==-1)
        {
            perror("error close");
            exit(1);
        }
        
    close(out);

    return 0;
}