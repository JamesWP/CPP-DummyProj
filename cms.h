
#ifndef CMS_H
#define CMS_H

#include <vector>
#include <iostream>
#include <string>
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

class CMSOutputTransition: public CMSOutput
{
  std::string message;
  bool _hasMessage;
public:
  CMSOutputTransition(std::string msg):message{msg},_hasMessage{true}{}
  bool hasMessage(){ return _hasMessage;}
  const std::string getMessage() const {return message;}
};

struct AggressDetails
{
  OrderID id; 
  int amount;
};

class CMSInterface
{
  Database db;
public:

  CMSInterface():db{}{}
  CMSOutput* post(Dealer, Side, Commodity, int, double);
  CMSOutput* revoke(Dealer, OrderID);
  CMSOutput* check(Dealer, OrderID);
  CMSOutput* list(Commodity, Dealer);
  CMSOutput* list(Commodity);
  CMSOutput* list();
  CMSOutput* aggress(std::vector<AggressDetails>);

  // Helper functions
private:
  CMSOutput* validateOrder(Dealer d, OrderID id, std::function<CMSOutput*(Record&)> cont);
  template<class Pred>
  CMSOutput* outputList(Pred);
};


#endif
