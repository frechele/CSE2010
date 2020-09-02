#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF (99999)

FILE* fpOut;

typedef struct _Node
{
	int key;
	int weight;
} Node, *NodePtr;

typedef struct _HeapNode
{
	int key;
	int* dist;
} HeapNode, *HeapNodePtr;

typedef struct _Heap
{
	int capacity;
	int size;
	HeapNodePtr elements;
} Heap, *HeapPtr;

typedef struct _Graph
{
	int size;
	int* node;
	int** matrix;
} Graph, *GraphPtr;

Graph CreateGraph(int* nodes, int size);
void DeleteGraph(GraphPtr G);
int InsertEdge(GraphPtr G, int a, int b, int w);
int findNode(GraphPtr G, int node);
int* DijkstraSearch(GraphPtr G, int start);

HeapPtr createMinHeap(int heapSize);
void deleteMinHeap(HeapPtr minHeap);
void insert(HeapPtr minHeap, int vertex, int* distance);
int deleteMin(HeapPtr minHeap);
void printShortestPath(GraphPtr G, int* pred, int start, int end);

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

	char buffer[8192] = { 0 };
	int value1, value2, value3;

	// Parsing first line (nodes)
	fgets(buffer, 8192, fp);
	
	int nodes[100];
	int numOfNodes = 0;
	
	char* ptr = buffer;
	int numOfReads;
	while (sscanf(ptr, "%d%n", &value1, &numOfReads) == 1)
	{
		ptr += numOfReads;
		nodes[numOfNodes++] = value1;
	}

	Graph graph = CreateGraph(nodes, numOfNodes);

	// Parsing second line (weights) 
	fgets(buffer, 8192, fp);

	ptr = buffer;
	while (sscanf(ptr, "%d-%d-%d%n", &value1, &value2, &value3, &numOfReads) == 3)
	{
		ptr += numOfReads;
		if (!InsertEdge(&graph, value1, value2, value3))
		{
			printf("INVALID EDGE\n");
			exit(EXIT_FAILURE);
		}
	}

	// Parsing third line (start, end)
	fscanf(fp, " %d %d", &value1, &value2);

	int* pred = DijkstraSearch(&graph, value1);

	printShortestPath(&graph, pred, value1, value2); 

	free(pred);

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
			graph.matrix[i][j] = INF;
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

int InsertEdge(GraphPtr G, int a, int b, int w)
{
	const int x = findNode(G, a);
	const int y = findNode(G, b);

	if (x == -1 || y == -1)
		return 0;
	
	G->matrix[y][x] = w;
	return 1;
}

int findNode(GraphPtr G, int node)
{
	for (int i = 0; i < G->size; ++i)
	{
		if (G->node[i] == node)
			return i;
	}

	return -1;
}

void propagate(HeapPtr minHeap, int idx)
{
	int child;

	for (; idx * 2 <= minHeap->size; idx = child)
	{
		child = idx * 2;
		if (child < minHeap->size &&
				*minHeap->elements[child + 1].dist < *minHeap->elements[child].dist)
			++child;

		if (*minHeap->elements[idx].dist <= *minHeap->elements[child].dist)
			break;

		HeapNode tmp = minHeap->elements[idx];
		minHeap->elements[idx] = minHeap->elements[child];
		minHeap->elements[child] = tmp;
	}
}

int* DijkstraSearch(GraphPtr G, int start)
{
	start = findNode(G, start);
	if (start == -1)
	{
		printf("INVALID STARTING POINT\n");
		exit(EXIT_FAILURE);
	}

	HeapPtr heap = createMinHeap(G->size);

	int** w = G->matrix;

	int* pred = malloc(sizeof(int) * G->size);
	int* d    = malloc(sizeof(int) * G->size);

	for (int i = 0; i < G->size; ++i)
	{
		if (w[i][start] == INF)
		{
			pred[i] = -1;
		}
		else
		{
			pred[i] = start;
		}

		d[i] = w[i][start];
		insert(heap, i, &d[i]);
	}

	HeapNode current;
	while (heap->size > 0)
	{
		current = heap->elements[1];
		deleteMin(heap);

		for (int i = 0; i < G->size; ++i)
		{
			int weight = w[i][current.key];
			if (weight != INF)
			{
				int newDist = *current.dist + weight;
				if (newDist < d[i])
				{
					pred[i] = current.key;
					d[i] = newDist;

					int pos;
					for (pos = 1; pos <= heap->size; ++pos)
						if (heap->elements[pos].key == i)
							break;

				    propagate(heap, pos);
				}
			}
		}
	}

	free(d);
	deleteMinHeap(heap);

	return pred;
}

HeapPtr createMinHeap(int heapSize)
{
	HeapPtr heap = malloc(sizeof(Heap));

	heap->capacity = heapSize;
	heap->size = 0;
	heap->elements = malloc(sizeof(HeapNode) * (1 + heapSize));

	return heap;
}

void deleteMinHeap(HeapPtr minHeap)
{
	free(minHeap->elements);
	free(minHeap);
}

void insert(HeapPtr minHeap, int vertex, int* distance)
{
	// Maybe heap not fulled

	int i;
	for (i = ++minHeap->size;
		i != 1 && *minHeap->elements[i/2].dist > *distance;
		i /= 2)
		minHeap->elements[i] = minHeap->elements[i/2];

	minHeap->elements[i].key = vertex;
	minHeap->elements[i].dist = distance;
}

int deleteMin(HeapPtr minHeap)
{
	// Maybe heap not empty
	
	int rootDist = *minHeap->elements[1].dist;

	minHeap->elements[1] = minHeap->elements[minHeap->size--];

	propagate(minHeap, 1);

	return rootDist;
}

void printShortestPath(GraphPtr G, int* pred, int start, int end)
{
	start = findNode(G, start);
	end = findNode(G, end);

	int idx = 0;
	int* succ = malloc(sizeof(int) * G->size);
	
	succ[idx++] = end;
	while (end != start && idx < G->size)
	{
		if (pred[end] == -1)
		{
			fprintf(fpOut, "no path");
			free(succ);
			return;
		}

		succ[idx++] = pred[end];
		end = pred[end];
	}

	for (int i = idx - 1; i >= 0; --i)
	{
		fprintf(fpOut, "%d ", G->node[succ[i]]);
	}
	
	free(succ);
}

