/*
   Algorithm C from:
   "A Simple Yet Efficient Algorithm for Reconstructing Binary Trees from Inorder-Preorder Sequences"

   label comparison | end comparison | Total
         2n-4       |       n-1      |  3n-5
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>             /* strlen */
#include <unistd.h>		/* getopt */
#include <stdbool.h>
#include <time.h>  // time計測のためのライブラリ

#define Label int
#define X     n+1		/* virtual label */
#define MAX_LENGTH 128          /* max. length of input i-p seq. */

int spI = 0;
int stackI[MAX_LENGTH];

void pushN(int x)
{
    stackI[spI++] = x;
}

int popN(void)
{
    return stackI[--spI];
}

struct node {
    int left;
    int right;
};
typedef struct node Node;

#define LEAF -1

int spA = -1;
void PUSH(int x, Node a[])
{
    a[x].right = spA;
    spA = x;
}

int POP(Node a[])
{
    int tmp = spA;
    spA = a[spA].right;
    return tmp;
}

void printTree(Node a[], int ip[], int x)
{
//    printf("printTree %d\n", x);
    if (x == LEAF) {  // x が LEAF の場合は何もない枝を表す
        return;
    } else if (a[x].left == LEAF && a[x].right == LEAF) {
        printf("_");
        return;
    }
    printf("(");
    printTree(a, ip, a[x].left);
    printf(",");
    printTree(a, ip, a[x].right);
    printf(")");
}

int end_comp = 0, lbl_comp = 0;

// Algorithm C
// Push only the nodes that are qualified as right children (if we have not yet visited the next node in inorder traversals.)
void algo_c(int ip[], int n, Node a[])
{
    pushN(ip[0]); /* ip[n] = n+1; */
    for (int i = 1; i < n; i++) {
        if (ip[i-1] > ip[i])         /* Test β */
            a[ip[i-1]].left = ip[i]; /* grafting a left child */
        else
            a[popN()].right = ip[i];  /* grafting a right child */
        if (a[ip[i]+1].left == LEAF) /* visited? */
            pushN(ip[i]);
    }
}

void algo_c_pointer(int *p, int n, Node a[])
{
    pushN(*p);
    for (p++; p < &a[n]; p++) {
        if (*(p-1) > *p)         /* Test β */
            a[*(p-1)].left = *p; /* grafting a left child */
        else
            a[popN()].right = *p;  /* grafting a right child */
        if (a[*p+1].left == LEAF) /* visited? */
            pushN(*p);
    }
}

// Alternative stack implementation
void algo_c_stack(int ip[], int n, Node a[])
{
    PUSH(ip[0],a); /* ip[n] = n+1; */
    for (int i = 1; i < n; i++) {
        if (ip[i-1] > ip[i])         /* Test β */
            a[ip[i-1]].left = ip[i]; /* grafting a left child */
        else
            a[POP(a)].right = ip[i];  /* grafting a right child */
        if (a[ip[i]+1].left == LEAF) /* visited? */
            PUSH(ip[i],a);
    }
}

// Debugging functions
void print_array(int ip[], int n)
{
    for (int i = 0; i < n; i++) {
        printf("%d ", ip[i]);
    }
    printf("\n");
}

void print_node_array(Node a[], int n)
{
    for (int i = 0; i < n; i++) {
        printf("%d: %d %d\n", i, a[i].left, a[i].right);
    }
}

void measure_execution_time(int ip[], int n, Node a[])
{
    // 開始時間を取得
    clock_t start = clock();

    // algo_cの実行
    algo_c(ip, n, a);

    // 終了時間を取得
    clock_t end = clock();

    // 実行時間を計算（秒単位で表示）
    double execution_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", execution_time);
}


int main(int argc, char *argv[])
{
    char str[MAX_LENGTH], *p;
    int n = 0;
    int length, ip[MAX_LENGTH];
    int root;                   /* index of root */
    int opt;
    int debug = 0;
    int algorithm = 'n';	/* default algorithm */

    while ((opt = getopt(argc, argv, "cdtxz")) != -1)
    {
        switch (opt)
        {
            case 'd':
		debug = 1;
                break;
            case 'c':
		algorithm = 'c';
                break;
            case 't':
		debug = 2;
                break;
            case 'x':
		algorithm = 'x';
                break;
            case 'z':
		debug = 9;
                break;
            case '?':
                fprintf(stderr, "Unknown option (1)\n");
                exit(EXIT_FAILURE);
            default:
                fprintf(stderr, "Usage: %s [-adm]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    {
        int result;
        result = scanf("%s", str);
        if (result != 1) {
            fprintf(stderr, "input error\n");
            return 1;
        }
    }
    length = strlen(str);
    if (str[length - 1] == '\n') {
        str[--length] = '\0';
    }

    // For the first entry
    p = strtok(str, ",");
    ip[0] = atoi(p);
    n++;

    while (p != NULL) {
        p = strtok(NULL, ",");
        if (p != NULL) {
            ip[n++] = atoi(p);
        }
    }

    Node *a = (Node*)malloc((n+1) * sizeof(Node));
    for (int i = 0; i < n+1; i++) {
        a[i].left = a[i].right = -1;
    }
    root = ip[0];
    switch (algorithm) {
    case 'c':
	algo_c_stack(ip, n, a);
	break;
    case 'x':
        break;
    default:
	fprintf(stderr, "Unknown option (2)\n");
	exit(EXIT_FAILURE);
    }

    switch (debug) {
    case 1: /* The option -d is for debugging to display the tree structure */
        puts("====");
        printf("ip[] = ");
        print_array(ip, n);
        printf("a[] is \n");
//        print_node_array(a, n);
        printTree(a, ip, ip[0]); printf("\n");
//        printf("\nlbl,end\n");
//        printf("%d,%d\n", lbl_comp, end_comp);
        break;
    case 2: /* The option -t is for only printing trees */
        printTree(a, ip, ip[0]); printf("\n");
        break;
    case 9: /* The option -z is for measuring execution time */
        measure_execution_time(ip, n, a);
        break;
    default:
    }

    return 0;
}
