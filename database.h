
#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <iostream>

enum Dealer {DB, JPM, UBS, RBC, BARX, MS, CITI, BOFA, RBS, HSBC};
enum Side {Buy, Sell};
enum Commodity {GOLD, SILV, PORK, OIL, RICE};

// NB order id relates to the order in the database
// data[OrderID-1] is the order represented by OrderID
using OrderID = int;

std::ostream& operator<<(std::ostream& os, const Side& s);
std::ostream& operator<<(std::ostream& os, const Dealer& s);
std::ostream& operator<<(std::ostream& os, const Commodity& s);

struct Record
{
  bool revoked;
  Side s;
  Dealer d;
  Commodity c;
  int amount;
  double price;
};

class Database
{
  std::vector<Record> data;
public:
  Database(){}
  OrderID create(Side, Dealer, Commodity, int amount, double price);
  Record& getByID(OrderID);
};

#endif
