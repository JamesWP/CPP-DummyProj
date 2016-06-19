//
//  CMSInput.hpp
//  CMS
//
//  Created by James Peach on 19/06/2016.
//  Copyright © 2016 James Peach. All rights reserved.
//

#ifndef CMSInput_hpp
#define CMSInput_hpp

#include <string>
#include "CMS.h"


class CMSInterface;

/**
 * handles user input and calls their respective methods
 * on the instance of the interface
 */
class CMSInput
{
public:
  static CMSOutput* parseInput(CMSInterface& i, std::string input);
};

#endif /* CMSInput_hpp */
