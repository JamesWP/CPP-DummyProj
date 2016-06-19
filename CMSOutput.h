//
//  CMSOutput.hpp
//  CMS
//
//  Created by James Peach on 19/06/2016.
//  Copyright © 2016 James Peach. All rights reserved.
//

#ifndef CMSOutput_hpp
#define CMSOutput_hpp

#include <iostream>
#include <sstream>
#include "database.h"
#include "strings.h"

class CMSOutput
{
public:
  CMSOutput(){}
  virtual ~CMSOutput(){}

  virtual bool hasMessage() = 0;
  virtual const std::string getMessage() const = 0;

  friend std::ostream& operator<<(std::ostream& os, const CMSOutput& o);

  static std::string OrderInfo(OrderID, Dealer, Side, Commodity, int, double);
  static std::string OrderInfo(OrderID, Record);
};

class CMSOutputNone: public CMSOutput
{
public:
  CMSOutputNone(){}
  bool hasMessage(){return false;}
  const std::string getMessage() const {return NULL;}
};

class CMSOutputUnauthorized: public CMSOutput
{

public:
  CMSOutputUnauthorized(){}
  bool hasMessage(){return true;}
  const std::string getMessage() const { return ERR_STR_UNAUTHORIZED;}
};

class CMSOutputInvalidMessage: public CMSOutput
{

public:
  CMSOutputInvalidMessage(){}
  bool hasMessage(){return true;}
  const std::string getMessage() const { return ERR_STR_INVALID_MESSAGE;}
};


class CMSOutputUnknownDealer: public CMSOutput
{

public:
  CMSOutputUnknownDealer(){}
  bool hasMessage(){return true;}
  const std::string getMessage() const { return ERR_STR_UNKNOWN_DEALER;}
};

class CMSOutputUnknownCommodity: public CMSOutput
{

public:
  CMSOutputUnknownCommodity(){}
  bool hasMessage(){return true;}
  const std::string getMessage() const { return ERR_STR_UNKNOWN_COMMODITY;}
};

class CMSOutputUnknownOrder: public CMSOutput
{

public:
  CMSOutputUnknownOrder(){}
  bool hasMessage(){return true;}
  const std::string getMessage() const { return ERR_STR_UNKNOWN_ORDER;}
};

class CMSOutputOrderRevoked: public CMSOutput
{
  OrderID id;
public:
  CMSOutputOrderRevoked(OrderID id):id{id}{}
  bool hasMessage(){ return true;}
  const std::string getMessage() const {return std::to_string(id) + MSG_STR_REVOKED;}
};

class CMSOutputOrderFilled: public CMSOutput
{
  OrderID id;
public:
  CMSOutputOrderFilled(OrderID id):id{id}{}
  bool hasMessage(){ return true;}
  const std::string getMessage() const {return std::to_string(id) + MSG_STR_FILLED;}
};

class CMSOutputPosted: public CMSOutput
{
  OrderID id;
  Dealer d;
  Side s;
  Commodity c;
  int amount;
  double price;
public:
  CMSOutputPosted(OrderID id, Dealer d, Side s, Commodity c, int amount, double price)
  :id{id},d{d},s{s},c{c},amount{amount},price{price}{}
  bool hasMessage(){return true;}
  const std::string getMessage() const {return CMSOutput::OrderInfo(id, d, s, c, amount, price) + MSG_STR_POSTED;}
};

class CMSOutputOrderDetails: public CMSOutput
{
  std::stringstream ss;
  bool empty;
public:
  CMSOutputOrderDetails():empty{true}{}
  bool hasMessage(){return true;}
  const std::string getMessage() const { return ss.str();}

  void addOrderDetail(Side s,int amount, Commodity c, double price, Dealer d);
};

class CMSOutputOrderList: public CMSOutput
{
  std::stringstream ss;
public:
  CMSOutputOrderList(){}
  bool hasMessage(){return true;}
  const std::string getMessage() const { return ss.str() + MSG_STR_EOL; }

  void addOrder(OrderID id, const Record& r)
  { ss << CMSOutput::OrderInfo(id, r) << '\n'; }

};

class CMSOutputOrderCheck: public CMSOutput
{
  OrderID id;
  const Record& r;
public:
  CMSOutputOrderCheck(OrderID id, const Record& r):id{id},r{r}{}
  bool hasMessage(){return true;}
  const std::string getMessage() const {return CMSOutput::OrderInfo(id,r); }
};

#endif /* CMSOutput_hpp */
