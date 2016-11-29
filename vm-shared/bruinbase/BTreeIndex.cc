/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"
//#include "conversion.h"

#include <cstdio>

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
    treeHeight = 0;
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{
   	RC rc;
  	// open the page file
  	if ((rc = pf.open(indexname, mode)) < 0) return rc;
  	return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
	return pf.close();
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
	RC rc;

	// if tree is empty
	if (rootPid == -1) {
		
		BTLeafNode leaf;
		leaf.insert(key, rid);
		rootPid = pf.endPid();
		if (rootPid == 0) rootPid = 1;
		if ((rc = leaf.write(rootPid, pf)) != 0) return rc;		

		treeHeight = 1;
		char buffer[PageFile::PAGE_SIZE];
		intToChar(treeHeight, buffer);
		intToChar(rootPid, buffer + 4);
		if ((rc = pf.write(0, (const void*) buffer)) != 0) return rc;

		return 0;
		

		/*
		BTLeafNode leaf;
		leaf.insert(key, rid);
		PageId pid = pf.endPid();
		if (pid == 0) pid = 1;
		if ((rc = leaf.write(pid, pf)) != 0) return rc;

		BTNonLeafNode root;
		if ((rc = root.initializeRoot(pid, key, pid)) != 0) return rc;
		rootPid = pf.endPid();
		if (rootPid == 0) rootPid = 1;
		if ((rc = root.write(rootPid, pf)) != 0) return rc;

		treeHeight = 2;
		char buffer[PageFile::PAGE_SIZE];
		intToChar(treeHeight, buffer);
		intToChar(rootPid, buffer + 4);
		if ((rc = pf.write(0, (const void*) buffer)) != 0) return rc;

		return 0;
		*/
	}
	
	char buffer[PageFile::PAGE_SIZE];
	if ((rc = pf.read(0, (void*) buffer)) != 0) return rc;
	charToInt(buffer, treeHeight);
	charToInt(buffer + 4, rootPid);

	IndexCursor cursor;
	rc = locate(key, cursor);
	if (rc == RC_NO_SUCH_RECORD || rc == RC_END_OF_TREE) {
		//printf("located %i %i %i \n", key, rid.pid, rid.sid);
		int midKey = -1;
		PageId midPid = -1;
		if ((rc = this->insertRecursive(key, rid, 1, midKey, rootPid, midPid)) != 0) return rc;
		//if ((rc = close()) != 0) return rc;
		return 0;
	}
	else {
		return rc;
		// duplicate key value
		//if ((rc = close()) != 0) return rc;
		//return -1;
	}
}

RC BTreeIndex::insertRecursive(int key, const RecordId& rid, int currHeight, 
	int& midKey, PageId insertPid, PageId &midPid)
{
	RC rc;
	char buffer[PageFile::PAGE_SIZE];
	if ((rc = pf.read(0, (void*) buffer)) != 0) return rc;
	charToInt(buffer, treeHeight);

	// inserting into leaf nodes
	if(currHeight == treeHeight)
	{
		

		BTLeafNode leafnode;
		leafnode.read(insertPid, pf);

		int keyCount = leafnode.getKeyCount();
		if (keyCount < 84 && keyCount >= 0) {	
			if ((rc = leafnode.insert(key, rid)) != 0) return rc;
			if ((rc = leafnode.write(insertPid, pf)) != 0) return rc;

			/*
			if (leafnode.getKeyCount() == 84 || key == 1000) {
		 		printf("inserted key pid sid: %i %i %i\n", key, rid.pid, rid.sid);
		 		printf("getKeyCount: %i\n", leafnode.getKeyCount());
		 		int key2;
		 		RecordId rid2;
		 		for (int i = 0; i < leafnode.getKeyCount(); i++) {
		 			leafnode.readEntry(i, key2, rid2);
		 			printf("%i %i %i %i \n", i, key2, rid2.pid, rid2.sid);
		 		}	
		 		printf("\n");		
			}			
			*/

			return 0;
		}	
		else if (keyCount == 84) {

			BTLeafNode sibling;
			int siblingKey;
			if (leafnode.insertAndSplit(key, rid, sibling, siblingKey) == 0) {
				
				PageId siblingPid = pf.endPid();
				if (siblingPid == 0) siblingPid = 1;	
				sibling.setNextNodePtr(leafnode.getNextNodePtr());
				leafnode.setNextNodePtr(siblingPid);
				if ((rc = leafnode.write(insertPid, pf)) != 0) return rc;
				if ((rc = sibling.write(siblingPid, pf)) != 0) return rc;
				midKey = siblingKey;
				midPid = siblingPid;

				/*
				int key2;
		 		RecordId rid2;
				for (int i = 0; i < leafnode.getKeyCount(); i++) {
					leafnode.readEntry(i, key2, rid2);
					printf("%i %i %i %i \n", i, key2, rid2.pid, rid2.sid);
				}
				for (int i = 0; i < sibling.getKeyCount(); i++) {
					sibling.readEntry(i, key2, rid2);
					printf("%i %i %i %i \n", i, key2, rid2.pid, rid2.sid);
				}
				printf("\n");
				*/
				
				// initialize first root
				if (treeHeight == 1) {
					//printf("%i %i\n", midKey, siblingPid);
					BTNonLeafNode newroot;
					rc = newroot.initializeRoot(insertPid, midKey, siblingPid);
					rootPid = pf.endPid();
					if ((rc = newroot.write(rootPid, pf)) != 0) return rc;
					treeHeight++;
					char buffer[PageFile::PAGE_SIZE];
					intToChar(treeHeight, buffer);
					intToChar(rootPid, buffer + 4);
					if ((rc = pf.write(0, (const void*) buffer)) != 0) return rc;
				}
				

				return 0;
			}
		}
		return -1;
	}

	// locate child pointer
	BTNonLeafNode node;
	if ((rc = node.read(insertPid, pf)) != 0) return rc;
	PageId childPid;
	if ((rc = node.locateChildPtr(key, childPid)) != 0) return rc;
	if (insertRecursive(key, rid, currHeight+1, midKey, childPid, midPid) == 0 && 
						midKey != -1) {
		// insert into non leaf node to point to new leaf node after split
		int insertKey = midKey;
		midKey = -1;
		rc = node.insert(insertKey, midPid);

		if (rc == 0) {	
			return node.write(insertPid, pf);
		}	
		else if (rc == RC_NODE_FULL) {
			BTNonLeafNode sibling;
			int siblingKey;
			if (node.insertAndSplit(insertKey, midPid, sibling, siblingKey) == 0) {
				if ((rc = node.write(insertPid, pf)) != 0) return rc;
				PageId siblingPid = pf.endPid();
				if (siblingPid == 0) siblingPid = 1;
				if ((rc = sibling.write(siblingPid, pf)) != 0) return rc;
				midKey = siblingKey;
				midPid = siblingPid;

				// check if we need to initializeRoot
				if (currHeight == 1) {
					BTNonLeafNode newroot;
					rc = newroot.initializeRoot(insertPid, midKey, siblingPid);
					rootPid = pf.endPid();
					if ((rc = newroot.write(rootPid, pf)) != 0) return rc;
					treeHeight++;
					char buffer[PageFile::PAGE_SIZE];
					intToChar(treeHeight, buffer);
					intToChar(rootPid, buffer + 4);
					if ((rc = pf.write(0, (const void*) buffer)) != 0) return rc;
				}

				return rc;
			}
		}
		else {
			return rc;
		}
	}
}

/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
	RC rc;
	
	char buffer[PageFile::PAGE_SIZE];
	if ((rc = pf.read(0, (void*) buffer)) != 0) return rc;
	charToInt(buffer, treeHeight);
	charToInt(buffer + 4, rootPid);
	PageId pid = rootPid;

	// find leaf node pid
	int currHeight = 1;
	BTNonLeafNode node;
	
	while(currHeight != treeHeight) {
		if (treeHeight == 1) break;
		if ((rc = node.read(pid, pf)) != 0) return rc;
		if ((rc = node.locateChildPtr(searchKey, pid)) != 0) return rc;
		currHeight++;
	}	

	// search for the key
	// assuming the page has been found and pid is in the pid variable
	cursor.pid = pid;
	cursor.eid = 0;
	int key;
	RecordId rid;
	IndexCursor previous;
	while(1)
	{
		previous = cursor;
		rc = readForward(cursor, key, rid);
		if (cursor.pid < 0) {
			cursor = previous;
			return RC_NO_SUCH_RECORD;
		}
		
		if (rc < 0) return rc;
		if (key >= searchKey) {
			cursor = previous;
			if (key != searchKey)
				return RC_NO_SUCH_RECORD;
			else
				return 0;
		}



	}
	//if ((rc = close()) != 0) return rc;
    return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
	BTLeafNode node;
	RC rc;
	//printf("eid: %i ", cursor.eid);
	if ((rc = node.read(cursor.pid, pf)) < 0) return rc;
	if ((rc = node.readEntry(cursor.eid, key, rid)) < 0) return rc;

	if (cursor.eid != node.getKeyCount() - 1) {
		cursor.eid++;
	} else {
		if (node.getNextNodePtr() != -1) {
			cursor.eid = 0;
			cursor.pid = node.getNextNodePtr();
			printf("\n\n");
		} else {
			//return RC_NO_SUCH_RECORD;
			return RC_END_OF_TREE;
		}
	}
	
    return 0;
}
