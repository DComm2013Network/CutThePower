#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "GameplayCommunication.h"
#include "NetworkRouter.h"
#include "Functions.h"

int main()
{
    pthread_t thread;
    PDATA data = (PDATA) malloc(sizeof(PDATA));
    int fd[2];

    create_pipe(fd);

    data->read_pipe = fd[0];
    data->write_pipe = fd[1];

    pthread_create(&thread, NULL, networkRouter, (void *)&data);
    pthread_detach(thread);

    char message[128];
    int result = read_pipe(fd[0], message, 128);

    if(result < 0)
    {
        printf("%s\n", "Couldn't read to pipe!");
    }

    printf("%s\n", message);

    printf("%s\n", "Finished");
}