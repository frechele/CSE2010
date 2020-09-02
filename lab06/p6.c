#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

FILE* fpOut;

typedef struct tagAVLNode
{
    int value;

    struct tagAVLNode* leftChild;
    struct tagAVLNode* rightChild;
    int height;
} AVLNode, *AVLNodePtr;

void ReleaseTree(AVLNodePtr root);

int Height(AVLNodePtr node);
AVLNodePtr Insert(AVLNodePtr node, int value, short* updated);
AVLNodePtr SingleRotateWithLeft(AVLNodePtr k2);
AVLNodePtr SingleRotateWithRight(AVLNodePtr k2);
AVLNodePtr DoubleRotateWithLeft(AVLNodePtr k3);
AVLNodePtr DoubleRotateWithRight(AVLNodePtr k3);

void InorderTraversal(AVLNodePtr root);

int max(int a, int b);

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

    AVLNodePtr tree = NULL;

    short updated;
    int value;
    while (fscanf(fp, "%d", &value) != EOF)
    {
        tree = Insert(tree, value, &updated); 

        if (updated)
        {
            InorderTraversal(tree);
            fprintf(fpOut, "\n");
        }
    }

    ReleaseTree(tree);

    fclose(fp);
    fclose(fpOut);
}

void ReleaseTree(AVLNodePtr root)
{
    if (root != NULL)
    {
        ReleaseTree(root->leftChild);
        ReleaseTree(root->rightChild);
        free(root);
    }   
}

int Height(AVLNodePtr node)
{
    return node ? node->height : -1;
}

AVLNodePtr Insert(AVLNodePtr node, int value, short* updated)
{
    if (node == NULL)
    {
        node = (AVLNodePtr)malloc(sizeof(AVLNode));
        assert(node != NULL);

        node->value = value;
        node->height = 0;

        node->leftChild = NULL;
        node->rightChild = NULL;

        *updated = 1;
    }
    else if (node->value > value)
    {
        node->leftChild = Insert(node->leftChild, value, updated);

        if (Height(node->leftChild) - Height(node->rightChild) == 2)
        {
            if (node->leftChild->value > value)
                node = SingleRotateWithLeft(node);
            else
                node = DoubleRotateWithLeft(node);
        }
    }
    else if (node->value < value)
    {
        node->rightChild = Insert(node->rightChild, value, updated);

        if (Height(node->rightChild) - Height(node->leftChild) == 2)
        {
            if (node->rightChild->value < value)
                node = SingleRotateWithRight(node);
            else
                node = DoubleRotateWithRight(node);
        }
    }
    else
    {
        fprintf(fpOut, "%d already in the tree!\n", value);
        *updated = 0;
    }

    node->height = max(Height(node->leftChild), Height(node->rightChild)) + 1;

    return node;
}

AVLNodePtr SingleRotateWithLeft(AVLNodePtr k2)
{
    AVLNodePtr k1;

    k1 = k2->leftChild;
    k2->leftChild = k1->rightChild;
    k1->rightChild = k2;

    k2->height = max(Height(k2->leftChild), Height(k2->rightChild)) + 1;
    k1->height = max(Height(k1->leftChild), Height(k2)) + 1;

    return k1;
}

AVLNodePtr SingleRotateWithRight(AVLNodePtr k2)
{
    AVLNodePtr k1;

    k1 = k2->rightChild;
    k2->rightChild = k1->leftChild;
    k1->leftChild = k2;

    k2->height = max(Height(k2->leftChild), Height(k2->rightChild)) + 1;
    k1->height = max(Height(k1->rightChild), Height(k2)) + 1;

    return k1;
}

AVLNodePtr DoubleRotateWithLeft(AVLNodePtr k3)
{
    k3->leftChild = SingleRotateWithRight(k3->leftChild);

    return SingleRotateWithLeft(k3);
}

AVLNodePtr DoubleRotateWithRight(AVLNodePtr k3)
{
    k3->rightChild = SingleRotateWithLeft(k3->rightChild);

    return SingleRotateWithRight(k3);
}

void InorderTraversal(AVLNodePtr root)
{
    if (root != NULL)
    {
        InorderTraversal(root->leftChild);
        fprintf(fpOut, "%d(%d) ", root->value, Height(root));
        InorderTraversal(root->rightChild);
    }
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

