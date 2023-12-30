/* sem-1.c */

#include <semaphore.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

sem_t sem;
pthread_t t1, t2;
int nums[10];
int index_is_correct = 1;

void *thread1_task(void *_)
{
    int index;
    int num;
    while (index_is_correct)
    {
        printf("Enter index: ");
        scanf("%d", &index);
        if (index < 0)
        {
            index_is_correct = 0;
            sem_post(&sem);
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
            nums[(size_t)index] = num;
            sem_post(&sem);
            sleep(1);
        }
    }
    return NULL;
}

void *thread2_task(void *_)
{
    while (index_is_correct)
    {
        printf("Ten int numbers:\n");
        for (size_t i = 0; i < 10; i++)
        {
            printf("%d ", nums[i]);
        }
        printf("\n");
        sem_wait(&sem);
    }
    return NULL;
}

int main()
{
    for (size_t i = 0; i < 10; i++)
    {
        nums[i] = (int)i + 1;
    }

    sem_init(&sem, 0, 0);
    pthread_create(&t2, NULL, thread2_task, NULL);
    sleep(1);
    pthread_create(&t1, NULL, thread1_task, NULL);

    sem_destroy(&sem);
    pthread_exit(NULL);
    return 0;
}