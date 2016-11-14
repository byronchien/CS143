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

#include "BTreeNode.h"

int main()
{
 	// Testing BTreeNode
	// LeafNode
	printf("LeafNode tests\n");

 	BTLeafNode testLeafNode;
 	printf("Initial key count: %i\n", testLeafNode.getKeyCount());

 	int key = 1;
 	RecordId rid = {1,2};
 	testLeafNode.insert(key, rid);
 	printf("key count should be 1: %i\n", testLeafNode.getKeyCount());

 	// NonLeafNode
 	printf("NonLeafNode tests\n");

  	BTNonLeafNode testNonLeafNode;
 	printf("Initial key count: %i\n", testNonLeafNode.getKeyCount());

 	key = 1;
 	PageId pid = 1;
 	testNonLeafNode.insert(key, pid);
 	printf("key count should be 1: %i\n", testNonLeafNode.getKeyCount());



  // run the SQL engine taking user commands from standard input (console).
  //SqlEngine::run(stdin);
  return 0;
}
