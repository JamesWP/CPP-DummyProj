
#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <iostream>

enum Dealer {DB, JPM, UBS, RBC, BARX, MS, CITI, BOFA, RBS, HSBC, UNKNOWN_DEALER};
enum Side {Buy, Sell, UNKNOWN_SIDE};
enum Commodity {GOLD, SILV, PORK, OIL, RICE, UNKNOWN_COMMODITY};

// NB order id relates to the order in the database
// data[OrderID-1] is the order represented by OrderID
using OrderID = int;

std::ostream& operator<<(std::ostream& os, const Side& s);
std::ostream& operator<<(std::ostream& os, const Dealer& s);
std::ostream& operator<<(std::ostream& os, const Commodity& s);


//Represents an order in the system
struct Record
{
  bool revoked;
  Side s;
  Dealer d;
  Commodity c;
  int amount;
  double price;
};

/**
 * represents a database of orders
 * provides methods to interface with the orders
 */
class Database
{
  std::vector<Record> data;
public:
  Database(){}
  /**
   * creates a new order in the system and returns the new OrderID
   */
  OrderID create(Side, Dealer, Commodity, int amount, double price);
  /**
   * returns a mutable reference to the order in the system
   */
  Record& getByID(OrderID);
  /**
   * itterates over each record that passes Pred and calls Func
   */
  template<typename Pred, typename Func>
  void itterate(Pred p, Func f)
  {
    OrderID id = 1;
    for(const auto& r:this->data)
    {
      if(p(r)) f(id, r);
      id++;
    }
  }
  /**
   * must be called before getByID
   */
  bool containsOrder(OrderID);

  /**
   * helper methods to parse strings to enum types
   */
  static Dealer parseDealer(std::string);
  static Side parseSide(std::string);
  static Commodity parseCommodity(std::string);
};

#endif
