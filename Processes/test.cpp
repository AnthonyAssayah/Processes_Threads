#include <string.h>
#include "stack.hpp"
#include <iostream>
#include <stack>
using namespace std;
#define WORD_SIZE 1024
#define STACK_MAX_SIZE 1024*1024*80

int main()
{
    char *my_stk = (char*)malloc(STACK_MAX_SIZE);
    int *my_top = (int*)malloc(sizeof(int));
    *my_top = 0;
    stack<char*> stk;

    stk.push("Hey");
    push(my_stk,"Hey",my_top);
    // printf("%s\n",my_stk);
    // pop(my_stk,my_top);
    // printf("%s",my_stk);

    stk.push("one");
    push(my_stk,"one",my_top);

    stk.push("Two");
    push(my_stk,"Two",my_top);

    stk.push("Three");
    push(my_stk,"Three",my_top);

    stk.push("four");
    push(my_stk,"four",my_top);

    //Test both my_top, PUSH and POP at the same time
    while(!stk.empty() && *my_top>0)
    {
        int flag = stk.top() == top(my_stk,my_top) ? 1 : 0;
        if(flag)
        {
            printf("top() TEST FAILED cpp_stack[%s] != our_stack[%s]\n",stk.top(),top(my_stk,my_top));
        }
        else
        {
            printf("top() TEST SUCCSESS cpp_stack[%s] == our_stack[%s]\n",stk.top(),top(my_stk,my_top));
        }
        pop(my_stk,my_top);
        stk.pop();
    }
    //now Stack is empty and we will check if we can still use it when head is NULL;

    stk.push("zero");
    push(my_stk,"zero",my_top);

    stk.push("one");
    push(my_stk,"one",my_top);

    stk.push("Two");
    push(my_stk,"Two",my_top);


    while(!stk.empty() && *my_top>0)
    {
        int flag = stk.top() == top(my_stk,my_top) ? 1 : 0;
        if(flag)
        {
            printf("my_top() TEST FAILED cpp_stack[%s] != our_stack[%s]\n",stk.top(),top(my_stk,my_top));
        }
        else
        {
            printf("my_top() TEST SUCCSESS cpp_stack[%s] == our_stack[%s]\n",stk.top(),top(my_stk,my_top));
        }
        pop(my_stk,my_top);
        stk.pop();
    }



    return 0;
}
