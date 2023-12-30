/* fork-2.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
    int pipes[2][2];
    pipe(pipes[0]);
    pipe(pipes[1]);

    void *shmem = create_shared_memory(10 * sizeof(int));

    for (size_t i = 0; i < 10; i++)
    {
        ((int *)shmem)[i] = (int)i + 1;
    }

    printf("Shared memory at: %p\n", shmem);
    int pid = fork();

    if (pid == 0)
    {
        int to_parent_pipe = pipes[1][1];
        int from_parent_pipe = pipes[0][0];
        close(pipes[1][0]);
        close(pipes[0][1]);
        int8_t send_byte = 1;
        int8_t index_is_incorrect_byte = 2;
        int index;
        int num;
        int index_is_correct = 1;
        while (index_is_correct)
        {
            printf("Enter index: ");
            scanf("%d", &index);
            if (index < 0)
            {
                index_is_correct = 0;
                write(to_parent_pipe, &index_is_incorrect_byte, 1);
                break;
            }

            if (index >= 10)
            {
                printf("Index is out of range, max value is 10\n");
            }
            else
            {
                printf("Enter new value: ");
                scanf("%d", &num);
                ((int *)shmem)[(size_t)index] = num;
                write(to_parent_pipe, &send_byte, 1);
                sleep(1);
            }
        }
        close(to_parent_pipe);
        close(from_parent_pipe);
    }
    else
    {
        int from_child_pipe = pipes[1][0];
        int to_child_pipe = pipes[0][1];
        close(pipes[1][1]);
        close(pipes[0][0]);
        printf("Child's pid is: %d\n", pid);
        int8_t receive_byte;
        int is_working = 1;
        while (is_working)
        {
            while (read(from_child_pipe, &receive_byte, 1) == 0)
                ;
            if (receive_byte == 1)
            {
                printf("Ten int numbers:\n");
                for (size_t i = 0; i < 10; i++)
                {
                    printf("%d ", ((int *)shmem)[i]);
                }
                printf("\n");
            }
            else if (receive_byte == 2)
            {
                is_working = 0;
            }
        }
        waitpid(pid, NULL, 0);

        close(from_child_pipe);
        close(to_child_pipe);
    }
}