#include <string.h>
#include "stack.hpp"
#include <iostream>
#include <stack>
using namespace std;

int main()
{
    pnode head = NULL;
    stack<char*> stk;

    stk.push("Hey");
    push(&head,"Hey");

    stk.push("one");
    push(&head,"one");

    stk.push("Two");
    push(&head,"Two");

    stk.push("Three");
    push(&head,"Three");

    stk.push("four");
    push(&head,"four");
    //Test both TOP, PUSH and POP at the same time
    while(!stk.empty()&& head != NULL)
    {
        int flag = stk.top()== top(head) ? 1 : 0;
        if(flag)
        {
            printf("top() TEST FAILED cpp_stack[%s] != our_stack[%s]\n",stk.top(),top(head));
        }
        else
        {
            printf("top() TEST SUCCSESS cpp_stack[%s] == our_stack[%s]\n",stk.top(),top(head));
        }
        pop(&head);
        stk.pop();
    }
    //now Stack is empty and we will check if we can still use it when head is NULL;

    stk.push("zero");
    push(&head,"zero");

    stk.push("one");
    push(&head,"one");

    stk.push("Two");
    push(&head,"Two");


    while(!stk.empty()&& head != NULL)
    {
        int flag = stk.top()== top(head) ? 1 : 0;
        if(flag)
        {
            printf("top() TEST FAILED cpp_stack[%s] != our_stack[%s]\n",stk.top(),top(head));
        }
        else
        {
            printf("top() TEST SUCCSESS cpp_stack[%s] == our_stack[%s]\n",stk.top(),top(head));
        }
        pop(&head);
        stk.pop();
    }



    return 0;
}
