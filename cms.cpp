

#include "cms.h"
#include <iostream>
#include <sstream>
#include <string>

std::ostream& operator<<(std::ostream& os, const CMSOutput& o)
{
  os << o.message;
  return os;
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

CMSOutput CMSInterface::aggress(std::vector<AggressDetails> details)
{
  for(const auto& detail:details)
  {
    // order must exist
    if(!this->db.containsOrder(detail.id))
      return CMSOutput("UNKNOWN_ORDER");
    // order must have enough parts to fill the order
    if(this->db.getByID(detail.id).amount<detail.amount)
      return CMSOutput("UNKNOWN_ORDER");
  }
  // trade can be made successfully
  std::stringstream ss;
  bool first = true;
  for(const auto& detail:details)
  {
    Record& r = db.getByID(detail.id);
    r.amount-=detail.amount;
    if(first)first = false;
    else ss << '\n';
    ss << (r.s==Side::Buy?"SOLD ":"BOUGHT ") << detail.amount << " " << r.c << " @ " << r.price << " FROM " << r.d;
  }
  return CMSOutput(ss.str());
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
    }else return CMSOutput("UNAUTHORIZED");
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

/**
 * OrderInfo
 * creates a string to represent the order
 */
std::string CMSOutput::OrderInfo(OrderID id, Dealer d, Side s, Commodity c, int amount, double price)
{
  std::stringstream ss;
  ss << id << " " << d << " " << s << " " << c << " " << amount << " " << price;
  return ss.str();
}

/**
 * OrderInfo
 * creates a string to represent the order in record form
 */
std::string CMSOutput::OrderInfo(OrderID id, Record r)
{
  return CMSOutput::OrderInfo(id, r.d, r.s, r.c, r.amount, r.price);
}

CMSOutput parsePost(CMSInterface& i, Dealer d, std::stringstream& ss)
{
  std::string sideStr;
  std::string commStr;
  int amount;
  double price;
  Side s;
  Commodity c;

  if(!(ss >> sideStr >> commStr >> amount >> price))
    return CMSOutput("INVALID_MESSAGE");

  s = Database::parseSide(sideStr);
  c = Database::parseCommodity(commStr);

  if(s==Side::UNKNOWN_SIDE)
    return CMSOutput("INVALID_MESSAGE");
  if(c==Commodity::UNKNOWN_COMMODITY)
    return CMSOutput("UNKNOWN_COMMODITY");

  return i.post(d, s, c, amount, price);
}

CMSOutput parseRevoke(CMSInterface& i, Dealer d, std::stringstream& ss)
{
  OrderID id;

  if(!(ss >> id))
    return CMSOutput("INVALID_MESSAGE");

  return i.revoke(d, id);
}

CMSOutput parseCheck(CMSInterface& i, Dealer d, std::stringstream& ss)
{
  OrderID id;

  if(!(ss >> id))
    return CMSOutput("INVALID_MESSAGE");

  return i.check(d, id);
}

CMSOutput parseList(CMSInterface& i, std::stringstream& ss)
{
  std::string commStr;
  std::string dealerStr;
  if(!(ss >> commStr))
    return i.list();
  Commodity c = Database::parseCommodity(commStr);
  if(c==UNKNOWN_COMMODITY)
    return CMSOutput("UNKNOWN_COMMODITY");
  if(!(ss >> dealerStr))
    return i.list(c);
  Dealer d = Database::parseDealer(dealerStr);
  if(d==UNKNOWN_DEALER)
    return CMSOutput("UNKOWN_DEALER");
  return i.list(c, d);
}

CMSOutput parseAggress(CMSInterface& i, Dealer d, std::stringstream& ss)
{
  std::vector<AggressDetails> details;
  OrderID id;
  int amount;
  while(ss >> id >> amount)
  {
    details.push_back({id, amount});
  }
  if(details.size()==0)
    return CMSOutput("INVALID_MESSAGE");
  return i.aggress(details);
}

int main(int argc, char* argv[])
{
  CMSInterface i;

  std::string   line;
  while(std::getline(std::cin,line))
  {
    std::stringstream linestr{line};
    std::string dealer;
    Dealer d;
    if(linestr >> dealer)
    {
      if((d = Database::parseDealer(dealer))!=UNKNOWN_DEALER)
      {
        std::string command;
        if(linestr >> command)
        {
          CMSOutput o;
          /**/ if(command=="POST")      o = parsePost(i, d, linestr);
          else if(command=="REVOKE")    o = parseRevoke(i, d, linestr);
          else if(command=="CHECK")     o = parseCheck(i, d, linestr);
          else if(command=="LIST")      o = parseList(i, linestr);
          else if(command=="AGGRESS")   o = parseAggress(i, d, linestr);
          else std::cout << CMSOutput("INVALID_MESSAGE") << std::endl;

          std::cout << o << std::endl;
        }else std::cout << CMSOutput("INVALID_MESSAGE") << std::endl;
      }else std::cout << CMSOutput("UNKNOWN_DEALER") << std::endl;
    }

  }

  return 0;
}
