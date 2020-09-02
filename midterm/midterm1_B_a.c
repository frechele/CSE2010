#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DUP_NODES 100

FILE* fpOut;

typedef struct tagTreeNode
{
    int value;
    int* dup_count;
    struct tagTreeNode** duplicated;

    struct tagTreeNode* leftChild;
    struct tagTreeNode* rightChild;
} TreeNode, *TreeNodePtr;

void ReleaseTree(TreeNodePtr root);

TreeNodePtr Insert(TreeNodePtr root, int value);
TreeNodePtr Delete(TreeNodePtr root, int value);
TreeNodePtr Find(TreeNodePtr root, int value);

void PrintInorder(TreeNodePtr root);

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
    int value;
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
        else if (strcmp(command, "f") == 0)
        {
            fscanf(fp, "%d", &value);
            TreeNodePtr finded;
            if ((finded = Find(root, value)) != NULL)
            {
                fprintf(fpOut, "%d is in the tree. count: %d\n", value, *finded->dup_count + 1);
            }
            else
            {
                fprintf(fpOut, "%d is not in the tree.\n", value);
            }
        }
        else if (strcmp(command, "p") == 0)
        {
            PrintInorder(root);
            fprintf(fpOut, "\n");
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

    for (int i = 0; i < *root->dup_count; ++i)
    {
        free(root->duplicated[i]);
    }
    free(root->duplicated);
    free(root->dup_count);

    ReleaseTree(root->leftChild);
    ReleaseTree(root->rightChild);
    free(root);
}

TreeNodePtr Insert(TreeNodePtr root, int value)
{
    if (root == NULL)
    {
        root = (TreeNodePtr)malloc(sizeof(TreeNode));
        assert(root != NULL);

        root->value = value;
        root->dup_count = (int*)malloc(sizeof(int));
        *root->dup_count = 0;

        root->duplicated = (TreeNodePtr*)malloc(sizeof(TreeNodePtr) * MAX_DUP_NODES);
        for (int i = 0; i < MAX_DUP_NODES; ++i)
            root->duplicated[i] = NULL;
        
        root->leftChild = NULL;
        root->rightChild = NULL;
    }
    else if (root->value > value)
    {
        root->leftChild = Insert(root->leftChild, value);
    }
    else if (root->value < value)
    {
        root->rightChild = Insert(root->rightChild, value);
    }
    else
    {
        TreeNodePtr tempNode = (TreeNodePtr)malloc(sizeof(TreeNode));
        assert(root != NULL);

        tempNode->value = value;
        tempNode->duplicated = root->duplicated;
        tempNode->dup_count = root->dup_count;

        root->duplicated[*root->dup_count] = tempNode;

        ++(*root->dup_count);
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
    }
    else if (root->value < value)
    {
        root->rightChild = Delete(root->rightChild, value);
    }
    else if (*root->dup_count > 0)
    {
        --(*root->dup_count);
        free(root->duplicated[*root->dup_count]);
        root->duplicated[*root->dup_count] = NULL;
    }
    else if (root->leftChild != NULL && root->rightChild != NULL)
    {
        tempNowNode = root->leftChild;
        while (tempNowNode->rightChild != NULL)
            tempNowNode = tempNowNode->rightChild;

        root->value = tempNowNode->value;
        root->leftChild = Delete(root->leftChild, root->value);
    }
    else
    {
        tempNowNode = root;

        if (root->leftChild == NULL)
            root = root->rightChild;
        else if (root->rightChild == NULL)
            root = root->leftChild;

        free(tempNowNode->dup_count);
        free(tempNowNode->duplicated);
        free(tempNowNode);
    }

    return root;
}

TreeNodePtr Find(TreeNodePtr root, int value)
{
    if (root == NULL)
        return NULL;
    else if (root->value < value)
        return Find(root->rightChild, value);
    else if (root->value > value)
        return Find(root->leftChild, value);

    if (*root->dup_count > 0)
        root = root->duplicated[*root->dup_count - 1];

    return root;
}

void PrintInorder(TreeNodePtr root)
{
    if (root != NULL)
    {
        PrintInorder(root->leftChild);
        fprintf(fpOut, "%d(%d) ", root->value, *root->dup_count + 1);
        PrintInorder(root->rightChild);
    }
}

