#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* fpOut;

#define order 3
typedef struct B_node {
    int n_keys;
    struct B_node* child[order];
    int key[order - 1];
} B_node, * B_node_ptr;

B_node_ptr tree;

void ReleaseBTree(B_node_ptr node);

void Insert(int key);
void Inorder(B_node_ptr node);

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

    char command[128];
    int value;
    while (fscanf(fp, "%s", command) != EOF)
    {
        if (strcmp(command, "i") == 0)
        {
            fscanf(fp, "%d", &value);
            Insert(value);
        }
        else if (strcmp(command, "p") == 0)
        {
            Inorder(tree);
            fprintf(fpOut, "\n");
        }
    }

    ReleaseBTree(tree);

    fclose(fp);
    fclose(fpOut);
}

char IsLeafNode(B_node_ptr node)
{
    for (int i = 0; i < order; ++i)
        if (node->child[i] != NULL)
            return 0;

    return 1;
}

B_node_ptr createNode()
{
    B_node_ptr node = malloc(sizeof(B_node));
    node->n_keys = 0;

    for (int i = 0; i < order; ++i)
        node->child[i] = NULL;

    return node;
}

void ReleaseBTree(B_node_ptr node)
{
    if (node != NULL)
    {
        if (!IsLeafNode(node))
        {
            for (int i = 0; i < order; ++i)
            {
                ReleaseBTree(node->child[i]);
            }
        }

        free(node);
    }
}

B_node_ptr insertImpl(B_node_ptr node, int key, int* median)
{
    int insertIndex = 0;
    while (insertIndex < node->n_keys && node->key[insertIndex] < key)
        ++insertIndex;

    const int mid = (order - 1) / 2;
	
    char needSplit = 0;
    char leafPos = 0;
    B_node_ptr temp = NULL;

	if (IsLeafNode(node))
	{
		if (node->n_keys == order - 1) // is full
		{
            needSplit = 1;

			if (insertIndex > mid) // 1 2 [*]
			{
                *median = node->key[mid];

                for (int i = mid; i < insertIndex - 1; ++i)
                    node->key[i] = node->key[i + 1];
                node->key[insertIndex - 1] = key;
			}
            else if (insertIndex < mid) // [*] 1 2
            {
                *median = node->key[mid - 1];

                for (int i = insertIndex + 1; i < mid; ++i)
                    node->key[i] = node->key[i - 1];
                node->key[insertIndex] = key;
            }
            else // 1 [*] 2
            {
                *median = key;
            }
		}
        else
        {
            for (int i = insertIndex; i < node->n_keys; ++i)
                node->key[i + 1] = node->key[i];
            node->key[insertIndex] = key;
            ++node->n_keys;
        }
	}
    else
    {
        int midValue;
        temp = insertImpl(node->child[insertIndex], key, &midValue);

    	if (temp != NULL)
    	{
    		if (node->n_keys == order - 1) // is full
    		{
                int midIndex = 0;
                while (midIndex < node->n_keys && node->key[midIndex] < midValue)
                    ++midIndex;

                needSplit = 1;

    			if (midIndex > mid)
    			{
                    *median = node->key[mid];

                    for (int i = mid; i < midIndex - 1; ++i)
                        node->key[i] = node->key[i + 1];
                    node->key[midIndex - 1] = midValue;

                    leafPos = 1;
    			}
                else if (midIndex < mid)
                {
                    *median = node->key[mid - 1];

                    for (int i = midIndex + 1; i < mid; ++i)
                        node->key[i] = node->key[i - 1];
                    node->key[midIndex] = midValue;

                    leafPos = 2;
                }
                else
                {
                    *median = midValue;

                    leafPos = 3;
                }
    		}
            else
            {
                for (int i = insertIndex; i < node->n_keys; ++i)
                    node->key[i + 1] = node->key[i];
                node->key[insertIndex] = midValue;

                for (int i = insertIndex; i < node->n_keys; ++i)
                    node->child[i + 2] = node->child[i + 1];
                node->child[insertIndex + 1] = temp;

                ++node->n_keys;
            }
    	}
    }

    if (needSplit)
    {
        B_node_ptr rightChild = createNode();

        rightChild->n_keys = node->n_keys - mid;

        for (int i = 0; i < rightChild->n_keys; ++i)
            rightChild->key[i] = node->key[mid + i];

        if (!IsLeafNode(node))
        {
        	if (leafPos == 1)
        	{
                for (int i = 0; i < rightChild->n_keys; ++i)
                {
                    rightChild->child[i] = node->child[mid + i + 1];
                    node->child[mid + i + 1] = NULL;
                }
                rightChild->child[rightChild->n_keys] = temp;
        	}
            else if (leafPos == 2)
            {
	            for (int i = 0; i < rightChild->n_keys + 1; ++i)
	            {
                    rightChild->child[i] = node->child[mid + i];
                    node->child[mid + i] = NULL;
	            }
                node->child[mid] = temp;
            }
            else if (leafPos == 3)
            {
                rightChild->child[0] = temp;
	            for (int i = 1; i <= rightChild->n_keys; ++i)
	            {
                    rightChild->child[i] = node->child[mid + i];
                    node->child[mid + i] = NULL;
	            }
            }
        }

        node->n_keys = mid;

        return rightChild;
    }
	
    return NULL;
}

void Insert(int key)
{
	if (tree == NULL)
	{
        tree = createNode();

        tree->key[0] = key;
        tree->n_keys = 1;
	}
    else
    {
        int median;
        B_node_ptr rightChild = insertImpl(tree, key, &median);

        if (rightChild)
        {
            B_node_ptr leftChild = tree;

            tree = createNode();

            tree->key[0] = median;
            tree->n_keys = 1;
        	
            tree->child[0] = leftChild;
            tree->child[1] = rightChild;
        }
    }
}

void Inorder(B_node_ptr node)
{
    if (node != NULL)
    {
        const char leaf = IsLeafNode(node);

        int i;
        for (i = 0; i < node->n_keys; ++i)
        {
            if (!leaf)
                Inorder(node->child[i]);

            fprintf(fpOut, "%d ", node->key[i]);
        }

        if (!leaf)
            Inorder(node->child[i]);
    }
}
