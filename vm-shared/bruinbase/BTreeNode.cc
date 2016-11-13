#include "BTreeNode.h"

using namespace std;

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{ 
	if (pid >= 0) { 
		pf.read(pid, (void*)buffer);
		return 0; 
	} else {
		return -1;
	}
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{ 	
	if (pid >= 0) {
		pf.write(pid, (const void*) buffer);
		return 0; 
	} else {
		return -1;
	}
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{ 
	return (int) buffer[PageFile::PAGE_SIZE - 8];
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{ 
	if (getKeyCount() == 84) {
		return -1;
	} else {
		int index = 0;
		locate(key, index);

		for (int i = getKeyCount(); i > index; i--)
		{
			int k = i-1;
			buffer[i * 12 + 8] = buffer[k * 12 + 8];
			buffer[i * 12 + 4] = buffer[k * 12 + 4];
			buffer[i * 12] = buffer[k * 12];
		}

		buffer[index] = rid.pid;
		buffer[index + 4] = rid.sid;
		buffer[index + 8] = key;
		return 0;
	}
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ 
	if (getKeyCount() != 84)
		return -1;

	int middlekey = 0;
	RecordId middlerid;
	readEntry(43, middlekey, middlerid);

	if (middlekey > key) {
		for (int i = 0; i < 42 * 12; i += 4)
		{
			sibling.buffer[i] = (int) buffer[i + 42 * 12];
		}

		buffer[PageFile::PAGE_SIZE - 8] = 43;
		sibling.buffer[PageFile::PAGE_SIZE - 8] = 42;
		insert(key, rid);

		siblingKey = (int) sibling.buffer[0];
		return 0;

	} else {
		for (int i = 0; i < 42 * 12; i += 4)
		{
			sibling.buffer[i] = (int) buffer[i + 43 * 12];
		}

		buffer[PageFile::PAGE_SIZE - 8] = 43;
		sibling.buffer[PageFile::PAGE_SIZE - 8] = 42;
		sibling.insert(key, rid);

		siblingKey = (int) sibling.buffer[0];
		return 0;
	}
}

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{ 
	/**
	 * Search through the node sequentially
	 */
	int limit = buffer[PageFile::PAGE_SIZE - 8];
	for (int i = 0; i < limit; i++)
	{
		if (buffer[i * 12 + 8] == searchKey) {
			eid = i;
			return 0;
		}

		if (buffer[i * 12 + 8] > searchKey) {
			eid = i - 1;
			return RC_NO_SUCH_RECORD;
		}
	}
	return -1;
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{ 
	if (eid > getKeyCount()) {
		return -1;
	} else {
		void* pointer = buffer + 12 * eid;
		rid = *(RecordId*) pointer; 
		//rid = (RecordId) buffer[12 * eid];

		key = (int) buffer[12 * eid + 8];
		return 0;
	}

}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ 
	return (PageId) buffer[PageFile::PAGE_SIZE - 4]; 
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ 
	if (pid >= 0) {
		buffer[PageFile::PAGE_SIZE - 4] = pid;
		return 0; 
	} else {
		return -1;
	}
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ return 0; }
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{ return 0; }

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ return 0; }


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ return 0; }

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{ return 0; }

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ return 0; }

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ return 0; }
