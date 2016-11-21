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
   	RC   rc;
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
	// if tree is empty
	if (rootPid == -1) {
		BTLeafNode root;
		rootPid = pf.endPid();
		treeHeight++;
		return root.write(rootPid, pf);
	}

	RC rc;
	IndexCursor index;

	if ((rc = locate(key, index)) == RC_NO_SUCH_RECORD) {
		int midKey = -1;
		return this->insertRecursive(key, rid, 1, midKey);
	}
	else {
		// duplicate key value
		return -1;
	}
}

RC BTreeIndex::insertRecursive(int key, const RecordId& rid, int currHeight, int& midKey)
{
	RC rc;
	// if we are at leaf nodes
	if(currHeight == treeHeight)
	{
		BTLeafNode leafnode;
		leafnode.read(rid.pid, pf);
		rc = leafnode.insert(key, rid);

		if (rc == 0) {	
			if ((rc = leafnode.write(rid.pid, pf)) != 0) {
				return rc;
			}
			return 0;
		}	
		else if (rc == RC_NODE_FULL) {
			BTLeafNode sibling;
			sibling.read(leafnode.getNextNodePtr(), pf);
			int siblingKey;
			if (leafnode.insertAndSplit(key, rid, sibling, siblingKey) == 0) {
				if ((rc = leafnode.write(rid.pid, pf)) != 0) {
					return rc;
				}
				if ((rc = sibling.write(pf.endPid(), pf)) != 0) {
					return rc;
				}
				midKey = siblingKey;
				return 0;
			}
		}
		else {
			return rc;
		}
	}

	// insert non leaf node to point to new leaf node
	if (insertRecursive(key, rid, currHeight+1, midKey) == 0 && midKey != -1) {
		int insertKey = midKey;
		midKey = -1;
		BTNonLeafNode node;
		node.read(rid.pid, pf);
		rc = node.insert(insertKey, rid.pid);

		if (rc == 0) {	
			return node.write(rid.pid, pf);
		}	
		else if (rc == RC_NODE_FULL) {
			BTNonLeafNode sibling;
			int siblingKey;
			if (node.insertAndSplit(insertKey, rid.pid, sibling, siblingKey) == 0) {
				if ((rc = node.write(rid.pid, pf)) != 0) {
					return rc;
				}
				PageId siblingPid = pf.endPid();
				if ((rc = sibling.write(siblingPid, pf)) != 0) {
					return rc;
				}
				midKey = siblingKey;

				// check if we need to initializeRoot
				if (currHeight == 1) {
					rc = node.initializeRoot(rid.pid , midKey, siblingPid);
					treeHeight++;
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
	int pid = rootPid;
	// find leaf node pid
	int currHeight = 1;
	BTNonLeafNode node;
	while(currHeight != treeHeight) {
		if ((rc = node.read(pid, pf)) != 0) return rc;
		if ((rc = node.locateChildPtr(searchKey, pid)) != 0) return rc;
		treeHeight++;
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
		if (rc < 0) return rc;
		if (key >= searchKey) {
			cursor = previous;
			if (key != searchKey)
				return RC_NO_SUCH_RECORD;
			else
				return 0;
		}

		if (cursor.pid < 0) {
			cursor = previous;
			return RC_NO_SUCH_RECORD;
		}

	}

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
