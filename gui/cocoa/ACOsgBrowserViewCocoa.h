//
//  ACImageBrowserOsgView.h
//  AudioCycle
//
//  @author Stéphane Dupont
//  @date 24/02/09
//  @copyright (c) 2008 – UMONS - Numediart
//  
//  MediaCycle of University of Mons – Numediart institute is 
//  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
//  licence (the “License”); you may not use this file except in compliance 
//  with the License.
//  
//  This program is free software: you can redistribute it and/or 
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//  
//  Each use of this software must be attributed to University of Mons – 
//  Numediart Institute
//  
//  Any other additional authorizations may be asked to avre@umons.ac.be 
//  <mailto:avre@umons.ac.be>
//

#import <Cocoa/Cocoa.h>
#import <Rendering/TiCocoaOsgView.h>
#import "ACOsgBrowserEventHandler.h"
//#import <Rendering/TiCocoaOsgView+Node.h>
#import <MediaCycleLight.h>

struct ACOsgBrowserViewData;

@interface ACOsgBrowserViewCocoa : TiCocoaOsgView {
	struct ACOsgBrowserViewData *_privateData;
	
	MediaCycle				*media_cycle;
	ACOsgBrowserEventHandler *event_handler;

	int mousedown, zoomdown, forwarddown, autoplaydown;
	float refx, refy;
	float refcamx, refcamy;
	float refzoom, refrotation;
	
	BOOL _initialized;
}

- (void)setMediaCycle:(MediaCycle*)_media_cycle;

// needs to be called when images are added or removed
- (void)prepareFromBrowser;

// needs to be called when image positions are changed
- (void)updateTransformsFromBrowser:(double)frac;

@end
