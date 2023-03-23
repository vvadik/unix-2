#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <signal.h>

typedef struct fileLock {
    char *path;
    size_t len;
} fileLock;

void getLockFilePath(fileLock lock, char *buf) {
    sprintf(buf, "%s.lck", lock.path);
}

void writeAll(int fd, char *data, size_t len) {
    while (len) {
        int written = write(fd, data, len);
        if (written < 0) {
            perror("unexpected error on write syscall");
            exit(1);
        }

        data += written;
        len -= written;
    }
}

void readAll(int fd, char *data, size_t len) {
    while (len) {
        int dataRead = read(fd, data, len);
        if (dataRead < 0) {
            perror("unexpected error on read syscall");
            exit(1);
        }

        data += dataRead;
        len -= dataRead;
    }
}

// 100 milliseconds
const size_t sleepIntervalMicroseconds = 5000;

size_t getSleepDurationMicroseconds() {
    int val = 50 + rand() % 50; // 50 <= x < 100
    return (size_t)((double)sleepIntervalMicroseconds * (val / 100.0));
}

void Lock(fileLock lock) {
    char buf[1024];
    getLockFilePath(lock, buf);

    int lockFd = 0;
    for (;;) {
        // sleep some time
        // yes, we need to do it before checking, to allow other processes take the lock
        // otherwise, much more likely, lock will be acquired by the process, that locked the lock in previous time
        // because other processes are sleeping
        usleep(getSleepDurationMicroseconds());

        // we need to atomically check if file is already exist
        // that's why flag O_EXCL is added
        lockFd = open(buf, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR);
        if (lockFd >= 0) {
            // we've created file, no need in busy wait anymore, lock is ours
            break;
        }

        // lockfd < 0 => error occurred
        if (errno != EEXIST) {
            // we are expecting only EEXIST error here, if other met, need to fast fail
            perror("unexpected error met");
            exit(1);
        }
    }

    // successfully locked file, need to write ours pid in it
    int len = sprintf(buf, "%d", getpid());
    if (len < 0) {
        // not necessary, because we are exiting and os will clean up resources, but good habbit
        close(lockFd);
        perror("unexpected spritnf error");
        exit(1);
    }

    writeAll(lockFd, buf, len);
    // don't forget to close file
    close(lockFd);
}

// unlock don't need to be atomic, because it is in critical section
void Unlock(fileLock lock) {
    char buf[1024];
    getLockFilePath(lock, buf);

    int lockFd = open(buf, O_RDONLY);
    if (lockFd < 0) {
        perror("cannot open locked file");
        exit(1);
    }

    struct stat statBuf;
    if (fstat(lockFd, &statBuf) < 0) {
        close(lockFd);
        perror("cannot get file size for reading pid from it");
        exit(1);
    }

    // file opened, checking pid
    readAll(lockFd, buf, statBuf.st_size);
    close(lockFd); // we may close file after reading info rom it

    // convert to int
    char *strEnd;
    pid_t pid = strtol(buf, &strEnd, 10);
    if (strEnd == buf) {
        perror("cannot parse pid to int");
        exit(1);
    }

    if (pid != getpid()) {
        perror("pid in lock file is not equal to ours pid, external error");
        exit(1);
    }

    getLockFilePath(lock, buf);
    if (remove(buf) < 0) {
        perror("cannot remove lock file for unlocking");
        exit(1);
    }

    // file removed => other processes may acquire the lock now
}

char stop = 0;

void handleSignal(int signum) {
    stop = 1;
}

int main(int argc, char *argv[]) {
    // init random
    srand(time(NULL));

    if (argc != 2) {
        perror("usage: ./lock <filename>");
        exit(1);
    }

    if (signal(SIGINT, handleSignal) == SIG_ERR) {
        perror("cannot register signal handler");
        exit(1);
    }

    char *f = argv[1];
    size_t len = strlen(f);

    fileLock lock = {f, len};

    size_t lockTaken = 0;
    while (!stop) {
        Lock(lock);
        lockTaken++;

        if (stop) {
            Unlock(lock);
            break;
        }

        sleep(1);

        Unlock(lock);
    }

    char buf[1024];
    size_t stat_len = sprintf(buf, "%d: %d\n", getpid(), lockTaken);

    {
        Lock(lock);

        int fd = open(f, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        if (fd < 0) {
            perror("cannot open stats file");
            exit(1);
        }

        writeAll(fd, buf, stat_len);

        close(fd);

        Unlock(lock);
    }

    return 0;
}
