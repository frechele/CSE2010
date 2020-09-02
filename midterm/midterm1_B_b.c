#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* fpOut;

typedef struct tagTreeNode
{
    int value;

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
            if (Find(root, value) != NULL)
            {
                fprintf(fpOut, "%d is in the tree.\n", value);
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
        
        root->leftChild = NULL;
        root->rightChild = NULL;
    }
    else if (root->value >= value)
    {
        root->leftChild = Insert(root->leftChild, value);
    }
    else if (root->value < value)
    {
        root->rightChild = Insert(root->rightChild, value);
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

    return root;
}

void PrintInorder(TreeNodePtr root)
{
    if (root != NULL)
    {
        PrintInorder(root->leftChild);
        fprintf(fpOut, "%d ", root->value);
        PrintInorder(root->rightChild);
    }
}

