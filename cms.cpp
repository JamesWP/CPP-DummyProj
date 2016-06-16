

#include "cms.h"
#include <iostream>
#include <sstream>
#include <string>

std::ostream& operator<<(std::ostream& os, const CMSOutput& o)
{
  os << o.message;
  return os;
}

std::string CMSOutput::OrderInfo(OrderID id, Dealer d, Side s, Commodity c, int amount, double price)
{
  std::stringstream ss;
  ss << id << " " << d << " " << s << " " << c << " " << amount << " " << price;
  return ss.str();
}

std::string CMSOutput::OrderInfo(OrderID id, Record r)
{
  return CMSOutput::OrderInfo(id, r.d, r.s, r.c, r.amount, r.price);
}

CMSOutput CMSInterface::post(Dealer d, Side s, Commodity c, int amount, double price)
{
  OrderID id = this->db.create(s, d, c, amount, price);
  return CMSOutput(CMSOutput::OrderInfo(id, d, s, c, amount, price) + " HAS BEEN POSTED");
}

CMSOutput CMSInterface::revoke(Dealer d, OrderID id)
{
  return validateOrder(d, id, [&](Record& r){
    r.revoked = true;
    return CMSOutput(std::to_string(id) + " HAS BEEN REVOKED");
  });
}

CMSOutput CMSInterface::check(Dealer d, OrderID id)
{
  return validateOrder(d, id, [&](Record& r){
    if(r.amount>0)
    {
      return CMSOutput(CMSOutput::OrderInfo(id, r));
    }else return CMSOutput(std::to_string(id) + " HAS BEEN FILLED");
  });
}

CMSOutput CMSInterface::list(Commodity c, Dealer d)
{
  auto pred = [&](const Record& r){ return r.c == c && r.d == d; };
  return outputList(pred);
}

CMSOutput CMSInterface::list(Commodity c)
{
  auto pred = [&](const Record& r){ return r.c == c; };
  return outputList(pred);
}

CMSOutput CMSInterface::list()
{
  auto pred = [&](const Record&){ return true; };
  return outputList(pred);
}

/**
 * validateOrder
 * helper function used to check an order exists for a dealer
 */
CMSOutput CMSInterface::validateOrder(Dealer d, OrderID id, std::function<CMSOutput(Record&)> cont)
{
  if(!this->db.containsOrder(id))
    return CMSOutput("UNKNOWN_ORDER");
  Record& r = this->db.getByID(id);
  if(!r.revoked)
  {
    if(r.d == d)
    {
      return cont(r);
    }else return CMSOutput("UNKNOWN_DEALER");
  }else return CMSOutput("UNKNOWN_ORDER");
}

/**
 * outputList
 * helper function used to list all orders that pass a predicate filter
 */
template<class Pred>
CMSOutput CMSInterface::outputList(Pred p)
{
  std::stringstream ss;

  auto output = [&](OrderID id, const Record& r){ ss << CMSOutput::OrderInfo(id, r) << '\n'; };

  this->db.itterate(p, output);

  ss << "END OF LIST";

  return CMSOutput(ss.str());
}

int main(int argc, char* argv[])
{

  CMSInterface i;

  std::cout << i.post(Dealer::DB, Side::Sell, Commodity::GOLD, 1000000, 1174.6) << std::endl;
  std::cout << i.check(Dealer::DB, 1) << std::endl;
  std::cout << i.revoke(Dealer::DB, 1) << std::endl;
  std::cout << i.check(Dealer::DB, 1) << std::endl;


  std::cout << i.post(Dealer::DB, Side::Sell, Commodity::GOLD, 0, 1174.6) << std::endl;
  std::cout << i.check(Dealer::DB, 2) << std::endl;

  std::cout << i.post(Dealer::BOFA, Side::Sell, Commodity::PORK, 10, 200) << std::endl;
  std::cout << i.check(Dealer::DB, 2) << std::endl;

  std::cout << i.list(Commodity::PORK, Dealer::BOFA) << std::endl;
  std::cout << i.list(Commodity::GOLD) << std::endl;
  std::cout << i.list() << std::endl;
  std::cout << i.list(Commodity::RICE) << std::endl;


  return 0;
}
