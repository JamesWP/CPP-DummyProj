
#ifndef CMS_H
#define CMS_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include "database.h"
#include "strings.h"
#include "CMSOutput.h"
#include "CMSInput.h"
#include "network.h"

struct AggressDetails
{
  OrderID id; 
  int amount;
};


/**
 * Represents the interface to the program
 * functions modify state of the database db and return CMSOutput 
 * to represent the result of the operation
 */
class CMSInterface
{
  Database db;
public:

  CMSInterface():db{}{}
  CMSOutput* post(Dealer, Side, Commodity, int, double);
  CMSOutput* revoke(Dealer, OrderID);
  CMSOutput* check(Dealer, OrderID);
  CMSOutput* list(Commodity, Dealer);
  CMSOutput* list(Commodity);
  CMSOutput* list();
  CMSOutput* aggress(std::vector<AggressDetails>);

  // Helper functions
private:
  /**
   * validates an order can be fulfilled
   */
  CMSOutput* validateOrder(Dealer d, OrderID id, std::function<CMSOutput*(Record&)> cont);

  /**
   * generates an output list for all items in the database that pass a predicate Pred
   */
  template<class Pred>
  CMSOutput* outputList(Pred);
};


#endif
