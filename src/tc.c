/*
   Algorithm A from:
   Tetsuo Yokoyama, Robert Glueck.
   "A Note on Efficient Binary Tree Construction."

             label comparison | end comparison | Total
   best  case       n+1       |       1        |  n+2
   worst case      2n         |     |n/2|      | 2n+|n/2|

   Here, given a real number x, |x| returns the least integer greater than or equal to x.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>             /* strlen */
#include <unistd.h>		/* getopt */

#define Label int
#define X     n+1              	/* ??? why not n? */

struct node {
    struct node* left;
    struct node* right;
};

typedef struct node Node;

int spN = 0;
Node *stackN[128];

void pushN(Node *x)
{
    stackN[spN++] = x;
}

Node *popN(void)
{
    return stackN[--spN];
}

Node *topN(void)
{
    return stackN[spN-1];
}

Node* createN(void)
{
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "Cannot allocate memory\n");
        exit(EXIT_FAILURE);
    }

    node->left = NULL;
    node->right = NULL;

    return node;
}

void printTree(Node* node)
{
    if (node != NULL) {
        if (node->left == NULL && node->right == NULL) {
            printf("_");
        } else {
            printf("(");
            printTree(node->left);
            printf(",");
            printTree(node->right);
            printf(")");
        }
    }
}

int spL = 0;
Label stackL[128];

void pushL(Label x)
{
    stackL[spL++] = x;
}

Label popL(void)
{
    return stackL[--spL];
}

Label topL(void)
{
    return stackL[spL-1];
}

void pushLN(Label l, Node *n)
{
    pushN(n);
    pushL(l);
}

Node *pop(void)
{
    popL();
    return popN();
}

int end_comp = 0, lbl_comp = 0;

Node *algo_m(int preorder[], int n)
{
    int i;
    struct node *x;
    struct node *prev;
    struct node *root;

    /* Block A */
    i = 1;
    x = createN(); pushLN(X, x); /* initialize the node and label stacks with the bottom marker X (= n+1) */
    root = createN(); pushLN(preorder[0], root);
    x->left = root;

    while (end_comp++, i < n) { /* Test α */
	if (lbl_comp++, preorder[i] < topL()) { /* Test β */
	    /* Block B */
	    topN()->left = createN(); /* create the left child of the top node */
	    pushLN(preorder[i], topN()->left);
	} else {
	    do {
		/* Block C */
		prev = pop();
	    } while (lbl_comp++, preorder[i] >= topL()); /* Test not β */
	    /* Block D */
	    prev->right = createN(); /* create the right child of the previous top node */
	    pushLN(preorder[i], prev->right);
	}
	/* Block E */
	i++;
    }

    return root;
}

Node *algo_a(int preorder[], int n)
{
    int i;
    struct node *x;
    struct node *prev;
    struct node *root;

    /* Block A' */
    preorder[n] = X+1;            /* virtual label (X = n+1) */

    /* Block A */
    i = 1;
    x = createN(); pushLN(X, x); /* initialize the node and label stacks with the bottom marker X */
    root = createN(); pushLN(preorder[0], root);
    x->left = root;

    while (1) {
	if (lbl_comp++, preorder[i] < topL()) { /* Test β */
	    /* Block B */
	    topN()->left = createN(); /* create the left child of the top node */
	    pushLN(preorder[i], topN()->left);
	} else {
	    /* Block C */
	    prev = pop();
	    if (lbl_comp++, preorder[i] >= topL()) { /* Test not β */
		if (end_comp++, i >= n) /* Test α */
		    break;
		do {
		    /* Block C */
		    /* Check if the stack is empty before popping elements */
		    if (spN > 0) {
			prev = pop();
		    } else {
			fprintf(stderr, "Error: Stack is empty\n");
			exit(EXIT_FAILURE);
		    }
		} while (preorder[i] >= topL()); /* Test not β */
	    }
	    /* Block D */
	    if (prev != NULL) {
		prev->right = createN(); /* create the right child of one of the previous top nodes */
		pushLN(preorder[i], prev->right);
	    } else {
		fprintf(stderr, "Error: prev is NULL\n");
		exit(EXIT_FAILURE);
	    }
	}
	/* Block E */
	i++;
    }

    return root;
}

int main(int argc, char *argv[])
{
    char str[1024], *p, i = 0, n;
    int length, preorder[130];
    Node *root;
    int opt;
    int debug = 0;
    int algorithm = 'a';

    while ((opt = getopt(argc, argv, "adm")) != -1)
    {
        switch (opt)
        {
            case 'd':
		debug = 1;
                break;
            case 'a':
		algorithm = 'a';
                break;
            case 'm':
		algorithm = 'm';
                break;
            case '?':
                fprintf(stderr, "Unknown option\n");
                exit(EXIT_FAILURE);
            default:
                fprintf(stderr, "Usage: %s [-adm]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    scanf("%s", str);
    length = strlen(str);
    if (str[length - 1] == '\n') {
        str[--length] = '\0';
    }

    // For the first entry
    p = strtok(str, ",");
    preorder[0] = atoi(p);

    while (p != NULL) {
        p = strtok(NULL, ",");
        if (p != NULL) {
            preorder[++i] = atoi(p);
        }
    }
    n = i + 1;

    switch (algorithm) {
    case 'a':
	root = algo_a(preorder, n);
	break;
    case 'm':
	root = algo_m(preorder, n);
	break;
    default:
	fprintf(stderr, "Unknown option\n");
	exit(EXIT_FAILURE);
    }

    /* The option -d is for debugging to display the tree structure */
    if (debug) {
        puts("====");
        printTree(root);
        printf("\nlbl,end\n");
    }
    printf("%d,%d\n", lbl_comp, end_comp);
}

