#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY (-1)
#define DEL (0)

#define TBL_ITEM(H, idx) ((H)->TheLists[(idx)]->Element)

FILE* fpOut;

typedef struct ListNode *Position;
typedef Position List;
typedef struct HashTbl *HashTable;

struct ListNode
{
	int Element;
};

struct HashTbl
{
	int TableSize;
	List* TheLists;
};

void CreateHashTable(HashTable H, int size);
void ReleaseHashTable(HashTable H);

void Insert(HashTable H, int value, int solution);
void delete(HashTable H, int value, int solution);
int find(HashTable H, int value, int solution);
void print(HashTable H);
int Hash(int value, int Size, int i, int solution);

void testCase(FILE* fp, int solution)
{
	if (solution == 1) fprintf(fpOut, "Linear\n");
	else if (solution == 2) fprintf(fpOut, "Quadratic\n");
	else if (solution == 3) fprintf(fpOut, "Double\n");

	int size;
	fscanf(fp, "%d", &size);

	struct HashTbl table;
	CreateHashTable(&table, size);

	char buffer[128];
	int value;
	while (1)
	{
		fscanf(fp, "%s", buffer);

		if (strcmp(buffer, "i") == 0)
		{
			fscanf(fp, "%d", &value);
			Insert(&table, value, solution); 
		}
		else if (strcmp(buffer, "d") == 0)
		{
			fscanf(fp, "%d", &value);
			delete(&table, value, solution);
		}
		else if (strcmp(buffer, "f") == 0)
		{
			fscanf(fp, "%d", &value);
			const int idx = find(&table, value, solution);

			if (idx == -1)
			{
				fprintf(fpOut, "Not found\n");
			}
			else
			{
				fprintf(fpOut, "%d\n", idx);
			}
		}
		else if (strcmp(buffer, "p") == 0)
		{
			print(&table);
		}
		else if (strcmp(buffer, "q") == 0)
		{
			break;
		}
		else
		{
			fprintf(fpOut, "<ERROR> Invalid command (%s)\n", buffer);
		}
	}

	ReleaseHashTable(&table);
}

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

	int numOfTestCases;
	fscanf(fp, "%d", &numOfTestCases);

	char buffer[256];
	char solution = 0;
	int value;
	for (int taskCase = 0; taskCase < numOfTestCases; ++taskCase)
	{
		fscanf(fp, "%s", buffer);

		if (strcmp(buffer, "Linear") == 0)
			solution = 1;
		else if (strcmp(buffer, "Quadratic") == 0)
			solution = 2;
		else if (strcmp(buffer, "Double") == 0)
			solution = 3;
		else
		{
			fprintf(fpOut, "<ERROR> Invalid solution(%s)\n", buffer);
			continue;
		}
	
		if (taskCase) fprintf(fpOut, "\n\n");
		testCase(fp, solution);		
	}

	fclose(fp);
	fclose(fpOut);
}

void CreateHashTable(HashTable H, int size)
{
	H->TableSize = size;
	H->TheLists = malloc(sizeof(List) * size);

	for (int i = 0; i < size; ++i)
	{
		H->TheLists[i] = malloc(sizeof(struct ListNode));
		TBL_ITEM(H, i) = EMPTY;
	}
}

void ReleaseHashTable(HashTable H)
{
	for (int i = 0; i < H->TableSize; ++i)
		free(H->TheLists[i]);

	free(H->TheLists);
	H->TableSize = 0;
}

void Insert(HashTable H, int value, int solution)
{
	for (int i = 0; i < H->TableSize; ++i)
	{
		const int idx = Hash(value, H->TableSize, i, solution);

		if (TBL_ITEM(H, idx) == EMPTY || TBL_ITEM(H, idx) == DEL)
		{
			TBL_ITEM(H, idx) = value;

			fprintf(fpOut, "Inserted %d\n", value);
			return;
		}
		else if (TBL_ITEM(H, idx) == value)
		{
			fprintf(fpOut, "Already exists\n");
			return;
		}
	}

	fprintf(fpOut, "Full table\n");
}

void delete(HashTable H, int value, int solution)
{
	for (int i = 0; i < H->TableSize; ++i)
	{
		const int idx = Hash(value, H->TableSize, i, solution);

		if (TBL_ITEM(H, idx) == value)
		{
			TBL_ITEM(H, idx) = DEL;

			fprintf(fpOut, "Deleted %d\n", value);
			return;
		}
	}

	fprintf(fpOut, "%d not exists\n", value);
}

int find(HashTable H, int value, int solution)
{
	for (int i = 0; i < H->TableSize; ++i)
	{
		const int idx = Hash(value, H->TableSize, i, solution);

		if (TBL_ITEM(H, idx) == value)
			return idx;
		else if (TBL_ITEM(H, idx) == DEL)
			continue;
		else
			break;
	}

	return -1;
}

void print(HashTable H)
{
	for (int i = 0; i < H->TableSize; ++i)
	{
		int value = TBL_ITEM(H, i);
		value = (value == EMPTY || value == DEL) ? 0 : value;

		fprintf(fpOut, "%d ", value);
	}
	fprintf(fpOut, "\n");
}

int Hash(int value, int Size, int i, int solution)
{
	switch (solution)
	{
	// Linear
	case 1: return (value%Size + i) % Size;
	case 2: return (value%Size + i*i) % Size;
	case 3: return (value%Size + i * (7 - (value % 7))) % Size;
	}

	return -1;
}

