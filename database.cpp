#include "database.h"

std::ostream& operator<<(std::ostream& os, const Side& s)
{
  if(s == Side::Buy)
    return os << "BUY";
  else if(s == Side::Sell)
    return os << "SELL";
  else
    return os << "UNKNOWN_SIDE";
}

#define PRINT_VALUE(p) case(p): return os << #p;
std::ostream& operator<<(std::ostream& os, const Dealer& d)
{
  switch(d){
      PRINT_VALUE(BARX);
      PRINT_VALUE(BOFA);
      PRINT_VALUE(CITI);
      PRINT_VALUE(DB);
      PRINT_VALUE(HSBC);
      PRINT_VALUE(JPM);
      PRINT_VALUE(MS);
      PRINT_VALUE(RBC);
      PRINT_VALUE(RBS);
      PRINT_VALUE(UBS);
      PRINT_VALUE(UNKNOWN_DEALER);
  }
}

std::ostream& operator<<(std::ostream& os, const Commodity& c)
{
  switch(c){
      PRINT_VALUE(GOLD);
      PRINT_VALUE(SILV);
      PRINT_VALUE(PORK);
      PRINT_VALUE(OIL);
      PRINT_VALUE(RICE);
      PRINT_VALUE(UNKNOWN_COMMODITY);
  }
}

OrderID Database::create(Side s, Dealer d, Commodity c, int amount, double price)
{
  this->data.push_back(Record{false,s,d,c,amount,price});
  return (int)this->data.size();
}

Record& Database::getByID(OrderID id)
{
  return this->data[id-1];
}

bool Database::containsOrder(OrderID id)
{
  return this->data.size()>=id && id > 0;
}

#define TEST_VALUE(x) if(str==#x) return x;
Dealer Database::parseDealer(std::string str)
{
  TEST_VALUE(BARX);
  TEST_VALUE(BOFA);
  TEST_VALUE(CITI);
  TEST_VALUE(DB);
  TEST_VALUE(HSBC);
  TEST_VALUE(JPM);
  TEST_VALUE(MS);
  TEST_VALUE(RBC);
  TEST_VALUE(RBS);
  TEST_VALUE(UBS);
  return UNKNOWN_DEALER;
}

Side Database::parseSide(std::string str)
{
  if(str=="BUY") return Buy;
  if(str=="SELL") return Sell;
  return UNKNOWN_SIDE;
}

Commodity Database::parseCommodity(std::string str)
{
  TEST_VALUE(PORK);
  TEST_VALUE(GOLD);
  TEST_VALUE(RICE);
  TEST_VALUE(SILV);
  TEST_VALUE(OIL);
  return UNKNOWN_COMMODITY;
}