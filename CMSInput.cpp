//
//  CMSInput.cpp
//  CMS
//
//  Created by James Peach on 19/06/2016.
//  Copyright © 2016 James Peach. All rights reserved.
//

#include "CMSInput.h"



/**
 * parse(Post|Revoke|Check|List|Aggress) methods
 * each takes the interface to act upon, the dealer(if required), and the
 * string stream to take remaining input from
 */

CMSOutput* parsePost(CMSInterface& i, Dealer d, std::stringstream& ss)
{
  std::string sideStr;
  std::string commStr;
  int amount;
  double price;
  Side s;
  Commodity c;

  if(!(ss >> sideStr >> commStr >> amount >> price))
    return new CMSOutputInvalidMessage();

  s = Database::parseSide(sideStr);
  c = Database::parseCommodity(commStr);

  if(s==Side::UNKNOWN_SIDE)
    return new CMSOutputInvalidMessage();
  if(c==Commodity::UNKNOWN_COMMODITY)
    return new CMSOutputUnknownCommodity();

  return i.post(d, s, c, amount, price);
}

CMSOutput* parseRevoke(CMSInterface& i, Dealer d, std::stringstream& ss)
{
  OrderID id;

  if(!(ss >> id))
    return new CMSOutputInvalidMessage();

  return i.revoke(d, id);
}

CMSOutput* parseCheck(CMSInterface& i, Dealer d, std::stringstream& ss)
{
  OrderID id;

  if(!(ss >> id))
    return new CMSOutputInvalidMessage();

  return i.check(d, id);
}

CMSOutput* parseList(CMSInterface& i, std::stringstream& ss)
{
  std::string commStr;
  if(!(ss >> commStr))
    return i.list();
  Commodity c = Database::parseCommodity(commStr);
  if(c==UNKNOWN_COMMODITY)
    return new CMSOutputUnknownCommodity();
  std::string dealerStr;
  if(!(ss >> dealerStr))
    return i.list(c);
  Dealer d = Database::parseDealer(dealerStr);
  if(d==UNKNOWN_DEALER)
    return new CMSOutputUnknownDealer();
  return i.list(c, d);
}

CMSOutput* parseAggress(CMSInterface& i, Dealer d, std::stringstream& ss)
{
  std::vector<AggressDetails> details;
  OrderID id;
  int amount;
  while(ss >> id >> amount)
  {
    details.push_back({id, amount});
  }
  if(details.size()==0)
    return new CMSOutputInvalidMessage();
  return i.aggress(details);
}

/**
 * parseInput
 * top level parse function, takes a command string and computes the output
 */
CMSOutput* CMSInput::parseInput(CMSInterface& i, std::string input)
{
  std::stringstream linestr{input};
  std::string dealer;

  if(!(linestr >> dealer)) return new CMSOutputNone();

  Dealer d = Database::parseDealer(dealer);
  if(d==UNKNOWN_DEALER)
    return new CMSOutputUnknownDealer();

  std::string command;
  if(!(linestr >> command))
    return new CMSOutputInvalidMessage();

  /**/ if(command=="POST")      return parsePost(i, d, linestr);
  else if(command=="REVOKE")    return parseRevoke(i, d, linestr);
  else if(command=="CHECK")     return parseCheck(i, d, linestr);
  else if(command=="LIST")      return parseList(i, linestr);
  else if(command=="AGGRESS")   return parseAggress(i, d, linestr);
  else return new CMSOutputInvalidMessage();
}
