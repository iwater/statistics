/*
 * Rewrite in C++ by Jakob Oestergaard
 * of:  (original copyright follows)
 */

/* Copyright (C) 1999 Lucent Technologies */
/* From 'Programming Pearls' by Jon Bentley */

/* wordfreq.c -- list of words in file, with counts */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>

using namespace std;

struct node_t {
  char *word;
  int count;
  node_t *next;
};

template <typename T>
struct alist_t {
  alist_t(size_t elms) : next(0) {
    area = new T[elms];
  }
  ~alist_t() {
    if (next) delete next;
    delete[] area;
  }
  T* allocate(size_t elms) {
    alist_t *n = new alist_t<T>(elms);
    n->next = next;
    next = n;
    return n->area;
  }
  T *area;
  alist_t<T> *next;
};

template <unsigned NHASH = 29989,    unsigned MULT = 31,
	  unsigned NODEGROUP = 1000, unsigned CHARGROUP = 1000>
class my_hash_t {
public:
  my_hash_t()
    : bin(NHASH,0), nodesleft(0), charsleft(0),
      nglist(NODEGROUP), cglist(CHARGROUP) { }

  unsigned hash(const char *p) const
  {
    unsigned int h = 0;
    while (*p)
      h = MULT * h + *p++;
    return h % NHASH;
  }

  void incword(const char *s)
  {
    node_t* p;
    int h = hash(s);
    for (p = bin[h]; p; p = p->next)
      if (strcmp(s, p->word) == 0) {
	return;
      }
    cout << s << endl;
    p = nmalloc();
    p->count = 1;
    p->word = smalloc(strlen(s)+1);
    strcpy(p->word, s);
    p->next = bin[h];
    bin[h] = p;
  }

  node_t *const * begin() const { return &bin[0]; }
  node_t *const * end() const { return &bin[NHASH]; }

private:
  std::vector<node_t*> bin;
  int nodesleft;
  node_t* freenode;
  int charsleft;
  char *freechar;
  alist_t<node_t> nglist;
  alist_t<char> cglist;

  node_t* nmalloc()
  {
    if (nodesleft == 0) {
      freenode = nglist.allocate(NODEGROUP);
      nodesleft = NODEGROUP;
    }
    nodesleft--;
    return freenode++;
  }

  char *smalloc(int n)
  {
    if (charsleft < n) {
      freechar = cglist.allocate(n+CHARGROUP);
      charsleft = n+CHARGROUP;
    }
    charsleft -= n;
    freechar += n;
    return freechar - n;
  }
};

bool less_second(const pair<string,int> & m1, const pair<string,int> & m2) { return m1.second > m2.second; }

int main( int argc, char * argv[] ){
  my_hash_t<999983> url_hash;
  char buf[4096];
  while ( cin.getline( buf, 4096 ) )
    url_hash.incword(buf);

  return 0;
}
