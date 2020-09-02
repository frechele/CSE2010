#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void DebugString(const char* fmt, ...)
{
#ifdef PRINT_DEBUG
    va_list ap;

    va_start(ap, fmt);
    
    vfprintf(stderr, fmt, ap);
    va_end(ap);
#endif
}

#define TRUE  ((short int)1)
#define FALSE ((short int)0)

FILE* fpOut;

typedef struct tagThreadedTree
{
    short int leftThread;
    short int rightThread;

    struct tagThreadedTree* leftChild;
    char data;
    struct tagThreadedTree* rightChild;

} ThreadedTree, *ThreadedTreePtr;

void InitThreadedTree(ThreadedTreePtr tree);
void ReleaseThreadedTree(ThreadedTreePtr tree);
void InsertNode(ThreadedTreePtr tree, char data);
void InorderTraversal(ThreadedTreePtr tree);

ThreadedTreePtr insucc(ThreadedTreePtr tree);

int main()
{
    fpOut = fopen("output.txt", "wt");
    if (!fpOut)
    {
        printf("ERROR: Cannot open output.txt\n");
        return -1;
    }

    FILE* fp = fopen("input.txt", "rt");
    if (!fp)
    {
        fclose(fpOut);
        printf("ERROR: Cannot open input.txt\n");
        return -1;
    }

    ThreadedTree tree;
    InitThreadedTree(&tree);

    int N;
    fscanf(fp, "%d ", &N);

    for (int i = 0; i < N; ++i)
    {
        char data;
        fscanf(fp, "%c ", &data);

        DebugString("Insert: %c\n", data);
        InsertNode(&tree, data);
    }

    InorderTraversal(&tree);

    ReleaseThreadedTree(&tree);

    fclose(fp);
    fclose(fpOut);
}

// ** Queue impl ** //
typedef struct
{
    int capacity;
    int size;

    int rear, front;
    ThreadedTreePtr* data;
} Queue;

void CreateQueue(Queue* pQueue, int capacity)
{
    pQueue->capacity = capacity;
    pQueue->size = 0;

    pQueue->rear = 0;
    pQueue->front = 1;

    pQueue->data = (ThreadedTreePtr*)malloc(capacity * sizeof(ThreadedTreePtr));
}

void ReleaseQueue(Queue* pQueue)
{
    pQueue->capacity = 0;
    pQueue->size = 0;

    pQueue->rear = 0;
    pQueue->front = 1;

    free(pQueue->data);
}

int IsEmptyQueue(Queue* pQueue)
{
    return pQueue->size == 0;
}

int IsFullQueue(Queue* pQueue)
{
    return pQueue->size == pQueue->capacity;
}

int Enqueue(Queue* pQueue, ThreadedTreePtr value)
{
    if (IsFullQueue(pQueue))
        return FALSE;

    ++pQueue->size;
    pQueue->rear = (pQueue->rear + 1) % pQueue->capacity;
    pQueue->data[pQueue->rear] = value;

    return TRUE;
}

int Dequeue(Queue* pQueue, ThreadedTreePtr* pValue)
{
    if (IsEmptyQueue(pQueue))
        return FALSE;

    *pValue = pQueue->data[pQueue->front];
    pQueue->front = (pQueue->front + 1) % pQueue->capacity;
    --pQueue->size;

    return TRUE;
}

// ** TBT impl ** //
void InitThreadedTree(ThreadedTreePtr tree)
{
    // Set dummy root node
    tree->leftThread = TRUE;
    tree->rightThread = FALSE;

    tree->leftChild = tree;
    tree->rightChild = tree;

    tree->data = 'R';

    DebugString("Created Threaded Tree root\n");
}

void ReleaseThreadedTree(ThreadedTreePtr tree)
{
    Queue deleteQ;
    CreateQueue(&deleteQ, 1000);

    ThreadedTreePtr tempNowNode = tree;

    DebugString("Root address: %x\n", tree);

    while (TRUE)
    {
        tempNowNode = insucc(tempNowNode);
        DebugString("\ttempNowNode: %x\n", tempNowNode);

        if (tempNowNode == tree) break;

        Enqueue(&deleteQ, tempNowNode);
    }   

    DebugString("Start release nodes.\n");
   
    while (Dequeue(&deleteQ, &tempNowNode))
    {
        DebugString("\tfree node: %c\n", tempNowNode->data);
        free(tempNowNode);
    }

    ReleaseQueue(&deleteQ);

    DebugString("Released Threaded Tree\n");
}

void InsertNode(ThreadedTreePtr tree, char data)
{
    ThreadedTreePtr node = (ThreadedTreePtr)malloc(sizeof(ThreadedTree));
    node->data = data;
    node->leftThread = TRUE;
    node->rightThread = TRUE;

    if (tree->leftThread && tree->rightThread) // Empty tree
    {
        DebugString("Empty tree\n");

        tree->leftChild = node;
        tree->leftThread = FALSE;

        node->leftChild = tree;
        node->rightChild = tree;

        return;
    }

    Queue queue;
    CreateQueue(&queue, 1000);

    Enqueue(&queue, tree->leftChild);

    ThreadedTreePtr tempNowNode;
    while (TRUE)
    {
        Dequeue(&queue, &tempNowNode);
        DebugString("dequeued node: %c\n", tempNowNode->data);

        if (tempNowNode->leftThread) // left child is empty
        {
            DebugString("Insert node at left\n");

            node->leftChild = tempNowNode->leftChild;
            node->rightChild = tempNowNode;

            tempNowNode->leftChild = node;
            tempNowNode->leftThread = FALSE;

            break;
        }
        else if (tempNowNode->rightThread) // right child is empty
        {
            DebugString("Insert node at right\n");

            node->leftChild = tempNowNode;
            node->rightChild = tempNowNode->rightChild;

            tempNowNode->rightChild = node;
            tempNowNode->rightThread = FALSE;

            break;
        }
    
        if (!tempNowNode->leftThread)
        {
            DebugString("left is not null. enqueue\n");
            Enqueue(&queue, tempNowNode->leftChild);
        }
        if (!tempNowNode->rightThread)
        {
            DebugString("right is not null. enqueue\n");
            Enqueue(&queue, tempNowNode->rightChild);
        }
    }

    DebugString("Insertion done.\n\n");
    ReleaseQueue(&queue);
}

ThreadedTreePtr insucc(ThreadedTreePtr tree)
{
    ThreadedTreePtr tempNowNode = tree->rightChild;

    if (!tree->rightThread)
    {
        while (!tempNowNode->leftThread)
            tempNowNode = tempNowNode->leftChild;
    }

    return tempNowNode;
}

void InorderTraversal(ThreadedTreePtr tree)
{
    ThreadedTreePtr tempNowNode = tree;

    DebugString("Root address: %x\n", tree);

    while (TRUE)
    {
        tempNowNode = insucc(tempNowNode);
        DebugString("\ttempNowNode: %x\n", tempNowNode);

        if (tempNowNode == tree) break;

        fprintf(fpOut, "%c ", tempNowNode->data);       
    }
}

