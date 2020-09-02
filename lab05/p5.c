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

TreeNodePtr InsertNode(TreeNodePtr root, int value);
TreeNodePtr DeleteNode(TreeNodePtr root, int value);
TreeNodePtr FindNode(TreeNodePtr root, int value);

void PrintInorder(TreeNodePtr root);
void PrintPreorder(TreeNodePtr root);
void PrintPostorder(TreeNodePtr root);

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
            root = InsertNode(root, value);
        }
        else if (strcmp(command, "d") == 0)
        {
            fscanf(fp, "%d", &value);
            root = DeleteNode(root, value);
        }
        else if (strcmp(command, "f") == 0)
        {
            fscanf(fp, "%d", &value);
            if (FindNode(root, value) != NULL)
            {
                fprintf(fpOut, "%d is in the tree.\n", value);
            }
            else
            {
                fprintf(fpOut, "%d is not in the tree.\n", value);
            }
        }
        else if (strcmp(command, "pi") == 0)
        {
            fprintf(fpOut, "pi -");
            PrintInorder(root);
            fprintf(fpOut, "\n");
        }
        else if (strcmp(command, "pr") == 0)
        {
            fprintf(fpOut, "pr -");
            PrintPreorder(root);
            fprintf(fpOut, "\n");
        }
        else if (strcmp(command, "po") == 0)
        {
            fprintf(fpOut, "po -");
            PrintPostorder(root);
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

TreeNodePtr InsertNode(TreeNodePtr root, int value)
{
    if (root == NULL)
    {
        root = (TreeNodePtr)malloc(sizeof(TreeNode));
        assert(root != NULL);

        root->value = value;
        
        root->leftChild = NULL;
        root->rightChild = NULL;
    }
    else if (root->value > value)
    {
        root->leftChild = InsertNode(root->leftChild, value);
    }
    else if (root->value < value)
    {
        root->rightChild = InsertNode(root->rightChild, value);
    }
    else
    {
        fprintf(fpOut, "%d already exists.\n", value);
    }

    return root;
}

TreeNodePtr FindMax(TreeNodePtr node)
{
    if (node == NULL)
        return NULL;

    while (node->rightChild != NULL)
        node = node->rightChild;

    return node;
}

TreeNodePtr DeleteNode(TreeNodePtr root, int value)
{
    if (root == NULL)
    {
        fprintf(fpOut, "Deletion failed. %d does not exist.\n", value);
    }
    else if (root->value > value)
    {
        root->leftChild = DeleteNode(root->leftChild, value);
    }
    else if (root->value < value)
    {
        root->rightChild = DeleteNode(root->rightChild, value);
    }
    else if (root->leftChild != NULL && root->rightChild != NULL)
    {
        root->value = FindMax(root->leftChild)->value;
        root->leftChild = DeleteNode(root->leftChild, root->value);
    }
    else
    {
        TreeNodePtr tempNowNode = root;

        if (root->leftChild == NULL)
            root = root->rightChild;
        else if (root->rightChild == NULL)
            root = root->leftChild;

        free(tempNowNode);
    }

    return root;
}

TreeNodePtr FindNode(TreeNodePtr root, int value)
{
    if (root == NULL)
        return NULL;
    else if (root->value < value)
        return FindNode(root->rightChild, value);
    else if (root->value > value)
        return FindNode(root->leftChild, value);

    return root;
}

void PrintInorder(TreeNodePtr root)
{
    if (root != NULL)
    {
        PrintInorder(root->leftChild);
        fprintf(fpOut, " %d", root->value);
        PrintInorder(root->rightChild);
    }
}

void PrintPreorder(TreeNodePtr root)
{
    if (root != NULL)
    {
        fprintf(fpOut, " %d", root->value);
        PrintPreorder(root->leftChild);
        PrintPreorder(root->rightChild);
    }
}

void PrintPostorder(TreeNodePtr root)
{
    if (root != NULL)
    {
        PrintPostorder(root->leftChild);
        PrintPostorder(root->rightChild);
        fprintf(fpOut, " %d", root->value);
    }
}

