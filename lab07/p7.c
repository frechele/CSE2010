#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <stdbool.h>

FILE* fpOut;

typedef struct
{
    int size;
    int* data;
    int count;
} DisjointSet;

typedef struct
{
    int r1, r2;
} Edge;

void CreateDisjointSet(DisjointSet* pSet, int size);
void ReleaseDisjointSet(DisjointSet* pSet);

void Union(DisjointSet* pSet, int r1, int r2);
int Find(DisjointSet* pSet, int value);

void CreateMaze(int size);

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

    srand((unsigned)time(NULL));

    int size;
    fscanf(fp, "%d", &size);

    if (size > 0)
        CreateMaze(size);

    fclose(fp);
    fclose(fpOut);
}

void CreateDisjointSet(DisjointSet* pSet, int size)
{
    pSet->size = size;
    pSet->count = size;
    pSet->data = (int*)malloc((size + 1) * sizeof(int));

    for (int i = 1; i <= size; ++i)
        pSet->data[i] = 0;
}

void ReleaseDisjointSet(DisjointSet* pSet)
{
    free(pSet->data);
    pSet->size = 0;
    pSet->count = 0;
}

void Union(DisjointSet* pSet, int r1, int r2)
{
    if (pSet->data[r2] < pSet->data[r1])
    {
        pSet->data[r1] = r2;
    }
    else
    {
        if (pSet->data[r1] == pSet->data[r2])
            --pSet->data[r1];
        pSet->data[r2] = r1;
    }

    --pSet->count;
}

int Find(DisjointSet* pSet, int value)
{
    if (pSet->data[value] > 0)
        return (pSet->data[value] = Find(pSet, pSet->data[value]));

    return value;
}

void CreateMaze(int size)
{
    const int sqSize = size * size;

    DisjointSet set;
    CreateDisjointSet(&set, sqSize);

    // Make edges
    int numOfEdges = 0;
    Edge* edges = (Edge*)malloc(2 * size * (size - 1) * sizeof(Edge));

    for (int i = 0; i < sqSize; ++i)
    {
        const int x = i % size;
        const int y = i / size;

        if (x + 1 < size)
        {
            edges[numOfEdges].r1 = i + 1;
            edges[numOfEdges].r2 = (x + 1) + y * size + 1;
            ++numOfEdges;
        }
        if (y + 1 < size)
        {
            edges[numOfEdges].r1 = i + 1;
            edges[numOfEdges].r2 = x + (y + 1) * size + 1;
            ++numOfEdges;
        }
    }

    // Fill buffer with full-edged maze
    const int sizeOfBuf = size * 2 + 1;
    char** buffer = (char**)malloc(sizeOfBuf * sizeof(char*));
    for (int y = 0; y < sizeOfBuf; ++y)
    {
        buffer[y] = (char*)malloc((sizeOfBuf + 1) * sizeof(char));

        for (int x = 0; x < sizeOfBuf; ++x)
        {
            if (y % 2 == 0)
                buffer[y][x] = (x % 2) ? '-' : '+';
            else
                buffer[y][x] = (x % 2) ? ' ' : '|';
        }
        buffer[y][sizeOfBuf] = 0;
    }
    // Erase edges at start or end point
    buffer[1][0] = ' '; // start point
    buffer[sizeOfBuf - 2][sizeOfBuf - 1] = ' '; // end point

    int picked, u, v, top = numOfEdges;
    while (numOfEdges > 0 && set.count > 1)
    {
        picked = rand() % top;

        u = Find(&set, edges[picked].r1);
        v = Find(&set, edges[picked].r2);

        if (u != v)
        {
            Union(&set, u, v);

            const int x = (edges[picked].r1 - 1) % size + 1;
            const int y = (edges[picked].r1 - 1) / size + 1;

            const bool isRightEdge = (edges[picked].r2 - edges[picked].r1) == 1;

            if (isRightEdge)
            {
                buffer[2 * y - 1][2 * x] = ' ';
            }
            else
            {
                buffer[2 * y][2 * x - 1] = ' ';
            }
        }
        
        edges[picked] = edges[--top];
    }

    for (int y = 0; y < sizeOfBuf; ++y)
    {
        fprintf(fpOut, "%s\n", buffer[y]);
        free(buffer[y]);
    }
    free(buffer);

    free(edges);

    ReleaseDisjointSet(&set);
}

