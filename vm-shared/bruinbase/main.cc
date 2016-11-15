/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include "Bruinbase.h"
#include "SqlEngine.h"
#include <cstdio>

// Testing
#include "BTreeNode.h"

int main()
{
 	// Testing
 	int key;
	PageId pid;
	// LeafNode
	/*
	printf("LeafNode tests\n");

 	BTLeafNode testLeafNode;
 	printf("Initial key count: %i\n", testLeafNode.getKeyCount());

 	key = 1;
 	RecordId rid = {1,2};
 	testLeafNode.insert(key, rid);
 	printf("key count should be 1: %i\n", testLeafNode.getKeyCount());
 	*/

 	// NonLeafNode
 	printf("NonLeafNode tests\n");

 	// Insert
 	BTNonLeafNode testNonLeafNode;
 	printf("Initial key count: %i\n", testNonLeafNode.getKeyCount());

 	key = 127;
 	pid = 1;
 	testNonLeafNode.insert(key, pid);

 	key = 99;
 	pid = 2;
 	testNonLeafNode.insert(key, pid);

 	key = 77;
 	pid = 3;
 	testNonLeafNode.insert(key, pid);

  	key = 77;
 	pid = 3;
 	testNonLeafNode.insert(key, pid);

 	for (int i = 0; i < 4; i++) {
		printf("Key, PageId %i: %i, %i\n", i, testNonLeafNode.buffer[i * 8],
			testNonLeafNode.buffer[i * 8 + 4]);
	}
	
	printf("Key Count: %i\n", testNonLeafNode.getKeyCount());	

	// InsertAndSplit
	
	BTNonLeafNode testNonLeafNode2;
	printf("Initial key count: %i\n", testNonLeafNode2.getKeyCount());
	for (int i = 0; i < 84; i++) {
		testNonLeafNode2.insert(i, i);
	}
	printf("Key count (84): %i\n", testNonLeafNode2.getKeyCount());

	BTNonLeafNode sibling;
	
	int midKey = -1;
	// 1,1 40,40, 41,41, 42,42, 80,80
	testNonLeafNode2.insertAndSplit(80,80, sibling, midKey);
	
	for (int i = 0; i < testNonLeafNode2.getKeyCount(); i++) {
		printf("Key, PageId %i: %i, %i\n", i, testNonLeafNode2.buffer[i * 8],
			testNonLeafNode2.buffer[i * 8 + 4]);
	}
	printf("Key Count: %i\n", testNonLeafNode2.getKeyCount());	
	
	for (int i = 0; i < sibling.getKeyCount(); i++) {
		printf("Key, PageId %i: %i, %i\n", i, sibling.buffer[i * 8],
			sibling.buffer[i * 8 + 4]);
	}
	printf("Key Count sibling: %i\n", sibling.getKeyCount());	
	
	printf("midKey: %i\n", midKey);	
	
	// Testing
  // run the SQL engine taking user commands from standard input (console).
  //SqlEngine::run(stdin);
  return 0;
}
