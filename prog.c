#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/file.h>

#define LOCK_SH 1 /*shared lock */
#define LOCK_EX 2 /*exclusive lock */
#define LOCK_NB 4 /*don't block when locking */
#define LOCK_UN 8 /*unlock */

const char* file_path;


int main(int argc, char* const* argv)
{
    int fd;
    int result = 0;
    while ((result = getopt(argc, argv, "f:")) != -1) {
        switch (result) {
        case 'f':
            printf("%s\n", optarg);

            file_path = optarg;
            // Проверить существование файла lck
            if (access(file_path, F_OK) == 0) {
                // если существует
                puts("File exists"); // заблокироваться до его удаления

                fd = open(file_path, O_RDWR, 0644);
                int lock = flock(fd, LOCK_SH); // Lock the file

            // struct flock flptr;
            // flptr.l_start=0;
            // flptr.l_whence=SEEK_SET;
            // flptr.l_len=0;
            // flptr.l_type=F_RDLCK;

            // if((fcntl(fd, F_SETLK, &flptr)!=-1))
            //   puts("Установлена блокировка по чтению"); 

            // flptr.l_start=0;
            // flptr.l_whence=SEEK_SET;
            // flptr.l_len=0;
            // flptr.l_type=F_WRLCK;

            // if((fcntl(fd, F_SETLK, &flptr)!=-1))
            //   puts("Установлена блокировка по записи"); 

            // flptr.l_type=F_UNLCK;
            // if((fcntl(fd, F_SETLK, &flptr)!=-1))
            // puts("Блокировка снята");
            }
            else {
                // если не существует, то создать
                puts("File doesn't exist");
                puts("File creating...");
                int fd = open(file_path, O_CREAT, 0644);
            }
        }
    }

    int self_pid = getpid();
    char self_pid_str[6];
    snprintf(self_pid_str, 6, "%d", self_pid);
    ;
    // Записать в этот файл свой pid
    write(fd, self_pid_str, sizeof(self_pid_str));

    // Выполнить чтение файла
    char sym;
    int i = 0;
    char buf[5];
    fd = open(file_path, O_RDWR, 0644);
    while (read(fd, &sym, 1) > 0) {
        buf[i] = sym;
        i++;
    }
    puts(buf);

    sleep(5);

    // Проверить, что файл существует и в нем записан свой pid

    if (access(file_path, F_OK) == 0) {
        if (atoi(buf) == self_pid) { // 1 - да, 0 - нет
            puts("File exists");
            puts("The file contains my PID");
        }

        // Если это так, то удалить файл
        if (remove(file_path) == 0) {
            printf("OK\n");
        } else {
            perror("error");
        } 
    }
    else {
        // если нет — завершиться с сообщением об ошибке
        puts("File doesn't exist");
        perror("error"); // No such file or directory
        return 1;
    }
    close(fd);
    return 0;
}
