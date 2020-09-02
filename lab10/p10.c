#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_NODES (9)

FILE* fpOut;

typedef struct _Graph
{
	int size;
	int* node;
	int** matrix;
} Graph, *GraphPtr;

Graph CreateGraph(int* nodes, int size);
void DeleteGraph(GraphPtr G);
void PrintAdjacentMatrix(GraphPtr G);
void InsertEdge(GraphPtr G, int a, int b);
void Topsort(GraphPtr G);

typedef struct _Queue
{
	int* key;

	int first;
	int rear;
	int qsize;
	int max_queue_size;
} Queue, *QueuePtr;

Queue MakeNewQueue(int X);
void DeleteQueue(QueuePtr Q);
char IsEmpty(QueuePtr Q);
void Enqueue(QueuePtr Q, int X);
int Dequeue(QueuePtr Q);
int Front(QueuePtr Q);

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

	int num_of_nodes = 0;
	int nodes[MAX_NODES] = { 0 };

	int value;
	while ((value = fgetc(fp)) != '\n')
	{
		if (isspace(value)) continue;

		nodes[num_of_nodes++] = value - '0';
	}

	Graph graph = CreateGraph(nodes, num_of_nodes);
	
	int a, b;
	while (fscanf(fp, "%d-%d", &a, &b) != EOF)
	{
		InsertEdge(&graph, a, b);	
	}

	fprintf(fpOut, "Adjacency matrix\n  ");
	PrintAdjacentMatrix(&graph);

	fprintf(fpOut, "\n\nTopSort Result : ");
	Topsort(&graph);
	fputc('\n', fpOut);

	DeleteGraph(&graph);

	fclose(fp);
	fclose(fpOut);
}

Graph CreateGraph(int* nodes, int size)
{
	Graph graph;

	graph.size = size;

	graph.node = malloc(sizeof(int) * size);
	for (int i = 0; i < size; ++i)
		graph.node[i] = nodes[i];

	graph.matrix = malloc(sizeof(int*) * size);
	for (int i = 0; i < size; ++i)
	{
		graph.matrix[i] = malloc(sizeof(int) * size);
		for (int j = 0; j < size; ++j)
			graph.matrix[i][j] = 0;
	}

	return graph;
}

void DeleteGraph(GraphPtr G)
{
	free(G->node);

	for (int i = 0; i < G->size; ++i)
		free(G->matrix[i]);

	free(G->matrix);
	G->size = 0;
}

void PrintAdjacentMatrix(GraphPtr G)
{
	for (int i = 0; i < G->size; ++i)
		fprintf(fpOut, "%d ", G->node[i]);
	fputc('\n', fpOut);

	for (int y = 0; y < G->size; ++y)
	{
		fprintf(fpOut, "%d ", G->node[y]);

		for (int x = 0; x < G->size; ++x)
		{
			fprintf(fpOut, "%d ", G->matrix[y][x]);
		}

		fputc('\n', fpOut);
	}
}

void InsertEdge(GraphPtr G, int a, int b)
{
	for (int y = 0; y < G->size; ++y)
		if (G->node[y] == a)
			for (int x = 0; x < G->size; ++x)
				if (G->node[x] == b)
				{
					G->matrix[y][x] = 1;
					return;
				}
}

void checkIndegree(GraphPtr G, int* indegree)
{
	for (int x = 0; x < G->size; ++x)
	{
		indegree[x] = 0;

		for (int y = 0; y < G->size; ++y)
			indegree[x] += (G->matrix[y][x]);
	}
}

void swap(int* a, int *b)
{
	int tmp = *a;

	*a = *b;
	*b = tmp;
}

void heapify(int* key, int* value, int size, int index)
{
	int child;

	for (; index * 2 <= size; index = child)
	{
		child = index * 2;

		if (child + 1 <= size && value[key[child - 1]] < value[key[child]])
			++child;

		if (value[key[index - 1]] < value[key[child - 1]])
			swap(&key[child - 1], &key[index - 1]);
		else
			break;
	}
}

void heapSort(GraphPtr G, QueuePtr Q)
{
	for (int i = Q->qsize / 2; i > 0; --i)
		heapify(Q->key, G->node, Q->qsize, 1);

	for (int i = Q->qsize - 1; i > 0; --i)
	{
		swap(&Q->key[0], &Q->key[i]);
		heapify(Q->key, G->node, i, 1);
	}
}

void Topsort(GraphPtr G)
{
	Queue queue = MakeNewQueue(G->size);
	Queue queue2 = MakeNewQueue(G->size);

	int indegree[MAX_NODES];
	checkIndegree(G, indegree);

	for (int i = 0; i < G->size; ++i)
		if (indegree[i] == 0)
			Enqueue(&queue, i);

	heapSort(G, &queue);

	int V;
	while (!IsEmpty(&queue))
	{
		V = Dequeue(&queue);
		fprintf(fpOut, "%d ", G->node[V]);

		for (int i = 0; i < G->size; ++i)
			if (G->matrix[V][i] && --indegree[i] == 0)
				Enqueue(&queue2, i);

		heapSort(G, &queue2);

		for (int i = 0; i < queue2.qsize; ++i)
		{
			Enqueue(&queue, Front(&queue2));
			Dequeue(&queue2);
		}
	}

	DeleteQueue(&queue);
	DeleteQueue(&queue2);
}

Queue MakeNewQueue(int X)
{
	Queue queue;

	queue.key = malloc(sizeof(int) * X);
	
	queue.first = 0;
	queue.rear = 0;
	queue.qsize = 0;
	queue.max_queue_size = X;

	return queue;
}

void DeleteQueue(QueuePtr Q)
{
	free(Q->key);

	Q->first = 0;
	Q->rear = 0;
	Q->qsize = 0;
	Q->max_queue_size = 0;
}

char IsEmpty(QueuePtr Q)
{
	return (Q->qsize == 0);
}

void Enqueue(QueuePtr Q, int X)
{
	if (Q->qsize == Q->max_queue_size)
	{
		fprintf(stderr, "Queue is full\n");
		return;
	}

	++Q->qsize;

	Q->key[Q->rear] = X;
	Q->rear = (Q->rear + 1) % Q->max_queue_size;
}

int Dequeue(QueuePtr Q)
{
	if (Q->qsize == 0)
	{
		fprintf(stderr, "Queue is empty\n");
		return -INT_MAX;
	}

	--Q->qsize;

	int ret = Q->key[Q->first];

	Q->first = (Q->first + 1) % Q->max_queue_size;
	return ret;
}

int Front(QueuePtr Q)
{
	if (Q->qsize == 0)
	{
		fprintf(stderr, "Queue is empty\n");
		return -INT_MAX;
	}

	return Q->key[Q->first];
}

