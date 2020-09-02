#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LENGTH_OF_FIRST_NAME 30
#define LENGTH_OF_LAST_NAME  30

FILE* fpOut;

typedef struct
{
    int  studentID;
    char studentName[LENGTH_OF_FIRST_NAME + LENGTH_OF_LAST_NAME + 1];
} ElementType;

typedef struct tagNode
{
    ElementType element;

    struct tagNode* next;

} Node, *NodePtr, *List;

void CreateList(List* list);
void ReleaseList(List* list);

void Insert(List list, ElementType element);
void Delete(List list, int studentID);
void DeleteNode(List list, NodePtr node);
NodePtr Find(List list, int studentID);
void PrintList(List list);
void PrintCurrentList(List list);

int main()
{
    FILE* fp = fopen("input.txt", "rt");
    if (!fp)
    {
        printf("Cannot open input file\n");
        return 1;
    }

    fpOut = fopen("output.txt", "wt");
    if (!fpOut)
    {
        fclose(fp);

        printf("Cannot open output file\n");
        return 1;
    }

    List list;
    CreateList(&list);

    while (!feof(fp))
    {
        char command;
        fscanf(fp, "%c ", &command);

        if (command == 'i')
        {
            ElementType elem;

            char firstName[LENGTH_OF_FIRST_NAME], lastName[LENGTH_OF_LAST_NAME];
            fscanf(fp, "%d %s %s\n", &elem.studentID, firstName, lastName);
            sprintf(elem.studentName, "%s %s", firstName, lastName);

            Insert(list, elem);
        }
        else if (command == 'd')
        {
            int studentID;
            fscanf(fp, "%d\n", &studentID);

            Delete(list, studentID);
        }
        else if (command == 'f')
        {
            int studentID;
            fscanf(fp, "%d\n", &studentID);

            NodePtr findNode = Find(list, studentID);
            if (findNode != NULL)
            {
                fprintf(fpOut, "Find Success : %d %s\n", findNode->element.studentID, findNode->element.studentName);
            }
            else
            {
                fprintf(fpOut, "Find %d Failed. There is no student ID\n", studentID);
            }
        }
        else if (command == 'p')
        {
            PrintList(list);
        }
        else
        {
            fprintf(fpOut, "Wrong command.\n");
        }
    }

    ReleaseList(&list);

    fclose(fp);
    fclose(fpOut);
}

void CreateList(List* list)
{
    *list = (List)malloc(sizeof(Node));

    (*list)->element.studentID = -1;

    (*list)->next = NULL;
}

void ReleaseList(List* list)
{
    NodePtr tempNowNode = *list;
    while (tempNowNode->next != NULL)
    {
        tempNowNode = tempNowNode->next;

        DeleteNode(*list, tempNowNode);
    }

    free(*list);
    *list = NULL;
}

void Insert(List list, ElementType element)
{
    NodePtr prevNode = list;

    while (prevNode->next != NULL && prevNode->next->element.studentID < element.studentID) prevNode = prevNode->next;

    if (prevNode->next != NULL && prevNode->next->element.studentID == element.studentID)
    {
        fprintf(fpOut, "Insertion Failed. : ID %d already exists.\n", element.studentID);
        return;
    }

    NodePtr newNode = (NodePtr)malloc(sizeof(Node));
    newNode->element.studentID = element.studentID;
    strcpy(newNode->element.studentName, element.studentName);

    newNode->next = prevNode->next;
    prevNode->next = newNode;

    fprintf(fpOut, "Insertion Success : %d\n", element.studentID);

    PrintCurrentList(list);    
}

void Delete(List list, int studentID)
{
    NodePtr nodeToRemove = Find(list, studentID);

    if (nodeToRemove == NULL)
    {
        fprintf(fpOut, "Deletion Failed : Student ID %d is not in the list.\n", studentID);
        return;
    }

    DeleteNode(list, nodeToRemove);

    fprintf(fpOut, "Deletion Success : %d\n", studentID);
    PrintCurrentList(list);
}

void DeleteNode(List list, NodePtr node)
{
    NodePtr prevNode = list;
    while (prevNode->next != node) prevNode = prevNode->next;

    prevNode->next = node->next;

    free(node);
}

NodePtr Find(List list, int studentID)
{
    NodePtr tempNowNode = list->next;

    while (tempNowNode != NULL)
    {
        if (tempNowNode->element.studentID == studentID)
        {
            return tempNowNode;
        }
        tempNowNode = tempNowNode->next;
    }

    return NULL;
}

void PrintList(List list)
{
    NodePtr tempNowNode = list->next;

    fprintf(fpOut, "-----LIST-----\n");
    
    while (tempNowNode != NULL)
    {
        fprintf(fpOut, "%d %s\n",
                 tempNowNode->element.studentID, tempNowNode->element.studentName);
        tempNowNode = tempNowNode->next;
    }

    fprintf(fpOut, "--------------\n");
}

void PrintCurrentList(List list)
{
    fprintf(fpOut, "Current List > ");

    NodePtr tempNowNode = list->next;
    int idx = 0;
    while (tempNowNode != NULL)
    {
        if (idx) fprintf(fpOut, "-");

        fprintf(fpOut, "%d %s", tempNowNode->element.studentID, tempNowNode->element.studentName);

        tempNowNode = tempNowNode->next;
        ++idx;
    }

    fprintf(fpOut, "\n");

}

