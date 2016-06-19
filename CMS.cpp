

#include "CMS.h"

std::ostream& operator<<(std::ostream& os, const CMSOutput& o)
{
  os << o.getMessage();
  return os;
}

CMSOutput* CMSInterface::post(Dealer d, Side s, Commodity c, int amount, double price)
{
  OrderID id = this->db.create(s, d, c, amount, price);
  return new CMSOutputPosted(id, d, s, c, amount, price);
}

CMSOutput* CMSInterface::revoke(Dealer d, OrderID id)
{
  return validateOrder(d, id, [&](Record& r){
    r.revoked = true;
    return new CMSOutputOrderRevoked(id);
  });
}

CMSOutput* CMSInterface::check(Dealer d, OrderID id)
{
  return validateOrder(d, id, [&](Record& r) -> CMSOutput* {
    if(r.amount>0)
    {
      return new CMSOutputOrderCheck(id, r);
    }else return new CMSOutputOrderFilled(id);
  });
}

CMSOutput* CMSInterface::list(Commodity c, Dealer d)
{
  auto pred = [&](const Record& r){ return r.c == c && r.d == d; };
  return outputList(pred);
}

CMSOutput* CMSInterface::list(Commodity c)
{
  auto pred = [&](const Record& r){ return r.c == c; };
  return outputList(pred);
}

CMSOutput* CMSInterface::list()
{
  auto pred = [&](const Record&){ return true; };
  return outputList(pred);
}

CMSOutput* CMSInterface::aggress(std::vector<AggressDetails> details)
{
  for(const auto& detail:details)
  {
    // order must exist
    if(!this->db.containsOrder(detail.id))
      return new CMSOutputUnknownOrder();
    // order must have enough parts to fill the order
    if(this->db.getByID(detail.id).amount<detail.amount)
      return new CMSOutputUnknownOrder();
  }
  CMSOutputOrderDetails* o = new CMSOutputOrderDetails();
  // trade can be made successfully
  for(const auto& detail:details)
  {
    Record& r = db.getByID(detail.id);
    r.amount-=detail.amount;
    o->addOrderDetail(r.s,detail.amount,r.c,r.price,r.d);
  }
  return o;
}

/**
 * validateOrder
 * helper function used to check an order exists for a dealer
 */
CMSOutput* CMSInterface::validateOrder(Dealer d, OrderID id, std::function<CMSOutput*(Record&)> cont)
{
  if(!this->db.containsOrder(id))
    return new CMSOutputUnknownOrder();
  Record& r = this->db.getByID(id);
  if(!r.revoked)
  {
    if(r.d == d)
    {
      return cont(r);
    }else return new CMSOutputUnauthorized();
  }else return new CMSOutputUnknownOrder();
}

/**
 * outputList
 * helper function used to list all orders that pass a predicate filter
 */
template<class Pred>
CMSOutput* CMSInterface::outputList(Pred p)
{
  CMSOutputOrderList* ol = new CMSOutputOrderList();
  auto output = [&](OrderID id, const Record& r){ ol->addOrder(id, r); };

  this->db.itterate(p, output);

  return ol;
}

/**
 * reads lines form stdin and writes the results to stdout
 */
void commandLineInterface()
{
  CMSInterface i;

  std::string line;
  while(std::getline(std::cin, line))
  {
    auto o = CMSInput::parseInput(i, line);
    if(o->hasMessage())
      std::cout << *o << std::endl;
    delete o;
  }
}

/**
 * binds to a socket on port and accepts a single client
 * input is read from the client (\n terminated string)
 * output is writen to the socket
 */
void singleNetworkInterface(char* port)
{
  CMSInterface i;
  NetworkInterface ni(std::atoi(port));

  ni.setup();
  ni.acceptConn();

  while(!ni.closed())
  {
    auto o = CMSInput::parseInput(i, ni.readStr());
    if(o->hasMessage())
      ni.sendStr(o->getMessage());
    delete o;
  }
}

int main(int argc, char* argv[])
{
  if(argc<2)
  {
    std::cout << "cms [\"base\"|\"ext1\" <port>|\"ext2\" <port>]" << std::endl;
    exit(1);
  }
  std::string mode{argv[1]};

  /**/ if(mode.compare("base")==0)
    commandLineInterface();
  else if(mode.compare("ext1")==0 && argc > 2)
    singleNetworkInterface(argv[2]);
  else if(mode.compare("ext2")==0 && argc > 2)
    exit(2); // not yet implemented
  return 0;
}
