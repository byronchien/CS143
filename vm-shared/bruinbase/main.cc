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
#include "BTreeIndex.h"
#include "BTreeNode.h"

int main()
{
	BTreeIndex btree;
	RecordId rid;
	RC rc;
	if ((rc = btree.open("pagefile.txt", 'w')) != 0) return rc;

	/*
	rid.pid = 5;
	rid.sid = 5;
	int key = 5;
	if ((rc = btree.insert(key, rid)) != 0) {
		printf("insert\n");
		printf("%i\n", rc);		
	}

	IndexCursor cursor;
	if ((rc = btree.locate(key, cursor)) != 0) {
		printf("locate\n");
		printf("%i\n", rc);		
	}

	BTLeafNode node;
	node.read(cursor.pid, btree.pf);

	RecordId rid2;
	int key2;
	node.readEntry(0, key2, rid2);
	printf("%i %i %i \n", key2, rid2.pid, rid2.sid);
	*/


	for (int i = 5; i < 7; i++) {
		rid.pid = i;
		rid.sid = i;
		if ((rc = btree.insert(i, rid)) != 0) {
			printf("insert\n");
			printf("%i\n", rc);		
		}
	}

	IndexCursor cursor;
	BTLeafNode node;
	RecordId rid2;
	int key2;

	for (int i = 5; i < 7; i++) {
		if ((rc = btree.locate(i, cursor)) != 0) {
			printf("locate\n");
			printf("%i\n", rc);		
		}	
		else {
			node.read(cursor.pid, btree.pf);
			node.readEntry(0, key2, rid2);
			printf("%i %i %i \n", key2, rid2.pid, rid2.sid);			
		}
	}



	
	
	
	
	if ((rc = btree.close()) != 0) return rc;	

/*
	IndexCursor cursor;

	btree.locate(0, cursor);

	BTLeafNode node;

	node.read(cursor.pid, btree.pf);

	int x;
	for (int k = 0; k < node.getKeyCount(); k++)
	{
		node.readEntry(k, x, rid);
		printf("Entry: {Key: %i, RecordId pid: %i, RecordId sid: %i\n",
			x, rid.pid, rid.sid);
	}

	printf("same\n");
*/

	/*
 	// Testing
 	int key;
	PageId pid;
	// LeafNode
	
	printf("LeafNode tests\n");

 	BTLeafNode testLeafNode;
 	printf("Initial key count: %i\n", testLeafNode.getKeyCount());

 	key = 1;
 	RecordId rid = {1,2};
 	testLeafNode.insert(key, rid);
 	printf("key count should be 1: %i\n", testLeafNode.getKeyCount());
 	
 	for (int i = 2; i < 85; i++)
 	{
 		RecordId rid2 = {i, i+1};
 		testLeafNode.insert(i, rid2);
 	}

 	printf("key count should be 84: %i\n", testLeafNode.getKeyCount());

 	key = 85;
 	rid.pid = 85;
 	rid.sid = 86;
 	int error = testLeafNode.insert(key, rid);
 	printf("this should be an error: %i\n", error);
 	printf("RC_NODE_FULL = %i\n", RC_NODE_FULL);

 	BTLeafNode testLeafNode2;

	int key2;
 	testLeafNode.insertAndSplit(key, rid, testLeafNode2, key2);

 	printf("keys in the first node: %i\n", testLeafNode.getKeyCount());
 	printf("keys in the escond node: %i\n", testLeafNode2.getKeyCount());

 	printf("entries in first node\n");
 	for (int i = 0; i < testLeafNode.getKeyCount(); i++)
 		testLeafNode.readEntry(i, key, rid);


 	printf("entries in second node\n");
 	for (int i = 0; i < testLeafNode2.getKeyCount(); i++)
		testLeafNode2.readEntry(i, key, rid);


	testLeafNode.setNextNodePtr(12);
	printf("next node ptr: %i\n", testLeafNode.getNextNodePtr());
*/
/*
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
	*/
	// Testing
  // run the SQL engine taking user commands from standard input (console).
  //SqlEngine::run(stdin);


  return 0;
}
