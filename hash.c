/*
 * hash.c - implementation of simple linked lists and hashtables in C
 * 2020/12/29 vasilios daskalopoulos
 * vasil9v@gmail.com
 *
 * to compile and run:
 *   $ gcc hash.c -o hash ; strip hash
 *   $ ./hash
 *
 * TODO
 *   can i get it under 8k? (current record 8848 stripped on osx with no DEBUG)
 *
 */

#include <stdlib.h>
#include <string.h>

#define DEBUG 1
#ifdef DEBUG
#include <stdio.h>
#endif

/* linked lists */
typedef struct node_struct {
   void *data;
   struct node_struct *next;
} node;

node *
nodecreate(void *data) {
  node *p = malloc(sizeof(node));
  p->data = data;
  p->next = NULL;
  return p;
}

void
nodekill(node *p) {
  // don't free p->data since we don't malloc it
  free(p);
}

void
listkill(node *p) {
  if (p != NULL) {
    if (p->next != NULL) {
      listkill(p->next);
    }
    free(p);
  }
}

#ifdef DEBUG
  void
  listdump(node *pl) {
    node *p = pl;
    printf("list:\n");
    while (p != NULL) {
      printf("  %s\n", (char *)p->data);
      p = p->next;
    }
  }
#endif

int
listappend(node *pl, node *q) {
  int count;
  node *p = pl;
  count = 0;
  while (p != NULL) {
    if (p->next == NULL) {
      p->next = q;
      return count + 1;
    }
    count++;
    p = p->next;
  }
  return count; // TODO write test for this
}

/* pair */
typedef struct pair_struct {
   void *data1;
   void *data2;
} pair;

pair *
paircreate(void *data1, void *data2) {
  pair *p = malloc(sizeof(pair));
  p->data1 = NULL;
  p->data2 = NULL;
  if (data1 != NULL) {
    p->data1 = (char *)malloc(strlen((char *)data1) + 1);
    strcpy(p->data1, (char *)data1);
  }
  if (data2 != NULL) {
    p->data2 = (char *)malloc(strlen((char *)data2) + 1);
    strcpy(p->data2, (char *)data2);
  }
  return p;
}

void
pairkill(pair *p) {
  free(p->data1);
  free(p->data2);
  free(p);
}

/* hash:
  entries[]
    |
    |-->[k,v]-->[k,v]-->[k,v]... L
    |
    ...
    |-->[k,v]-->[k,v]-->[k,v]... L
    |
    H
*/

int
mkhashval(char *s, int m) {
  int hash = 0, i, L;
  L = strlen(s);
  for (i = 0; i < L; i++) {
    hash += s[i];
  }
  return hash % m;
}

typedef struct hash_struct {
   node **entries; // array of linked list heads
   int H;
   int maxlen;
} hash;

hash *
hashcreate(int H) {
  int i;
  hash *p = malloc(sizeof(hash));
  p->entries = (node **)malloc(H * sizeof(node *));
  for (i = 0; i < H; i++) {
    p->entries[i] = NULL;
  }
  p->H = H;
  p->maxlen = 0;
  return p;
}

void
hashkill(hash *p) {
  int i;
  node *pp;
  for (i = 0; i < p->H; i++) {
    pp = p->entries[i];
    while(pp != NULL) {
      pairkill(pp->data);
      pp = pp->next;
    }
    listkill(p->entries[i]);
  }
  free(p->entries);
  free(p);
}

void
hashadd(hash *h, pair *q) {
  int hval = mkhashval(q->data1, h->H);
  node *qnode = nodecreate(q);
  qnode->data = q;
  int L = 0;
  node *entry = h->entries[hval];
  if (entry != NULL) {
    listappend(entry, qnode);
  }
  else {
    h->entries[hval] = qnode;
  }
  if (L > h->maxlen) {
    h->maxlen = L;
  }
}

// TODO hash delete
// TODO implement rehash by checking maxlen

node *
hashfind(hash *h, char *key) {
  int hval = mkhashval(key, h->H);
  node *entry = h->entries[hval];
  node *p = entry;
  while (p != NULL) {
    if (strcmp(key, ((pair *)p->data)->data1) == 0) {
      return p;
    }
    p = p->next;
  }
  return NULL;
}

#ifdef DEBUG
  void
  hashdump(hash *h) {
    int i;
    printf("hash(%d):\n", h->H);
    for (i = 0; i < h->H; i++) {
      printf("  entry %d\n", i);
      node *p = h->entries[i];
      while (p != NULL) {
        printf("    %s->%s\n", ((pair *)p->data)->data1, ((pair *)p->data)->data2);
        p = p->next;
      }
    }
  }
#endif

void
set(hash *h, char *key, char *value) {  
  node *a = hashfind(h, key);
  pair *pp;
  if (a != NULL) {
    pp = (pair *)a->data;
    free(pp->data2);
    pp->data2 = (char *)malloc(strlen(value) + 1);
    strcpy(pp->data2, value);
  }
  else {
    pair *p = paircreate(key, value);
    hashadd(h, p);
  }
}

char *
get(hash *h, char *key) {
  node *p = hashfind(h, key);
  if (p != NULL) {
    return ((pair *)p->data)->data2;
  }
  return NULL;
}

int
assert(int i) {
  if (!i) {
    #ifdef DEBUG
      printf("ERROR: assertion failed\n");
    #endif
  }
  return i;
}

void
testlist() {
  char *s1 = "foo";
  char *s2 = "bar";
  char *s3 = "baz";
  node *pn;
  node *p = nodecreate(s1);
  listappend(p, nodecreate(s2));
  listappend(p, nodecreate(s3));
  #ifdef DEBUG
    listdump(p);
  #endif
  assert(strcmp((char *)p->next->data, s2) == 0);
  listkill(p);
}

void
testhash() {
  char *keyfoo = "keyfoo";
  char *valfoo = "valfoo";
  char *valfoo2 = "valfoo2";
  char *keybar = "keybar";
  char *valbar = "valbar";
  char *keyoof = "keyoof"; // will hash to same as keyfoo
  hash *h = hashcreate(13);
  set(h, keyfoo, valfoo);
  set(h, keyfoo, valfoo2);
  set(h, keybar, valbar);
  set(h, keyoof, valbar);
  #ifdef DEBUG
    hashdump(h);
  #endif
  assert(strcmp(get(h, keyfoo), valfoo2) == 0);
  assert(strcmp(get(h, keyoof), valbar) == 0);
  assert(strcmp(get(h, keybar), valbar) == 0);
  hashkill(h);
}

int
main(int argc, char *argv[]) {
  testlist();
  testhash();
  return 0;
}
