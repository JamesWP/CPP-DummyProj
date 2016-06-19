//
//  CMSOutput.cpp
//  CMS
//
//  Created by James Peach on 19/06/2016.
//  Copyright © 2016 James Peach. All rights reserved.
//

#include "CMSOutput.h"


/**
 * addOrderDetail
 * adds detail for a completed order to the OrderDetails
 */
void CMSOutputOrderDetails::addOrderDetail(Side s,int amount, Commodity c, double price, Dealer d)
{
  if(empty) empty = false;
  else ss << '\n';
  ss << (s==Side::Buy?MSG_STR_SOLD:MSG_STR_BOUGHT) << amount << " " << c << " @ " << price << MSG_STR_FROM << d;
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