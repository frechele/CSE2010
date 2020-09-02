#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* fpOut;

typedef int ElementType;

typedef struct tagPriorityQueue {
    int Capacity;
    int Size;
    ElementType* Elements;
} PriorityQueue, *PriorityQueuePtr;

void CreatePriorityQueue(PriorityQueuePtr H, int capacity);
void ReleasePriorityQueue(PriorityQueuePtr H);

void Insert(ElementType X, PriorityQueuePtr H);
void Find(ElementType X, PriorityQueuePtr H);
void Print(PriorityQueuePtr H);

short IsFull(PriorityQueuePtr H);

int main()
{
    fpOut = fopen("output.txt", "wt");
    if (!fpOut)
    {
        printf("ERROR: Cannot open output.txt\n");
        return 1;
    }
    
    FILE* fp = fopen("input.txt", "rt");
    if (!fp)
    {
        printf("ERROR: Cannot open input.txt\n");
        fclose(fp);
        return 1;
    }

    int capacity;
    fscanf(fp, "%d", &capacity);

    PriorityQueue pq;
    CreatePriorityQueue(&pq, capacity);

    char command[128];
    int value;
    while (fscanf(fp, "%s", command) != EOF)
    {
        if (strcmp(command, "i") == 0)
        {
            fscanf(fp, "%d", &value);
            Insert(value, &pq);
        }
        else if (strcmp(command, "f") == 0)
        {
            fscanf(fp, "%d", &value);
            Find(value, &pq);
        }
        else if (strcmp(command, "p") == 0)
        {
            Print(&pq);
            fprintf(fpOut, "\n");
        }
    }

    ReleasePriorityQueue(&pq);

    fclose(fp);
    fclose(fpOut);
}

void CreatePriorityQueue(PriorityQueuePtr H, int capacity)
{
    H->Capacity = capacity;
    H->Size = 0;

    H->Elements = (ElementType*)malloc(sizeof(ElementType) * (capacity + 1));
}

void ReleasePriorityQueue(PriorityQueuePtr H)
{
    free(H->Elements);
    H->Elements = NULL;

    H->Capacity = 0;
    H->Size = 0;
}

void Insert(ElementType X, PriorityQueuePtr H)
{
    if (IsFull(H))
    {
        fprintf(fpOut, "Heap is full\n");
        return;
    }

    int i;

    for (i = 1; i <= H->Size; ++i)
    {
        if (H->Elements[i] == X)
        {
            fprintf(fpOut, "%d is already in the heap.\n", X);
            return;
        }
    }

    for (i = ++H->Size; i != 1 && H->Elements[i/2] < X; i /= 2)
    {
        H->Elements[i] = H->Elements[i/2];
    }
    H->Elements[i] = X;

    fprintf(fpOut, "insert %d\n", X);
}

void Find(ElementType X, PriorityQueuePtr H)
{
    for (int i = 1; i <= H->Size; ++i)
    {
        if (H->Elements[i] == X)
        {
            fprintf(fpOut, "%d is in the heap.\n", X);
            return;
        }
    }

    fprintf(fpOut, "%d is not in the heap.\n", X);    
}

void Print(PriorityQueuePtr H)
{
    for (int i = 1; i <= H->Size; ++i)
    {
        fprintf(fpOut, "%d ", H->Elements[i]);       
    }
}

short IsFull(PriorityQueuePtr H)
{
    return (H->Size == H->Capacity);
}

