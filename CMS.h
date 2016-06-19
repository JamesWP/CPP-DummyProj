
#ifndef CMS_H
#define CMS_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include "database.h"
#include "strings.h"
#include "CMSOutput.h"
#include "network.h"

struct AggressDetails
{
  OrderID id; 
  int amount;
};

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
  CMSOutput* validateOrder(Dealer d, OrderID id, std::function<CMSOutput*(Record&)> cont);
  template<class Pred>
  CMSOutput* outputList(Pred);
};


#endif
