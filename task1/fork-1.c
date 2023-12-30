/* fork-1.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

void *create_shared_memory(size_t size)
{
    return mmap(NULL,
                size,
                PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS,
                -1, 0);
}

int main()
{
    void *shmem = create_shared_memory(10 * sizeof(int));

    for (size_t i = 0; i < 10; i++)
    {
        ((int *)shmem)[i] = (int)i + 1;
    }

    printf("Shared memory at: %p\n", shmem);
    int pid = fork();

    if (pid == 0)
    {
        size_t index;
        int num;
        scanf("%zu %d", &index, &num);
        if (index >= 10)
        {
            printf("Index is out of range, max value is 10\n");
        }
        else
        {
            ((int *)shmem)[index] = num;
        }
    }
    else
    {
        printf("Child's pid is: %d\n", pid);
        wait(NULL);
        printf("Ten int numbers:\n");
        for (size_t i = 0; i < 10; i++)
        {
            printf("%d ", ((int *)shmem)[i]);
        }
    }
}