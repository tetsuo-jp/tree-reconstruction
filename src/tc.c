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
#include <stdbool.h>

#define Label int
#define X     n+1		/* virtual label */
#define MAX_LENGTH 128          /* max. length of input i-p seq. */

struct node {
    struct node *left;
    struct node *right;
};

typedef struct node Node;

int spN = 0;
Node *stackN[MAX_LENGTH];

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

Node *node(void)
{
    Node *node = (Node*)calloc(1, sizeof(Node));
    if (node == NULL) {
        perror("Cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    return node;
}

Node *node_array(int n)
{
    Node *nodes = (Node*)calloc(n, sizeof(Node));
    if (nodes == NULL) {
        perror("Cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    return nodes;
}

void printTree(Node *node)
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
    if (spL >= MAX_LENGTH) {
        fprintf(stderr, "Error: stack overflow in pushL!\n");
        exit(EXIT_FAILURE);
    }
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
    Node *prev, *root, *vroot;

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
    Node *vroot, *prev, *root;

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

// N with the inner loop unrolled twice
Node *algo_b(int ip[], int n)
{
    int i;
    Node *vroot, *vroot2, *prev, *root;

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

// Algorithm C': minimize the number of label comparisons
//  with sentinel
Node *algo_c2(int ip[], int n)
{
    int i;
    Node *root, *c;
    int processed[MAX_LENGTH] = {0};

    /* Block A'' */
    ip[n] = X+1;            /* virtual label (X = n+1) */
    i = 1;
    c = root = node(); push(ip[0], root);
    processed[ip[0]] = 1;

    while (1) {
	if (lbl_comp++, ip[i-1] > ip[i]) { /* Test β */
	    /* Block B' */
	    c = c->left = node(); /* create the left child */
	} else {
	    /* Block C' */
            c = pop()->right = node();
	}
        if (end_comp++, !processed[ip[i]+1]) {
            if (end_comp++, i >= n) /* Test not α */
                break;
            push(ip[i], c);
        }
        processed[ip[i]] = 1;
	/* Block E */
	i++;
    }

    return root;
}

// Algorithm C: minimize # of label comparisons (n-1)
// # of the other comparisons = n (loop) + n-1 (visit?) = 2n-1
// Push only the nodes that are qualified as right children (if we have not yet visited the next node in inorder traversals.)
Node *algo_c3(int ip[], int n)
{
    Node *root, *c;
    char visited[MAX_LENGTH] = {0};    /* always O(n) space overhead required */

    c = root = node(); push(ip[0], root); /* the root may or may not have a right child */
    visited[ip[0]] = 1;

    for (int i = 1; i < n; i++) {
	if (ip[i-1] > ip[i])           /* Test β */
	    c = c->left = node();      /* create the left child */
	else
            c = pop()->right = node(); /* create the right child */
        if (!visited[ip[i]+1])         /* elems of ip are supporsed to be integers */
            push(ip[i], c);            /* push a node that HAS a right child */
        visited[ip[i]] = 1;
    }

    return root;
}

// Algorithm C: minimize # of label comparisons (n-1)
// # of the other comparisons = n + n-1 = 2n-1
// Push only the nodes that are qualified as right children (if we have not yet visited the next node in inorder traversals.)
// Assigments are modified from algo_c3
Node *algo_c3b(int ip[], int n)
{
    Node *root, *c;
    char visited[MAX_LENGTH] = {0};

    root = node(); c = root; push(ip[0], root);
    visited[ip[0]] = 1;

    for (int i = 1; i < n; i++) {
	if (ip[i-1] > ip[i]) {  /* Test β */
	    c->left = node();   /* create the left child */
            c = c->left;
        } else {
            c = pop();          /* create the right child */
            c->right = node();
            c = c->right;
        }
        if (!visited[ip[i]+1])
            push(ip[i], c);
        visited[ip[i]] = 1;
    }

    return root;
}

// Algorithm C: minimize # of label comparisons (n-1)
// # of the other comparisons = n + n-1 = 2n-1
Node *algo_c4(int ip[], int n)
{
    Node *s, *p;
    bool visited[MAX_LENGTH] = {0};

    Node *nodes = node_array(n);
    s = &nodes[0];
    visited[ip[0]] = true;

    for (int i = 1; i < n; i++) {
	if (ip[i-1] > ip[i])           /* Test β */
	    nodes[i-1].left = &nodes[i];      /* the left child */
	else {
            p = s;
            s = s->right;
            p->right = &nodes[i]; /* the right child */
        }
        if (!visited[ip[i]+1]) {
            nodes[i].right = s;
            s = &nodes[i];
        }
        visited[ip[i]] = true;
    }

    return nodes;
}

Node *algo_c4b(int ip[], int n)
{
    Node *nodes = node_array(n+1);
    Node *s = &nodes[ip[0]];        /* stack */

    for (int i = 1; i < n; i++) {
	if (ip[i-1] > ip[i]) {                   /* Test β */
	    nodes[ip[i-1]].left = &nodes[ip[i]]; /* grafting a left child */
	} else {
            Node *p = s;
            s = s->right;             /* pop */
            p->right = &nodes[ip[i]]; /* grafting a right child */
        }
        if (nodes[ip[i]+1].left == 0) {
            nodes[ip[i]].right = s; s = &nodes[ip[i]];  /* push */
        }
    }

    return &nodes[ip[0]];
}

// Algorithm C: minimize # of label comparisons (n-1)
// # of the other comparisons = n + n-1 = 2n-1
// Push only the nodes that are qualified as right children (if we have not yet visited the next node in inorder traversals.)
Node *algo_c5(int ip[], int n)
{
    Node *s, *p;
    bool visited[MAX_LENGTH] = {0};

    Node *nodes = node_array(n);
    s = &nodes[0];
    visited[ip[0]] = true;

    for (int i = 1; i < n; i++) {
        Node *c = &nodes[i];
	if (ip[i-1] > ip[i])           /* Test β */
	    nodes[i-1].left = c;      /* the left child */
	else {
            p = s;
            s = s->right;
            p->right = c; /* the right child */
        }
        if (!visited[ip[i]+1]) {
            nodes[i].right = s;
            s = c;
        }
        visited[ip[i]] = true;
    }

    return nodes;
}

// Algorithm C: minimize # of label comparisons (n-1)
// # of the other comparisons = n + n-1 = 2n-1
Node *algo_c6(int ip[], int n)
{
    Node *s, *p;
    bool visited[MAX_LENGTH] = {0};
    Node *nodes = node_array(n+1);
    int i;

    ip[n] = X+1;            /* virtual label (X = n+1) */
    s = &nodes[0];
    visited[ip[0]] = true;
    i = 1;

    while (1) {
        Node *c = &nodes[i];
	if (lbl_comp++, ip[i-1] > ip[i])           /* Test β */
	    nodes[i-1].left = c;      /* the left child */
	else {
            if (end_comp++, i >= n) /* Test not α */
                break;
            p = s;
            s = s->right;
            p->right = c; /* the right child */
        }
        if (end_comp++, !visited[ip[i]+1]) {
            nodes[i].right = s;
            s = c;
        }
        visited[ip[i]] = true;
        i++;
    }

    return nodes;               /* return root */
}

Node *algo_c(int ip[], int n) {
    return algo_c4b(ip, n);
}

int main(int argc, char *argv[])
{
    char str[MAX_LENGTH], *p;
    int i = 0, n;
    int length, ip[MAX_LENGTH];
    Node *root;
    int opt;
    int debug = 0;
    int algorithm = 'n';	/* default algorithm */

    while ((opt = getopt(argc, argv, "bcdmnt")) != -1)
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
            case 'c':
		algorithm = 'c';
                break;
            case 't':
		debug = 2;
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
    case 'c':
	root = algo_c(ip, n);
	break;
    case 'm':
	root = algo_m(ip, n);
	break;
    default:
	fprintf(stderr, "Unknown option\n");
	exit(EXIT_FAILURE);
    }

    switch (debug) {
    case 2: /* The option -t is for only printing trees */
        printTree(root); printf("\n");
        break;
    case 1: /* The option -d is for debugging to display the tree structure */
        puts("====");
        printTree(root);
        printf("\nlbl,end\n");
        printf("%d,%d\n", lbl_comp, end_comp);
    default:
    }

    return 0;
}
