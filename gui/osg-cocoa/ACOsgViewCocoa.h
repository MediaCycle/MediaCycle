//
//  ACOsgViewCocoa.h
//  MediaCycle
//
//  @author Christian Frisson
//  @date 14/10/10
//  @copyright (c) 2010 – UMONS - Numediart
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
//  Forked from TiCore by Raphael Sebbe.
//  @copyright (c) 2007 – UMONS - Numediart
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
#import <CoreVideo/CoreVideo.h>

// Opaque structure for C++ implementation
struct ACOsgViewCocoaData;

@class ACOsgViewCocoa;
// Do not expect these to be in the main thread...
@interface NSObject (ACOsgViewCocoaRenderDelegate)
- (void)osgViewWillStartPlaying:(ACOsgViewCocoa*)view;
- (void)osgViewDidStopPlaying:(ACOsgViewCocoa*)view;

- (void)osgViewWillRenderFrame:(ACOsgViewCocoa*)view;
- (void)osgViewDidRenderFrame:(ACOsgViewCocoa*)view;
@end

@interface ACOsgViewCocoa : NSOpenGLView {
	struct ACOsgViewCocoaData 		*_osgViewData;
	
	CVDisplayLinkRef 	_displayLink;
	
	BOOL			_openGLPrepared, _drawing;
	BOOL			_continuousUpdate, _playing; // 2 flags that want diplay link to be running
	
	id			_renderDelegate;
}

@property (readwrite) BOOL playing;
@property (readwrite, assign) id renderDelegate; 
@property (readonly) double currentTime;

@end
