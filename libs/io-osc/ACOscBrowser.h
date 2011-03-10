/*
 *  ACOscBrowser.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/02/10
 *  Based on Raphael Sebbe's TiCore Osc implementation from 2007.
 *
 *  @copyright (c) 2010 – UMONS - Numediart
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

#ifndef HEADER_ACOSCBROWSER
#define HEADER_ACOSCBROWSER

typedef void *ACOscBrowserRef;
typedef void (*ACOscBrowserCallback)(ACOscBrowserRef, const char *tagName, void *userData);

// TODO : shouldn't this class have only static functions ?
class ACOscBrowser {
	// We need an opaque type as oscpack definition conflicts with Objective-C definitions.
public:
	ACOscBrowser(){};
	~ACOscBrowser(){};
	
	// pass 0 to receive from any host
	ACOscBrowserRef create(const char *hostname, int port);
	void release(ACOscBrowserRef aReceiver);
	
	// Starts a background thread and listens to socket from there
	void start(ACOscBrowserRef aReceiver);
	void stop(ACOscBrowserRef aReceiver);
	
	void setUserData(ACOscBrowserRef aReceiver, void *userData);
	void setCallback(ACOscBrowserRef aReceiver, ACOscBrowserCallback callback);
	
	// these can be called only from the callback
	void readFloat(ACOscBrowserRef aReceiver, float *aVal);
	void readInt(ACOscBrowserRef aReceiver, int *aVal);
	void readString(ACOscBrowserRef aReceiver, char *aval, int maxlen);
};



#endif /* _ACOSCBROWSER_H_ */
