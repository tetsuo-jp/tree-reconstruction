/*
   Algorithm N from:
   Tetsuo Yokoyama, Robert Glueck.
   "Faster Construction of Binary Trees from Their Traversals"

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
#define X     n+1		/* virtual label */
#define MAX_LENGTH 1024	/* max. length of input i-p seq. */

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

Node* node(void)
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
Label stackL[MAX_LENGTH];

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

void push(Label l, Node *n)
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

Node *algo_m(int ip[], int n)
{
    int i;
    struct node *prev, *root, *vroot;

    /* Block A */
    i = 1;
    vroot = node(); push(X, vroot); /* initialize stack with X */
    root  = node(); push(ip[0], root); vroot->left = root;

    while (end_comp++, i < n) { /* Test α */
	if (lbl_comp++, topL() > ip[i]) { /* Test β */
	    /* Block B */
	    topN()->left = node(); /* create the left child of the top node */
	    push(ip[i], topN()->left);
	} else {
	    do {
		/* Block C */
		prev = pop();
	    } while (lbl_comp++, topL() <= ip[i]); /* Test not β */
	    /* Block D */
	    prev->right = node(); /* create the right child of the previous top node */
	    push(ip[i], prev->right);
	}
	/* Block E */
	i++;
    }

    return root;
}

Node *algo_n(int ip[], int n)
{
    int i;
    struct node *vroot, *prev, *root;

    /* Block A' */
    ip[n] = X+1;            /* add virtual right child Y = X + 1 */
    i = 1;
    vroot = node(); push(X, vroot); /* initialize stack with X */
    root  = node(); push(ip[0], root); vroot->left = root;

    while (1) {
	if (lbl_comp++, topL() > ip[i]) { /* Test β */
	    /* Block B */
	    topN()->left = node(); /* create the left child of the top node */
	    push(ip[i], topN()->left);
	} else {
	    /* Block C */
	    prev = pop();
	    if (lbl_comp++, topL() <= ip[i]) { /* Test not β */
		    if (end_comp++, i >= n) /* Test not α */
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
		    } while (lbl_comp++, topL() <= ip[i]); /* Test not β */
	    }
	    /* Block D */
	    if (prev != NULL) {
    		prev->right = node(); /* create the right child of one of the previous top nodes */
	    	push(ip[i], prev->right);
	    } else {
		    fprintf(stderr, "Error: prev is NULL\n");
		    exit(EXIT_FAILURE);
	    }
	}
	/* Block E */
	i++;
    }

    free(vroot);

    return root;
}

Node *algo_b(int ip[], int n)
{
    int i;
    struct node *vroot, *vroot2, *prev, *root;

    /* Block A'' */
    ip[n] = X+1;            /* virtual label (X = n+1) */
    i = 1;
    vroot  = node(); push(X+1, vroot); /* initialize the node and label stacks with the bottom marker X+1 */
    vroot2 = node(); push(X, vroot2);
    root   = node(); push(ip[0], root);
    vroot->left = vroot2;
    vroot2->left = root;

    while (1) {
	if (lbl_comp++, ip[i] < topL()) { /* Test β */
	    /* Block B */
	    topN()->left = node(); /* create the left child of the top node */
	    push(ip[i], topN()->left);
	} else {
	    /* Block C */
	    prev = pop();
	    if (lbl_comp++, ip[i] >= topL()) { /* Test not β */
		/* Block C */
		prev = pop();
		if (lbl_comp++, ip[i] >= topL()) { /* Test not β */
		    if (end_comp++, i >= n) /* Test not α */
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
		    } while (lbl_comp++, ip[i] >= topL()); /* Test not β */
		}
	    }
	    /* Block D */
	    if (prev != NULL) {
		prev->right = node(); /* create the right child of one of the previous top nodes */
		push(ip[i], prev->right);
	    } else {
		fprintf(stderr, "Error: prev is NULL\n");
		exit(EXIT_FAILURE);
	    }
	}
	/* Block E */
	i++;
    }

    free(vroot->left);		/* vroot2 */
    free(vroot);

    return root;
}

int main(int argc, char *argv[])
{
    char str[MAX_LENGTH], *p, i = 0, n;
    int length, ip[MAX_LENGTH];
    Node *root;
    int opt;
    int debug = 0;
    int algorithm = 'n';	/* default algorithm */

    while ((opt = getopt(argc, argv, "bdmn")) != -1)
    {
        switch (opt)
        {
            case 'd':
		debug = 1;
                break;
            case 'n':
		algorithm = 'n';
                break;
            case 'b':
		algorithm = 'b';
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
    ip[0] = atoi(p);

    while (p != NULL) {
        p = strtok(NULL, ",");
        if (p != NULL) {
            ip[++i] = atoi(p);
        }
    }
    n = i + 1;

    switch (algorithm) {
    case 'n':
	root = algo_n(ip, n);
	break;
    case 'b':
	root = algo_b(ip, n);
	break;
    case 'm':
	root = algo_m(ip, n);
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

