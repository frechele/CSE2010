#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* fpOut;

typedef struct tagTreeNode
{
    int value;
    int size;

    struct tagTreeNode* leftChild;
    struct tagTreeNode* rightChild;
} TreeNode, *TreeNodePtr;

void ReleaseTree(TreeNodePtr root);

TreeNodePtr Insert(TreeNodePtr root, int value);
TreeNodePtr Delete(TreeNodePtr root, int value);

int RangeSearch(TreeNodePtr node, int k1, int k2);

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

    TreeNodePtr root = NULL;

    char command[128];
    int value, value2;
    while (fscanf(fp, "%s", command) != EOF)
    {
        if (strcmp(command, "i") == 0)
        {
            fscanf(fp, "%d", &value);
            root = Insert(root, value);
        }
        else if (strcmp(command, "d") == 0)
        {
            fscanf(fp, "%d", &value);
            root = Delete(root, value);
        }
        else if (strcmp(command, "s") == 0)
        {
            fscanf(fp, "%d %d", &value, &value2);
            fprintf(fpOut, "between %d and %d: key value count = %d\n", value, value2, RangeSearch(root, value, value2));        
        }
        else
        {
            fprintf(fpOut, "ERROR: Wrong command\n");
        }
    }
    
    ReleaseTree(root);

    fclose(fp);
    fclose(fpOut);
}

void ReleaseTree(TreeNodePtr root)
{
    if (root == NULL)
        return;

    ReleaseTree(root->leftChild);
    ReleaseTree(root->rightChild);
    free(root);
}

int Size(TreeNodePtr node)
{
    return (node == NULL) ? 0 : node->size;
}

TreeNodePtr Insert(TreeNodePtr root, int value)
{
    if (root == NULL)
    {
        root = (TreeNodePtr)malloc(sizeof(TreeNode));
        assert(root != NULL);

        root->value = value;
        root->size = 1;
        
        root->leftChild = NULL;
        root->rightChild = NULL;
    }
    else if (root->value > value)
    {
        root->leftChild = Insert(root->leftChild, value);

        root->size = 1 + Size(root->leftChild) + Size(root->rightChild);
    }
    else if (root->value < value)
    {
        root->rightChild = Insert(root->rightChild, value);

        root->size = 1 + Size(root->leftChild) + Size(root->rightChild);
    }
    else
    {
        fprintf(fpOut, "%d already exists.\n", value);
    }

    return root;
}

TreeNodePtr Delete(TreeNodePtr root, int value)
{
    TreeNodePtr tempNowNode;

    if (root == NULL)
    {
        fprintf(fpOut, "Deletion failed. %d does not exist.\n", value);
    }
    else if (root->value > value)
    {
        root->leftChild = Delete(root->leftChild, value);

        root->size = 1 + Size(root->leftChild) + Size(root->rightChild);
    }
    else if (root->value < value)
    {
        root->rightChild = Delete(root->rightChild, value);
    
        root->size = 1 + Size(root->leftChild) + Size(root->rightChild);
    }
    else if (root->leftChild != NULL && root->rightChild != NULL)
    {
        tempNowNode = root->leftChild;
        while (tempNowNode->rightChild != NULL)
            tempNowNode = tempNowNode->rightChild;

        root->value = tempNowNode->value;
        root->leftChild = Delete(root->leftChild, root->value);

        root->size = 1 + Size(root->leftChild) + Size(root->rightChild);
    }
    else
    {
        tempNowNode = root;

        if (root->leftChild == NULL)
            root = root->rightChild;
        else if (root->rightChild == NULL)
            root = root->leftChild;

        free(tempNowNode);
    }

    return root;
}

int FindLowerBoundRank(TreeNodePtr root, int value)
{
    if (root == NULL)
        return 0;
    
    int rank = 0;
    int result = -1;
    while (root)
    {
        if (root->value > value)
        {
            result = rank + Size(root->leftChild);
            root = root->leftChild;
        }
        else if (root->value < value)
        {
            rank += 1 + Size(root->leftChild);
            root = root->rightChild;
        }
        else
        {
            return rank + Size(root->leftChild);
        }
    }

    return result;
}

int FindUpperBoundRank(TreeNodePtr root, int value)
{
    if (root == NULL)
        return 0;
    
    int rank = 0;
    int result = -1;
    while (root)
    {
        if (root->value > value)
        {
            root = root->leftChild;
        }
        else if (root->value < value)
        {
            rank += 1 + Size(root->leftChild);
            result = rank - 1;
            root = root->rightChild;
        }
        else
        {
            return rank + Size(root->leftChild);
        }
    }

    return result;
}

int RangeSearch(TreeNodePtr node, int k1, int k2)
{
    if (node == NULL)
        return 0;

    int rankOfLowerBound = FindLowerBoundRank(node, k1);
    int rankOfUpperBound = FindUpperBoundRank(node, k2);

    if (rankOfLowerBound < 0 || rankOfUpperBound < 0 || rankOfLowerBound > rankOfUpperBound)
        return 0;    

    return rankOfUpperBound - rankOfLowerBound + 1;
}

