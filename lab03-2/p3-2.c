#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int capacity;
    int size;
    int rear, front;
    int* data;
} Queue;

void CreateQueue(Queue* pQueue, int capacity);
void ReleaseQueue(Queue* pQueue);

int Enqueue(Queue* pQueue, int value);
int Dequeue(Queue* pQueue, int* pValue);

int IsFullQueue(Queue* pQueue);
int IsEmptyQueue(Queue* pQueue);

int main()
{
    FILE* fpOut = fopen("output.txt", "wt");
    if (!fpOut)
    {
        printf("ERROR: Cannot open output.txt\n");
        return 1;
    }

    FILE* fp = fopen("input.txt", "rt");
    if (!fp)
    {
        printf("ERROR: Cannot open input.txt\n");
        fclose(fpOut);
        return 1;
    }

    Queue queue;
    CreateQueue(&queue, 100);
    
    int T;
    fscanf(fp, "%d", &T);

    char command[128];
    int arg;
    for (int t = 0; t < T; ++t)
    {
        fscanf(fp, "%s", command);

        if (strcmp(command, "enQ") == 0)
        {
            fscanf(fp, "%d", &arg);
            if (!Enqueue(&queue, arg))
            {
                fprintf(fpOut, "Full\n");
            }
        }
        else if (strcmp(command, "deQ") == 0)
        {
            if (Dequeue(&queue, &arg))
            {
                fprintf(fpOut, "%d\n", arg);
            }
            else
            {
                fprintf(fpOut, "Empty\n");
            }
        }
        else
        {
            fprintf(fpOut, "ERROR: Wrong command\n");
        }
    }

    ReleaseQueue(&queue);

    fclose(fp);
    fclose(fpOut);
}

void CreateQueue(Queue* pQueue, int capacity)
{
    pQueue->capacity = capacity;
    pQueue->size = 0;
    pQueue->rear = 0;
    pQueue->front = 1;
    pQueue->data = (int*)malloc(capacity * sizeof(int));
}

void ReleaseQueue(Queue* pQueue)
{
    free(pQueue->data);
    pQueue->data = NULL;

    pQueue->size = 0;
    pQueue->rear = 0;
    pQueue->front = 1;
    pQueue->capacity = 0;
}

int Enqueue(Queue* pQueue, int value)
{
    if (IsFullQueue(pQueue))
    {
        return 0;
    }

    ++pQueue->size;
    pQueue->rear = (pQueue->rear + 1) % pQueue->capacity;
    pQueue->data[pQueue->rear] = value;
    return 1;
}

int Dequeue(Queue* pQueue, int* pValue)
{
    if (IsEmptyQueue(pQueue))
    {
        return 0;
    }

    *pValue = pQueue->data[pQueue->front];
    pQueue->front = (pQueue->front + 1) % pQueue->capacity;
    --pQueue->size;
    return 1;
}

int IsFullQueue(Queue* pQueue)
{
    return (pQueue->size == pQueue->capacity);
}

int IsEmptyQueue(Queue* pQueue)
{
    return (pQueue->size == 0);
}
