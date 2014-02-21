#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

//#include "GameplayCommunication.h"
#include "NetworkRouter.h"

int main()
{
    pthread_t thread;
    int fd[2];

    pipe(fd);

    printf("%s", "Creating thread..");
    pthread_create(&thread, NULL, networkRouter, (void *)&fd[1]);
    pthread_detach(thread);
    printf("%s\n", "done!");

    char message[128];
    int result = read(fd[0], message, 128);

    if(result < 0)
    {
        printf("%s\n", "Couldn't read to pipe!");
    }

    printf("%s\n", message);

    printf("%s\n", "Finished");
}