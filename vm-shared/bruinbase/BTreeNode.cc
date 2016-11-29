#include "BTreeNode.h"
#include <bitset>
#include <cstdio>

using namespace std;

RC intToChar(int n, char* buffer)
{
	buffer[0] = n & (unsigned) 255;
	buffer[1] = (n >> 8) & (unsigned) 255;
	buffer[2] = (n >> 16) & (unsigned) 255;
	buffer[3] = (n >> 24) & (unsigned) 255;

//	printf("%i %i %i %i\n", buffer[0], buffer[1], buffer[2], buffer[3]);

	return 0;
}

RC charToInt(char* buffer, int& n)
{
	int x = 0;

	x |= (unsigned char) buffer[3];
	x = x << 8;
	x |= (unsigned char) buffer[2];
	x = x << 8;
	x = x | (unsigned char) buffer[1];
	x = x << 8;
	x = x | (unsigned char) buffer[0];	

	n = x;
	
//	printf("%i\n", x);

	return 0;
}

BTLeafNode::BTLeafNode()
{
	for (int i = 0; i < 1024; i++)
	{
		buffer[i] = 0;
	}

//	buffer[PageFile::PAGE_SIZE - 4] = -1;
	int k = -1;
	intToChar(k, buffer + PageFile::PAGE_SIZE - 4);

}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{ 
	if (pid >= 0) { 
		return pf.read(pid, (void*)buffer);
	} else {
		return RC_FILE_READ_FAILED;
	}
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.wdr
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{ 	
	if (pid >= 0) {
		return pf.write(pid, (const void*) buffer);
	} else {
		return RC_FILE_READ_FAILED;
	}
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{ 
//	return (int) buffer[PageFile::PAGE_SIZE - 8];
	int k;
	charToInt(buffer + PageFile::PAGE_SIZE - 8, k);
	return k;
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
		return RC_NODE_FULL;
	} else {
		int index = 0;
		int result = locate(key, index);


		if (getKeyCount() != 0)
			index++;

		for (int i = getKeyCount(); i > index; i--)
		{
			int k = i-1;

			for (int j = 0; j < 12; j++)
			{
				buffer[i * 12 + j] = buffer[k * 12 + j];
			}
/*
			buffer[i * 12 + 8] = (int) buffer[k * 12 + 8];
			buffer[i * 12 + 4] = (int) buffer[k * 12 + 4];
			buffer[i * 12] = (int) buffer[k * 12];
*/
		}

		RecordId rid2 = rid;

//		buffer[index * 12] = rid.pid;
		intToChar(rid2.pid, buffer + index * 12);
//		buffer[index * 12 + 4] = rid.sid;
		intToChar(rid2.sid, buffer + index * 12 + 4);
//		buffer[index * 12 + 8] = key;
		intToChar(key, buffer + index * 12 + 8);
		
		int keys;
		charToInt(buffer + PageFile::PAGE_SIZE - 8, keys);
		keys++;
		intToChar(keys, buffer + PageFile::PAGE_SIZE - 8);
//		buffer[PageFile::PAGE_SIZE - 8]++;


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
	readEntry(42, middlekey, middlerid);

	int temp;

	if (middlekey > key) {
		for (int i = 0; i < 42 * 12; i += 4)
		{
//			sibling.buffer[i] = (int) buffer[i + 42 * 12];
			charToInt(buffer + i + 42 * 12, temp);
			intToChar(temp, sibling.buffer + i);
		}

//		buffer[PageFile::PAGE_SIZE - 8] = 42;
		intToChar(42, buffer + PageFile::PAGE_SIZE - 8);
//		sibling.buffer[PageFile::PAGE_SIZE - 8] = 42;
		intToChar(42, sibling.buffer + PageFile::PAGE_SIZE - 8);
		insert(key, rid);

//		siblingKey = (int) sibling.buffer[0];
		charToInt(sibling.buffer, siblingKey);
		return 0;

	} else {
		for (int i = 0; i < 41 * 12; i += 4)
		{
//			sibling.buffer[i] = (int) buffer[i + 43 * 12];
			charToInt(buffer + i + 43 * 12, temp);
			intToChar(temp, sibling.buffer + i);
		}

//		buffer[PageFile::PAGE_SIZE - 8] = 43;
//		sibling.buffer[PageFile::PAGE_SIZE - 8] = 41;
		intToChar(43, buffer + PageFile::PAGE_SIZE - 8);
		intToChar(41, sibling.buffer + PageFile::PAGE_SIZE - 8);
		sibling.insert(key, rid);

//		siblingKey = (int) sibling.buffer[0];
		charToInt(sibling.buffer + 8, siblingKey);
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
//	int limit = (int) buffer[PageFile::PAGE_SIZE - 8];

	int limit;
	charToInt(buffer + PageFile::PAGE_SIZE - 8, limit);

	if (limit == 0)
	{
		eid = 0;
		return 0;
	}
	for (int i = 0; i < limit; i++)
	{

		int curr;
		charToInt(buffer + i * 12 + 8, curr);

		if (curr == searchKey) {
			eid = i;
			return 0;
		}

		if (curr > searchKey) {
			eid = i - 1;

			return RC_NO_SUCH_RECORD;
		}
	}

	eid = limit - 1;

	return RC_NO_SUCH_RECORD;
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
		return RC_NO_SUCH_RECORD;
	} else {
		char* pointer = buffer + 12 * eid;
//		rid = *(RecordId*) pointer; 
		//rid = (RecordId) buffer[12 * eid];

//		key = (int) buffer[12 * eid + 8];
		charToInt(pointer, rid.pid);
		charToInt(pointer + 4, rid.sid);
		charToInt(pointer + 8, key);
/*
		bitset<32> temp;

		char c;
		for (int k = 0; k < 4; k++)
		{
			c = buffer[12 * eid + k];
			string x = bitset<8>((int)c).to_string();

			for (int j = 8; j > 0; j--)
			{
				if (x[j] == '1')
					temp.set(k * 8 + (8 - j));
			}
		}

		key = (int) temp.to_ulong();
*/
		return 0;
	}

}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ 
	PageId temp;
	charToInt(buffer + PageFile::PAGE_SIZE - 4, temp);
	return temp;
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ 
	if (pid >= 0) {
		intToChar(pid, buffer + PageFile::PAGE_SIZE - 4);
//		buffer[PageFile::PAGE_SIZE - 4] = pid;
		return 0; 
	} else {
		return -1;
	}
}

BTNonLeafNode::BTNonLeafNode()
{ 
	for (int i = 0; i < 1024; i++)
	{
		buffer[i] = 0;
	}

	intToChar(0, buffer + PageFile::PAGE_SIZE - 8);
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ 
	if (pid >= 0) { 
		return pf.read(pid, (void*)buffer);
	} else {
		return RC_INVALID_PID;
	}	
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{
	if (pid >= 0) {
		return pf.write(pid, (const void*) buffer);
	} else {
		return RC_INVALID_PID;
	}
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ 	
	int keys;
	charToInt(buffer + PageFile::PAGE_SIZE - 8, keys);	
//	return (int) buffer[PageFile::PAGE_SIZE - 8];
	return keys;
}

/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ 
	if (getKeyCount() == 84) {
		return RC_NODE_FULL;
	}
	else if (getKeyCount() > 84) {
		return -1;
	}
	else {
		int k = getKeyCount() * 8;
		int limit;
		charToInt(buffer + k - 8, limit);

		int temp;
		for (int i = k; i != 0 && limit > key; i -= 8)
		{
			k = i-8;
//			buffer[i + 4] = (PageId) buffer[k + 4];
			charToInt(buffer + k + 4, temp);
			intToChar(temp, buffer + i + 4);
//			buffer[i] = (int) buffer[k];
			charToInt(buffer + k, temp);
			intToChar(temp, buffer + i);

			charToInt(buffer + i - 8, limit);
		}

//		buffer[k] = key;
		intToChar(key, buffer + k);
//		buffer[k + 4] = pid;
		intToChar(pid, buffer + k + 4);
//		buffer[PageFile::PAGE_SIZE - 8]++;
		charToInt(buffer + PageFile::PAGE_SIZE - 8, temp);
		intToChar(temp + 1, buffer + PageFile::PAGE_SIZE - 8);

		return 0;
	}
}

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
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, 
									int& midKey)
{
	if (getKeyCount() != 84) {
		return -1;
	}

	int a, b;
	charToInt(buffer + 42 * 8, a);
	charToInt(buffer + 41 * 8, b);
	int temp;

	// key to insert is the midKey
	if (key <= a && key >= b) {
		midKey = key;
//		sibling.buffer[PageFile::PAGE_SIZE - 4] = pid;
		intToChar(pid, sibling.buffer + PageFile::PAGE_SIZE - 4);


	    for (int i = 0; i < 42 * 8; i += 4)
	    {
	    	charToInt(buffer + i + 42 * 8, temp);
	    	intToChar(temp, sibling.buffer + i);
//	        sibling.buffer[i] = (int) buffer[i + 42 * 8];
	    }

//	    buffer[PageFile::PAGE_SIZE - 8] = 42;
//	    sibling.buffer[PageFile::PAGE_SIZE - 8] = 42;
	    intToChar(42, buffer + PageFile::PAGE_SIZE - 8);
	    intToChar(42, buffer + PageFile::PAGE_SIZE - 8);
	} 
	else if (key > a) {
//		midKey = (int) buffer[42 * 8];
		midKey = a;

		charToInt(buffer + 42 * 8 + 4, temp);
		intToChar(temp, sibling.buffer + PageFile::PAGE_SIZE - 4);
//		sibling.buffer[PageFile::PAGE_SIZE - 4] = (int) buffer[42 * 8 + 4];

	    for (int i = 0; i < 41 * 8; i += 4)
	    {
//	        sibling.buffer[i] = (int) buffer[i + 43 * 8];
			charToInt(buffer + i + 43 * 8, temp);
	    	intToChar(temp, sibling.buffer + i);
	    }

//	    buffer[PageFile::PAGE_SIZE - 8] = 42;
	    intToChar(42, buffer + PageFile::PAGE_SIZE - 8);
//	    sibling.buffer[PageFile::PAGE_SIZE - 8] = 41;
	    intToChar(41, sibling.buffer + PageFile::PAGE_SIZE - 8);
	    sibling.insert(key, pid);
	}
	else {
//		midKey = (int) buffer[41 * 8];
		midKey = b;
//		sibling.buffer[PageFile::PAGE_SIZE - 4] = (int) buffer[41 * 8 + 4];

		charToInt(buffer + 41 * 8 + 4, temp);
		intToChar(temp, sibling.buffer + PageFile::PAGE_SIZE - 4);

	    for (int i = 0; i < 42 * 8; i += 4)
	    {
//	        sibling.buffer[i] = (int) buffer[i + 42 * 8];
			charToInt(buffer + i + 42 * 8, temp);
			intToChar(temp, sibling.buffer + i);
	    }

//	    buffer[PageFile::PAGE_SIZE - 8] = 41;
//	    sibling.buffer[PageFile::PAGE_SIZE - 8] = 42;
	    intToChar(41, buffer + PageFile::PAGE_SIZE - 8);
	    intToChar(42, sibling.buffer + PageFile::PAGE_SIZE - 8);
	    insert(key, pid);
	}

    return 0;
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ 
	int temp;
	for (int i = 0; i < getKeyCount(); i++) {
		charToInt(buffer + i * 8, temp);
		printf("key %i: %i\n", i, temp);
	}



	int key;
	charToInt(buffer, key);
	if (searchKey < key) {
//		pid = (int) buffer[PageFile::PAGE_SIZE - 4];
		charToInt(buffer + PageFile::PAGE_SIZE - 4, pid);
		printf("searchKey < key %i %i\n", searchKey, key);
		return 0;
	}

//	int limit = (int) buffer[PageFile::PAGE_SIZE - 8] - 1;
	int limit = getKeyCount() - 1;
	/*
	charToInt(buffer + PageFile::PAGE_SIZE - 8, limit);
	limit--;
	*/

	for (int i = 0; i < limit; i++)
	{
		int x, y;
		charToInt(buffer + i * 8, x);
		charToInt(buffer + i * 8 + 8, y);
		if (x == searchKey || (x < searchKey && y > searchKey)) {
//			pid = (int) buffer[i * 8 + 4];
			charToInt(buffer + i * 8 + 4, pid);
			printf("x < searchKey < y %i %i %i\n", x, searchKey, y);
			return 0;
		}
	}

//	pid = (int) buffer[limit * 8 + 4];
	charToInt(buffer + limit * 8 + 4, pid);
	printf("searchKey > all %i\n", searchKey);
	return 0;
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ 
//	buffer[0] = key;
	intToChar(key, buffer);
//	buffer[4] = pid2;
	intToChar(pid2, buffer + 4);
//	buffer[PageFile::PAGE_SIZE - 4] = pid1;
	intToChar(pid1, buffer + PageFile::PAGE_SIZE - 4);
//	buffer[PageFile::PAGE_SIZE - 8] = 1;
	intToChar(1, buffer + PageFile::PAGE_SIZE - 8);
	return 0; 
}
