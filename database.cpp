#include "database.h"

std::ostream& operator<<(std::ostream& os, const Side& s)
{
  if(s == Side::Buy)
    return os << "BUY";
  else
    return os << "SELL";
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
  }
}

OrderID Database::create(Side s, Dealer d, Commodity c, int amount, double price)
{
  this->data.push_back(Record{false,s,d,c,amount,price});
  return (int)this->data.size();
}

Record& Database::getByID(OrderID id)
{
  //assert(id<=this->data.size());
  return this->data[id-1];
}