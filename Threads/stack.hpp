

typedef struct NODE_{
    char *data;
    struct NODE_ *next;

} node, *pnode;

void push(pnode *head, char *d);

char* pop(pnode *head);

const char* top(pnode head);

void print(pnode head);

void clear(pnode *head);