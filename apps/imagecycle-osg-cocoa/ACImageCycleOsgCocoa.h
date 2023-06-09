/*
 *  ACController.h
 *  AudioCycle
 *
 *  @author Stéphane Dupont
 *  @date 21/10/08
 *  @copyright (c) 2008 – UMONS - Numediart
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

// Warning: include C++ headers first, then import objective-c/c++ headers
#include <MediaCycle.h>
#include <ACOscBrowser.h>
#include <ACOscFeedback.h>

#import <Cocoa/Cocoa.h>
#import <ACOsgBrowserViewCocoa.h>

@interface ACImageCycleOsgCocoa : NSObject {
	
	MediaCycle				*media_cycle;
	ACOscBrowser			*osc_browser;
	ACOscFeedback			*osc_feedback;

	NSString* osc_control_ip;
	int osc_control_port;
	NSString* osc_feedback_ip;
	int osc_feedback_port; 
	int browser_mode;

	//
	IBOutlet ACOsgBrowserViewCocoa*		browser_osg_view;
	//
	IBOutlet NSButton*				mBackButton;
	IBOutlet NSButton*				mForwardButton;
	IBOutlet NSButton*				mHistoryButton;
	IBOutlet NSButton*				mBookmarkButton;
	IBOutlet NSButton*				mTagButton;
	//
	IBOutlet NSSlider*				mWeight1Slider;
	IBOutlet NSSlider*				mWeight2Slider;
	IBOutlet NSSlider*				mWeight3Slider;
	IBOutlet NSButton*				mWeight1Check;
	IBOutlet NSButton*				mWeight2Check;
	IBOutlet NSButton*				mWeight3Check;
	IBOutlet NSSlider*				mWeightRhythmSlider;
	IBOutlet NSSlider*				mWeightTimbreSlider;
	IBOutlet NSSlider*				mWeightHarmonySlider;
	IBOutlet NSSlider*				mClusterNumberSlider;
	IBOutlet NSButton*				mFilterInButton;
	IBOutlet NSButton*				mFilterOutButton;
	IBOutlet NSButton*				mFilterSuggestButton;
	/*
	IBOutlet NSTextField*			mMediaFileName;
	IBOutlet NSTextField*			mMediaTimestampBegin;
	IBOutlet NSTextField*			mMediaTimestampEnd;
	IBOutlet NSTextField*			mMediaBPM;
	IBOutlet NSTextField*			mMediaTimeSignature;
	IBOutlet NSTextField*			mMediaKey;
	IBOutlet NSTextField*			mMediaUserTags;
	*/
	//
	IBOutlet NSSlider*				mPosXSlider;
	IBOutlet NSSlider*				mPosYSlider;
	IBOutlet NSSlider*				mZoomSlider;

	IBOutlet NSTextField*			mOscControlIp;
	IBOutlet NSTextField*			mOscControlPort;
	IBOutlet NSButton*				mOscControlStatus;
 
	IBOutlet NSTextField*			mOscFeedbackIp;
	IBOutlet NSTextField*			mOscFeedbackPort;
	IBOutlet NSButton*				mOscFeedbackStatus;
 
	IBOutlet NSSegmentedControl*	mBrowserMode;
	
	IBOutlet NSPopUpButton*			mClustersMethodPopUpButton;
	IBOutlet NSPopUpButton*			mClustersPositionsPopUpButton;
	IBOutlet NSPopUpButton*			mNeighborsMethodPopUpButton;
	IBOutlet NSPopUpButton*			mNeighborsPositionsPopUpButton;	
	
	float prevz;

};

// Context/listener Menu
- (IBAction)	setAddToLibrary:(id)inSender;
- (IBAction)	setOpen:(id)inSender;
- (IBAction)	setSaveAs:(id)inSender;

- (IBAction)	setEngineStart:(id)inSender;
- (IBAction)	setEngineStop:(id)inSender;

- (IBAction)	setCleanLibrary:(id)inSender;

- (IBAction)	setBackButton:(id)inSender;
- (IBAction)	setForwardButton:(id)inSender;
- (IBAction)	setBrowserMode:(id)inSender;
- (IBAction)	setHistoryButton:(id)inSender;
- (IBAction)	setBookmarkButton:(id)inSender;
- (IBAction)	setTagButton:(id)inSender;

- (IBAction)	setNeighboursModeRadio:(id)inSender;

- (IBAction)	setClustersMethodPopUpButton:(id)inSender;
- (IBAction)	setClustersPositionsPopUpButton:(id)inSender;
- (IBAction)	setNeighborsMethodPopUpButton:(id)inSender;
- (IBAction)	setNeighborsPositionsPopUpButton:(id)inSender;

- (IBAction)	setWeight1Check:(id)inSender;
- (IBAction)	setWeight2Check:(id)inSender;
- (IBAction)	setWeight3Check:(id)inSender;

- (IBAction)	setWeight1Slider:(id)inSender;
- (IBAction)	setWeight2Slider:(id)inSender;
- (IBAction)	setWeight3Slider:(id)inSender;
- (IBAction)	setWeightRhythmButton:(id)inSender;
- (IBAction)	setWeightTimbreButton:(id)inSender;
- (IBAction)	setWeightHarmonyButton:(id)inSender;
- (IBAction)	setClusterNumberSlider:(id)inSender;
- (IBAction)	setFilterInButton:(id)inSender;
- (IBAction)	setFilterOutButton:(id)inSender;
- (IBAction)	setFilterSuggestButton:(id)inSender;

- (IBAction)	setPosXSlider:(id)inSender;
- (IBAction)	setPosYSlider:(id)inSender;
- (IBAction)	setZoomSlider:(id)inSender;

- (IBAction)	setCameraRecenter:(id)inSender;

- (IBAction)	setOscControlIp:(id)inSender;
- (IBAction)	setOscControlPort:(id)inSender;
- (IBAction)	setOscControlStatus:(id)inSender;
 
- (IBAction)	setOscFeedbackIp:(id)inSender;
- (IBAction)	setOscFeedbackPort:(id)inSender;
- (IBAction)	setOscFeedbackStatus:(id)inSender;

- (void) controlTextDidEndEditing:(NSNotification *) aNotification;
- (void) myObserver:(NSNotification *)inNotification;
- (void) awakeFromNib;
- (void) updatedLibrary;

@end