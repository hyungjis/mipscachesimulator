/* set.cxx
 * implementaiton of the set class for the cache simulator
 *
 * ECEN 4593, Fall 2010
 * Danny Gale, Chris Messick
 *
 * REVISION HISTORY:
 * 11/8/2010   Danny Gale  created
 *
 */

using namespace std;

// COMPILER INCLUDES
#include <iostream>
#include <iomanip>

// PROJECT INCLUDES
#include "set.h"
#include "cacheline.h"
#include "defines.h"

// CONSTRUCTOR
set::set()
{
   associativity = 1;
}

// DESTRUCTOR
set::~set()
{
   cacheLine * cursor = head;
   cacheLine * temp = head;

   while (cursor != 0)
   {
      temp = cursor;
      cursor = cursor->get_next();
      delete temp;
   }
}

// Inserts a cacheLine pointed to by newLine into the set
// INCREASES SIZE BY 1
/*
void set::add_line()
{
   cacheLine * temp;
   if (size >= associativity)
   {
      cerr << "Set trying to increase beyond associativity. Exiting.";
      exit(ES_SET_SIZE);
   }
   else
   {
      // add to lines
      temp = head;
      head = new cacheLine;
      head->set_next(temp);
      size++;
   }
}
*/

// returns a pointer to a block
cacheLine * set::get_block(unsigned i)
{
   cacheLine * it;
   unsigned j = 0;
   
   if (head == 0)
      cout << "Set is empty" << endl;
   else
   {
      it = head;
      for(j = 0; j < i; j++)
      {
	 it = it->get_next();
	 if (it == 0)
	    return it;
      }
   }
   cout << "returning block # " << j << " from get_block" << endl;
   return it;
}

// resizes the dynamic array
// DELETES THE CONTENTS OF THE SET
void set::set_associativity(unsigned assoc)
{
   cacheLine * cursor = head;
   cacheLine * temp;
   //unsigned i;

   // delete the old array of lines
   while(cursor != 0)
   {
      temp = cursor;
      cursor = cursor->get_next();
      delete temp;
   }

   // update the associativity
   associativity = assoc;
   size = 0;
   head = 0;
}


// returns a cacheLine * if one was evicted
cacheLine * set::update_LRU(cacheLine * mostRecent)
{
#ifdef _DEBUG_SET_UPDATELRU_
   cout << "\t_DEBUG_SET_UPDATELRU_\tmost recent: tag = " << mostRecent->get_tag() << endl;
#endif
   cacheLine * cursor, * oldHead, * retPtr;

#ifdef _DEBUG_SET_UPDATELRU_
   cout << "\t_DEBUG_SET_UPDATELRU_\tchecking set for tag " << mostRecent->get_tag() << endl;
#endif
   // check the set to see if it's currently the head
   if (head == mostRecent)
   {
#ifdef _DEBUG_SET_UPDATELRU_
	 cout << "\t_DEBUG_SET_UPDATELRU_\ttag was found at the head. returning" << endl;
#endif
	    return 0;
   }
   // check the set to see if it's currently elsewhere in the list
#ifdef _DEBUG_SET_UPDATELRU_
   cout << "\t_DEBUG_SET_UPDATELRU_\ttag was NOT found at the head. Searching the rest of the set" << endl;
#endif

   for (cursor = head; cursor != 0; cursor = cursor->get_next())
   {
      if (cursor == mostRecent)
      {	 // found it
#ifdef _DEBUG_SET_UPDATELRU_
	 cout << "\t_DEBUG_SET_UPDATELRU_\ttag has been found in the set not at the head. moving to head and returning" << endl;
#endif
	 // if it's not already the head
	 // move it to the head 
	 oldHead = head;
	 head = cursor->get_next();
	 cursor->set_next(cursor->get_next()->get_next());
	 head->set_next(oldHead);
	 return 0;
      }
   }

#ifdef _DEBUG_SET_UPDATELRU_
   cout << "\t_DEBUG_SET_UPDATELRU_\ttag was NOT found in the set" << endl;
#endif

   // ok, we didn't find it
   // we need to evict the least recently used one
   // IF the set is full
   // which is currently pointed to by cursor

   if (size == associativity)
   {
      retPtr = cursor->get_next();
      cursor->set_next(0);
      mostRecent->set_next(head);
      head = mostRecent;
#ifdef _DEBUG_SET_UPDATELRU_
      cout << "\t_DEBUG_SET_UPDATELRU_\tset is full. evicting least recently used block" << endl;
#endif
   }
   else if (size < associativity)
   {
#ifdef _DEBUG_SET_UPDATELRU_
      cout << "\t_DEBUG_SET_UPDATELRU_\tset is NOT full. adding to head of set" << endl;
#endif
      oldHead = head;
      head = mostRecent;
      mostRecent->set_next(oldHead);
      retPtr = 0;
   }
   else
   {
#ifdef _DEBUG_SET_UPDATELRU_
      cout << "\t_DEBUG_SET_UPDATELRU_\tERROR: SET IS OVERSIZE" << endl;
      exit(ES_LINES_OVERSIZE);
#endif
   }

#ifdef _DEBUG_SET_UPDATELRU_
   cout << "\t_DEBUG_SET_UPDATELRU_\treturning retPtr = " << retPtr << endl;
#endif
   return retPtr;
}


// searches the list of lines for a given tag
// if the tag is found and the valid bit is set,
// a pointer to the block is returned
// otherwise 0 is returned
cacheLine * set::find_line(unsigned t)
{
#ifdef _DEBUG_SET_FINDLINE_
   cout << "\t_DEBUG_SET_FINDLINE_\tlooking for tag " << t << endl;
#endif
   cacheLine * it = 0;

   if (head == 0)
   {
#ifdef _DEBUG_SET_FINDLINE_
      cout << "\t_DEBUG_SET_FINDLINE_\tEmpty set. Returning 0" << endl;
#endif
      return 0;
   }
#ifdef _DEBUG_SET_FINDLINE_
   cout << "\t_DEBUG_SET_FINDLINE_\tset not empty, searching for tag " << t << endl;
#endif

   for (it = head; it != 0; it = it->get_next())
   {
#ifdef _DEBUG_SET_FINDLINE_
      cout << "\t_DEBUG_SET_FINDLINE_\tit = " << it << " tag = " << it->get_tag() << " valid = " << it->get_valid() << endl;
#endif
      if (it->get_tag() == t && it->get_valid() == true)
      {
#ifdef _DEBUG_SET_FINDLINE_
	 cout << "\t_DEBUG_SET_FINDLINE_\treturning it: " << it << endl;
#endif
	 return it;
      }
   }
   return 0;
}


void set::output_blocks()
{
#ifdef _DEBUG_SET_OUTPUTBLOCKS_
   cout << "assoc = " << associativity << ". size = " << size << " ";
#endif
   cacheLine * it;

   for (it = head; it != 0; it = it->get_next())
   {
      cout << setw(8) << hex << it->get_tag() << " ";
   }
}

