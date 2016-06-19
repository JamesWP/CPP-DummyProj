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
  ss << (s==Side::Buy?"SOLD ":"BOUGHT ") << amount << " " << c << " @ " << price << " FROM " << d;
}
