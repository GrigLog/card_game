#pragma once
#include <unistd.h>

class TPipeWrapper {
    int fd[2];

public:
    TPipeWrapper() {
        pipe(fd);
    }

    ~TPipeWrapper() {
        if (fd[0] >= 0) {
            close(fd[0]);
        }
        if (fd[1] >= 0) {
            close(fd[1]);
        }
    }

    int getReadFd() {
        return fd[0];
    }
    int getWriteFd() {
        return fd[1];
    }
};