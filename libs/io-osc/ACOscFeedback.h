/*
 *  ACOscFeedback.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 03/04/11
 *
 *  @copyright (c) 2011 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

#ifndef _ACOSCFEEDBACK_H_
#define _ACOSCFEEDBACK_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <lo/lo.h>
#include "Observer.h"
#include <string>

typedef void *ACOscFeedbackRef;

class ACOscFeedback : public Observer{
public:
     ACOscFeedback();
    ~ACOscFeedback();

    void create(const char *hostname, int port);
    void release();
    void messageBegin(const char *tag);
    void messageEnd();
    void messageSend();
    void messageAppendFloat(float val);
    void messageAppendInt(int val);
    void messageAppendString(const char *val);
    void messageAppendString(std::string val);
    void sendMessage(std::string _message);

    lo_address getAddress() {
        return this->sendto;
    }

    bool isActive(){return active;}

    virtual void update(Subject*); // overrides Observer virtual update fct

private:
    lo_address sendto;
    lo_message message;
    const char *tag;
    bool active;
};

#endif /* _ACOSCFEEDBACK_H_ */
