#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY_TOS (-1)

typedef struct
{
    int capacity;
    int topOfStack;
    int* data;
} Stack;

void CreateStack(Stack* pStack, int capacity);
void ReleaseStack(Stack* pStack);

int PushStack(Stack* pStack, int value);
int PopStack(Stack* pStack, int* pValue);

int IsFullStack(Stack* pStack);
int IsEmptyStack(Stack* pStack);

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

    Stack stack;
    CreateStack(&stack, 100);

    int T;
    fscanf(fp, "%d", &T);

    char command[128];
    int arg;

    for (int t = 0; t < T; ++t)
    {
        fscanf(fp, "%s", command);

        if (strcmp(command, "push") == 0)
        {
            fscanf(fp, "%d", &arg);
            if (!PushStack(&stack, arg))
            {
                fprintf(fpOut, "Full\n");
            }
        }
        else if (strcmp(command, "pop") == 0)
        {
            if (PopStack(&stack, &arg))
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

    ReleaseStack(&stack);

    fclose(fp);
    fclose(fpOut);
}

void CreateStack(Stack* pStack, int capacity)
{
    pStack->capacity = capacity;
    pStack->topOfStack = EMPTY_TOS;
    pStack->data = (int*)malloc(capacity * sizeof(int));
}

void ReleaseStack(Stack* pStack)
{
    free(pStack->data);
    pStack->data = NULL;
    
    pStack->topOfStack = EMPTY_TOS;
    pStack->capacity = 0;
}

int PushStack(Stack* pStack, int value)
{
    if (IsFullStack(pStack))
    {
        return 0;
    }

    pStack->data[++pStack->topOfStack] = value;
    return 1;
}

int PopStack(Stack* pStack, int* pValue)
{
    if (IsEmptyStack(pStack))
    {
        return 0;
    }

    *pValue = pStack->data[pStack->topOfStack--];
    return 1;
}

int IsFullStack(Stack* pStack)
{
    return (pStack->topOfStack == pStack->capacity - 1);
}

int IsEmptyStack(Stack* pStack)
{
    return (pStack->topOfStack == EMPTY_TOS);
}
