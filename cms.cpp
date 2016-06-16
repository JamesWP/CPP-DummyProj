

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

CMSOutput CMSInterface::post(Dealer d, Side s, Commodity c, int amount, double price)
{
  OrderID id = this->db.create(s, d, c, amount, price);
  return CMSOutput(CMSOutput::OrderInfo(id, d, s, c, amount, price) + " HAS BEEN POSTED");
}


int main(int argc, char* argv[])
{

  CMSInterface i;

  CMSOutput o = i.post(Dealer::DB, Side::Sell, Commodity::GOLD, 1000000, 1174.6);
  std::cout << o << std::endl;
  return 0;
}
