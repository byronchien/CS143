/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "BTreeIndex.h"
#include "SqlEngine.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning

  RC     rc;
  int    key;     
  string value;
  int    count;
  int    diff;

  // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

  string index_name = table + ".idx";

  BTreeIndex index;
  rc = index.open(index_name, 'r');
  count = 0;
  if (rc < 0) {
    // index doesn't exist; search normally
    // scan the table file from the beginning
    rid.pid = rid.sid = 0;
    count = 0;
    while (rid < rf.endRid()) {
      // read the tuple
      if ((rc = rf.read(rid, key, value)) < 0) {
        fprintf(stderr, "Error2: while reading a tuple from table %s\n", table.c_str());
        goto exit_select;
      }

      // check the conditions on the tuple
      for (unsigned i = 0; i < cond.size(); i++) {
        // compute the difference between the tuple value and the condition value
        switch (cond[i].attr) {
          case 1:
            diff = key - atoi(cond[i].value);
            break;
          case 2:
            diff = strcmp(value.c_str(), cond[i].value);
            break;
        }

        // skip the tuple if any condition is not met
        switch (cond[i].comp) {
          case SelCond::EQ:
            if (diff != 0) goto next_tuple;
            break;
          case SelCond::NE:
            if (diff == 0) goto next_tuple;
            break;
          case SelCond::GT:
            if (diff <= 0) goto next_tuple;
            break;
          case SelCond::LT:
            if (diff >= 0) goto next_tuple;
            break;
          case SelCond::GE:
            if (diff < 0) goto next_tuple;
            break;
          case SelCond::LE:
            if (diff > 0) goto next_tuple;
            break;
        }
      }

      // the condition is met for the tuple. 
      // increase matching tuple counter
      count++;

      // print the tuple 
      switch (attr) {
        case 1:  // SELECT key
          fprintf(stdout, "%d\n", key);
          break;
        case 2:  // SELECT value
          fprintf(stdout, "%s\n", value.c_str());
          break;
        case 3:  // SELECT *
          fprintf(stdout, "%d '%s'\n", key, value.c_str());
          break;
      }

      // move to the next tuple
      next_tuple:
      ++rid;
    }

    // print matching tuple count if "select count(*)"
    if (attr == 4) {
      fprintf(stdout, "%d\n", count);
    }
    rc = 0;

    // close the table file and return
    exit_select:
    rf.close();
    return rc;
  } else {
    // index does exist;

    // separate the conditions into key conditions and value conditions
    vector<SelCond> key_cond;
    vector<SelCond> value_cond;

    for (int k = 0; k < cond.size(); k++) {
      switch(cond[k].attr) {
        case 1:
          key_cond.push_back(cond[k]);
          break;
        case 2:
          value_cond.push_back(cond[k]);
          break;
        default:
          printf("Error: Unknown attribute in SelCond\n");
          return -1;
          break;
      }
    }

    // set index search range based off of key condition
    int lower_limit = 0;
    int upper_limit = 2147483647;
    bool equal_check = false;
    int equal_num = 0;

    for (int k = 0; k < key_cond.size(); k++) {
      switch(key_cond[k].comp) {
        case SelCond::EQ:
          if (equal_check) {
            if (atoi(key_cond[k].value) == equal_num)
              continue;
            else
            {
              if (attr == 4) {
                fprintf(stdout, "0\n");
              }
              return -1;
            }
              // error; two different key equality conditions
          } else {
            equal_check = true;
            equal_num = atoi(key_cond[k].value);
          }
          break;
        case SelCond::NE:
          value_cond.push_back(key_cond[k]);
          break;
        case SelCond::GT:
          lower_limit = (atoi(key_cond[k].value) > lower_limit) ? atoi(key_cond[k].value) + 1
            : lower_limit;
          break;
        case SelCond::LT:
          upper_limit = (atoi(key_cond[k].value) < upper_limit) ? atoi(key_cond[k].value) + 1
            : upper_limit;
          break;
        case SelCond::GE:
          lower_limit = (atoi(key_cond[k].value) > lower_limit) ? atoi(key_cond[k].value)
            : lower_limit;
          break;
        case SelCond::LE:
          upper_limit = (atoi(key_cond[k].value) < upper_limit) ? atoi(key_cond[k].value)
            : upper_limit;
          break;
      }
    }

    if (upper_limit < lower_limit) {
      if (attr == 4) {
          fprintf(stdout, "0\n");
      }
      return -1;
    }
      // error; upper_limit is less than the lower limit


    IndexCursor cursor;
    index.locate(lower_limit, cursor);  
    //printf("%i %i %i\n", cursor.eid, cursor.pid, lower_limit);

    index.readForward(cursor, key, rid);
    //printf("%i %i %i %i %i\n", cursor.eid, cursor.pid, key,
    //        rid.pid, rid.sid);

    IndexCursor limit_cursor;
    index.locate(upper_limit, limit_cursor);
    //printf("%i %i %i\n", limit_cursor.eid, limit_cursor.pid, upper_limit);
    int limit_key;
    RecordId limit_rid;
    index.readForward(limit_cursor, limit_key, limit_rid);
    //printf("%i %i %i %i %i\n", limit_cursor.eid, limit_cursor.pid, limit_key, 
    //        limit_rid.pid, limit_rid.sid);

    if (equal_check) {
      index.locate(equal_num, cursor);
      index.readForward(cursor, key, rid);

      index.locate(equal_num + 1, limit_cursor);
      index.readForward(limit_cursor, limit_key, limit_rid);
    }



    while(key != limit_key)
    {
      // read the tuple
      if ((rc = rf.read(rid, key, value)) < 0) {
        fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
        goto exit_select2;
      }

      // check the conditions on the tuple
      for (unsigned i = 0; i < value_cond.size(); i++) {
        // compute the difference between the tuple value and the condition value
        switch (value_cond[i].attr) {
          case 1:
            diff = key - atoi(value_cond[i].value);
            break;
          case 2:
            diff = strcmp(value.c_str(), value_cond[i].value);
            break;
        }

        // skip the tuple if any condition is not met
        switch (value_cond[i].comp) {
          case SelCond::EQ:
            if (diff != 0) goto next_tuple2;
            break;
          case SelCond::NE:
            if (diff == 0) goto next_tuple2;
            break;
          case SelCond::GT:
            if (diff <= 0) goto next_tuple2;
            break;
          case SelCond::LT:
            if (diff >= 0) goto next_tuple2;
            break;
          case SelCond::GE:
            if (diff < 0) goto next_tuple2;
            break;
          case SelCond::LE:
            if (diff > 0) goto next_tuple2;
            break;
        }
      }

      // the condition is met for the tuple. 
      // increase matching tuple counter
      count++;

      // print the tuple 
      switch (attr) {
        case 1:  // SELECT key
          fprintf(stdout, "%d\n", key);
          break;
        case 2:  // SELECT value
          fprintf(stdout, "%s\n", value.c_str());
          break;
        case 3:  // SELECT *
          fprintf(stdout, "%d '%s'\n", key, value.c_str());
          break;
      }

      // move to the next tuple
      next_tuple2:
      rc = index.readForward(cursor, key, rid);
      if (rc < 0) break;
    }

    // print matching tuple count if "select count(*)"
    if (attr == 4) {
      fprintf(stdout, "%d\n", count);
    }
    rc = 0;

    // close the table file and return
    exit_select2:
    rf.close();
    return rc;
  }
/*
    printf("Key conditions: %i\n", key_cond.size());
    printf("Value conditions: %i\n", value_cond.size());
*/
    return 0;

}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
  /* Read loadfile with fstream, fgets, etc */
  RC rc;
  string line;
  ifstream fs;
  fs.open(loadfile.c_str(), ios::in);
  if(fs.is_open()) {

    string filename = table + ".tbl";
    RecordFile recordfile(filename, 'w');

    BTreeIndex btreeindex;
    if (index) {
      string indexFilename = table + ".idx";     
      if ((rc = btreeindex.open(indexFilename, 'w')) < 0) {
        fprintf(stderr, "Error: while opening BTreeIndex pagefile\n");
        goto exit_select;
      }
    }

    while (getline (fs, line)) {
      int key;
      string value;
      RecordId recordID;
      parseLoadLine(line, key, value);
      recordfile.append(key, value, recordID);
      
      if (index) {
        if ((rc = btreeindex.insert(key, recordID)) < 0) {
          fprintf(stderr, "Error: while inserting inserting key and recordID pair into BTreeIndex pagefile\n");
          btreeindex.close();
          goto exit_select;         
        }
      }
      
    }

    if (index) {
      if ((rc = btreeindex.close()) < 0) {
        fprintf(stderr, "Error: while closing BTreeIndex pagefile\n");
        goto exit_select;       
      }
    }

    fs.close();
  }
  else {
    cout << "Unable to open file" << endl;
  }
  


  /* if file does not yet exist create a RecordFile in the current working 
      directory and store all tuples in the file. */

  /* Open the input RecordFile */

  /* Parse each line of the loadfile to read a tuple using 
      SqlEngine::parseLoadLine(), and insert the tuple to the Record File */

  return 0;

  // close the table file and return
  exit_select:
  fs.close();
  return rc;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}
