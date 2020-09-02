#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ** operator ** // 
typedef enum
{
    LP,
    LR,

    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    INVALID
} OpType;
const int OpPrecedence[] = { 0, 0, 1, 1, 2, 2, 2, -999 };
const char OpStr[] = { '(', ')', '+', '-', '*', '/', '%', '?' };

OpType CharToOperator(char op);
void InfixToPostfix(const char* infix, char* postfix, int size);
int EvaluatePostfix(const char* postfix, int size);

// ** stack implementation ** //
#define EMPTY_TOS (-1)

typedef struct
{
    int capacity;
    int topOfStack;

    int* data;
} Stack;

void CreateStack(Stack* pStack, int capacity);
void ReleaseStack(Stack* pStack);

int PushStack(Stack* pStack, int value);
int PopStack(Stack* pStack);
int TopStack(Stack* pStack);

int IsFullStack(Stack* pStack);
int IsEmptyStack(Stack* pStack);

// ** program entry point **//
int main()
{
    FILE* fpOut = fopen("output.txt", "wt");
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

    char infix[101], postfix[101];
    fscanf(fp, "%[^#]s", infix);
    InfixToPostfix(infix, postfix, strlen(infix));

    fprintf(fpOut, "Infix Form : %s\n", infix);
    fprintf(fpOut, "Postfix Form : %s\n", postfix);
    fprintf(fpOut, "Evaluation Result : %d\n",
        EvaluatePostfix(postfix, strlen(postfix)));

    fclose(fp);
    fclose(fpOut);
}

// ** operator ** //
void InfixToPostfix(const char* infix, char* postfix, int size)
{
    Stack stack;
    CreateStack(&stack, 100);

    int postCur = 0, ch;
    for (int i = 0; i < size; ++i)
    {
        ch = infix[i];

        if (isdigit(ch))
        {
            postfix[postCur++] = ch;
        }
        else if (ch == '+' || ch == '-' || ch == '*' ||
                 ch == '/' || ch == '%')
        {
            while (OpPrecedence[CharToOperator(ch)] 
                    <= OpPrecedence[CharToOperator(TopStack(&stack))])
                postfix[postCur++] = OpStr[CharToOperator(PopStack(&stack))];

            PushStack(&stack, ch);
        }
        else if (ch == '(')
        {
            PushStack(&stack, '(');
        }
        else if (ch == ')')
        {
            while (TopStack(&stack) != '(')
            {
                postfix[postCur++] = OpStr[CharToOperator(PopStack(&stack))];
            }
            PopStack(&stack); // pop left parenthesis
        }
    }

    // pop all operators
    while (!IsEmptyStack(&stack))
    {
        postfix[postCur++] = OpStr[CharToOperator(PopStack(&stack))];
    }
    postfix[postCur++] = '\0';

    ReleaseStack(&stack);
}

int EvaluatePostfix(const char* postfix, int size)
{
    Stack stack;
    CreateStack(&stack, 100);

    int op1, op2;
    char ch;

    for (int i = 0; i < size; ++i)
    {
        ch = postfix[i];

        if (isdigit(ch))
        {
            PushStack(&stack, ch - '0');
        }
        else
        {
            op2 = PopStack(&stack);
            op1 = PopStack(&stack);

            if (ch == '+')
                PushStack(&stack, op1 + op2);
            else if (ch == '-')
                PushStack(&stack, op1 - op2);
            else if (ch == '*')
                PushStack(&stack, op1 * op2);
            else if (ch == '/')
                PushStack(&stack, op1 / op2);
            else if (ch == '%')
                PushStack(&stack, op1 % op2);
        }
    }

    int result = PopStack(&stack);
    ReleaseStack(&stack);

    return result;
}

OpType CharToOperator(char op)
{
    switch (op)
    {
        case '+': return ADD;
        case '-': return SUB;
        case '*': return MUL;
        case '/': return DIV;
        case '%': return MOD;
        default: return INVALID;
    }
}

// ** stack implementation **//
void CreateStack(Stack* pStack, int capacity)
{
    pStack->capacity = capacity;
    pStack->topOfStack = EMPTY_TOS;
    pStack->data = (int*)malloc(capacity * sizeof(int));
}

void ReleaseStack(Stack* pStack)
{
    free(pStack->data);
    pStack->data = NULL;
    
    pStack->topOfStack = EMPTY_TOS;
    pStack->capacity = 0;
}

int PushStack(Stack* pStack, int value)
{
    if (IsFullStack(pStack))
    {
        return 0;
    }

    pStack->data[++pStack->topOfStack] = value;
    return 1;
}

int PopStack(Stack* pStack)
{
    if (IsEmptyStack(pStack))
    {
        return INVALID;
    }

    return pStack->data[pStack->topOfStack--];
}

int TopStack(Stack* pStack)
{
    if (IsEmptyStack(pStack))
    {
        return INVALID;
    }

    return pStack->data[pStack->topOfStack];
}

int IsFullStack(Stack* pStack)
{
    return (pStack->topOfStack == pStack->capacity - 1);
}

int IsEmptyStack(Stack* pStack)
{
    return (pStack->topOfStack == EMPTY_TOS);
}

