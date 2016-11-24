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
		PageId pid = pf.endPid();
		if (pid == 0) pid = 1;
		if ((rc = leaf.write(pid, pf)) != 0) return rc;

		BTNonLeafNode root;
		if ((rc = root.initializeRoot(-1, key, pid)) != 0) return rc;
		rootPid = pf.endPid();
		if (rootPid == 0) rootPid = 1;
		if ((rc = root.write(rootPid, pf)) != 0) return rc;

		treeHeight = 2;
		char buffer[PageFile::PAGE_SIZE];
		buffer[0] = treeHeight;
		buffer[4] = rootPid;
		if ((rc = pf.write(0, (const void*) buffer)) != 0) return rc;
		//if ((rc = close()) != 0) return rc;
		return 0;
	}

	char buffer[PageFile::PAGE_SIZE];
	if ((rc = pf.read(0, (void*) buffer)) != 0) return rc;
	treeHeight = (int) buffer[0];
	rootPid = (int) buffer[4];

	IndexCursor cursor;
	if ((rc = locate(key, cursor)) == RC_NO_SUCH_RECORD) {
		int midKey = -1;
		if ((rc = this->insertRecursive(key, rid, 1, midKey, rootPid)) != 0) return rc;
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
	int& midKey, PageId insertPid)
{
	int key2;
	RecordId rid2;

	RC rc;
	char buffer[PageFile::PAGE_SIZE];
	if ((rc = pf.read(0, (void*) buffer)) != 0) return rc;
	treeHeight = (int) buffer[0];



	// if we are at leaf nodes
	if(currHeight == treeHeight)
	{
		BTLeafNode leafnode;
		leafnode.read(insertPid, pf);

		int keyCount = leafnode.getKeyCount();
		if (keyCount < 84 && keyCount >= 0) {	
			if ((rc = leafnode.insert(key, rid)) != 0) return rc;
			if ((rc = leafnode.write(insertPid, pf)) != 0) return rc;

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
	if (insertRecursive(key, rid, currHeight+1, midKey, childPid) == 0 && 
						midKey != -1) {
		// insert non leaf node to point to new leaf node
		int insertKey = midKey;
		midKey = -1;
		rc = node.insert(midKey, insertPid);

		if (rc == 0) {	
			return node.write(insertPid, pf);
		}	
		else if (rc == RC_NODE_FULL) {
			BTNonLeafNode sibling;
			int siblingKey;
			if (node.insertAndSplit(insertKey, insertPid, sibling, siblingKey) == 0) {
				if ((rc = node.write(insertPid, pf)) != 0) return rc;
				PageId siblingPid = pf.endPid();
				if (siblingPid == 0) siblingPid = 1;
				if ((rc = sibling.write(siblingPid, pf)) != 0) return rc;
				midKey = siblingKey;

				// check if we need to initializeRoot
				if (currHeight == 1) {
					BTNonLeafNode newroot;
					rc = newroot.initializeRoot(insertPid, midKey, siblingPid);
					rootPid = pf.endPid();
					if ((rc = newroot.write(rootPid, pf)) != 0) return rc;
					treeHeight++;
					char buffer[PageFile::PAGE_SIZE];
					buffer[0] = (int) treeHeight;
					buffer[4] = (int) rootPid;
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
	//if ((rc = open("pagefile.txt", 'w')) != 0) return rc;
	char buffer[PageFile::PAGE_SIZE];
	if ((rc = pf.read(0, (void*) buffer)) != 0) return rc;
	treeHeight = (int) buffer[0];
	rootPid = (int) buffer[4];
	PageId pid = rootPid;

	// find leaf node pid
	int currHeight = 1;
	BTNonLeafNode node;
	
	while(currHeight != treeHeight) {
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

	if ((rc = node.read(cursor.pid, pf)) < 0) return rc;
	if ((rc = node.readEntry(cursor.eid, key, rid)) < 0) return rc;

	if (cursor.eid != 84) {
		cursor.eid++;
	} else {
		cursor.eid = 0;
		cursor.pid = node.getNextNodePtr();
	}

    return 0;
}
