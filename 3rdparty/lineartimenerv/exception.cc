/*Copyright (C) Kristian Nybo, Jarkko Venna
 *
 *This software is released under the GNU Lesser General Public
 *License. See the included file LICENSE for details.*/

#include "exception.hh"

Exception::Exception (std::string errMsg):errMsg (errMsg)
{
}

const
std::string &
Exception::getErrMsg () const
{
  return errMsg;
}
