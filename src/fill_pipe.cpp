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
#include <getopt.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

struct node_t {
	char *word;
	char *val;
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

			 void incword(const char *s, const char *v)
			 {
				 node_t* p;
				 int h = hash(s);
				 for (p = bin[h]; p; p = p->next)
					 if (strcmp(s, p->word) == 0) {
						 ++p->count;
						 return;
					 }
				 p = nmalloc();
				 p->count = 1;
				 p->word = smalloc(strlen(s)+1);
				 p->val = smalloc(strlen(v)+1);
				 strcpy(p->word, s);
				 strcpy(p->val, v);
				 p->next = bin[h];
				 bin[h] = p;
			 }

			 char * find(const char *s){
				 node_t* p;
				 int h = hash(s);
				 for (p = bin[h]; p; p = p->next)
					 if (strcmp(s, p->word) == 0) {
						 p->count=0;
						 return p->val;
					 }
				 return NULL;
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

void split(const char * buf,char * key, char * value){
	memset( key, '\0', 4096 );
	memset( value, '\0', 4096 );
	char * pch=strchr(buf,'	');
	if(pch!=NULL){
		strncpy(key, buf, pch-buf);
		strcpy(value, pch+1);
	}else{
		strcpy(key, buf);
	}
}

void cut(const char * buf,char * key, char * value, int length){
        memset( key, '\0', 4096 );
        memset( value, '\0', 4096 );
	strncpy(key, buf, length);
	strcpy(value, buf);
}

int main( int argc, char * argv[] ){
	my_hash_t<999983> url_hash;
	char buf[4096];
	char * pch;
	char * result;
	char substr[4096];
	char value[4096];
	char *fvalue = NULL;
	int c;
	int length = 0;

	while ((c = getopt (argc, argv, "f:c:")) != -1)
		switch (c)
		{
			case 'f':
				fvalue = optarg;
				break;
			case 'c':
				length = atoi(optarg);
				break;
			case '?':
				if (optopt == 'c' || optopt == 'f')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr,
							"Unknown option character `\\x%x'.\n",
							optopt);
				return 1;
			default:
				abort ();
		}

	ifstream fin(fvalue);

	while ( fin.getline( buf, 4096 ) ){
		split(buf, substr, value);
		url_hash.incword(substr, value);
	}

	while ( cin.getline( buf, 4096 ) ){
		if(length == 0)
			split(buf, substr, value);
		else
			cut(buf, substr, value, length);
		if((result = url_hash.find(substr))!=NULL){
			cout << buf << "\t" << result << endl;
		}else{
			cout << buf << "\t-" << endl;
		}
	}

	return 0;
}
