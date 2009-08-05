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

#include <string>
using namespace::std;

#import <Cocoa/Cocoa.h>
#import <Io/TiOscReceiver.h>
#import <Rendering/TiCocoaOsgView.h>

#import "ACOsgBrowserViewCocoa.h"
#import <MediaCycleLight.h>

// Tags of the Text items in the dialog boxes
enum	{
	kListenerIndex				= 0,
	kListenerGainEditTextItem	= 1000,
	kListenerPosX				= 3000,
	kListenerElevation			= 3001,	// ListenerPosY
	kListenerPosZ				= 3002,
	kDopplerFactorEditTextItem  = 4000,
	kSpeedOfSoundEditTextItem	= 4001,
	kVelocitySpeedEditTextItem	= 4002
};

@interface ACController : NSObject {
	
	MediaCycle				*media_cycle;
	//
	IBOutlet ACOsgBrowserViewCocoa*		browser_osg_view;
	//
	IBOutlet NSSlider*				mListenerGainSlider;
	IBOutlet NSTextField*			mListenerGain;
	IBOutlet NSButton*				mRenderChannelsCheckbox;
	IBOutlet NSButton*				mRenderQualityCheckbox;
	IBOutlet NSPopUpButton*			mDistanceModelPopup;
	//
	IBOutlet NSSlider*				mListenerDirectionSlider;
	IBOutlet NSTextField*			mListenerXPos;
	IBOutlet NSTextField*			mListenerZPos;
	IBOutlet NSTextField*			mListenerXVelocity;
	IBOutlet NSTextField*			mListenerZVelocity;	
	IBOutlet NSSlider*				mListenerElevationSlider;	// 1008
	IBOutlet NSTextField*			mListenerElevation;
	//
	IBOutlet NSSlider*				mDopplerFactorSlider;
	IBOutlet NSTextField*			mDopplerFactor;
	IBOutlet NSSlider*				mSpeedOfSoundSlider;
	IBOutlet NSTextField*			mSpeedOfSound;
	IBOutlet NSSlider*				mListenerVelocityScalerSlider;
	IBOutlet NSTextField*			mListenerVelocityScaler;
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
	IBOutlet NSSlider*				mWeightRhythmSlider;
	IBOutlet NSSlider*				mWeightTimbreSlider;
	IBOutlet NSSlider*				mWeightHarmonySlider;
	IBOutlet NSSlider*				mClusterNumberSlider;
	IBOutlet NSButton*				mFilterInButton;
	IBOutlet NSButton*				mFilterOutButton;
	IBOutlet NSButton*				mFilterSuggestButton;
	//
	IBOutlet NSTextField*			mMediaFileName;
	IBOutlet NSTextField*			mMediaTimestampBegin;
	IBOutlet NSTextField*			mMediaTimestampEnd;
	IBOutlet NSTextField*			mMediaBPM;
	IBOutlet NSTextField*			mMediaTimeSignature;
	IBOutlet NSTextField*			mMediaKey;
	IBOutlet NSTextField*			mMediaUserTags;
	//
	IBOutlet NSTextField*			mALExtensionList;
	IBOutlet NSTextField*			mALCExtensionList;
	IBOutlet NSTextField*			mALCDefaultDeviceName;
	IBOutlet NSTextField*			mALCCaptureDefaultDeviceName;
	IBOutlet NSTextField*			mALVersion;	
	//
	IBOutlet NSSlider*				mBPMSlider;
	IBOutlet NSSlider*				mKeySlider;
	//
	IBOutlet NSSlider*				mPosXSlider;
	IBOutlet NSSlider*				mPosYSlider;
	IBOutlet NSSlider*				mZoomSlider;
	
	TiOscReceiverRef			mOscReceiver;
}

// Context/listener Menu
- (IBAction)	setAddToLibrary:(id)inSender;
- (IBAction)	setOpen:(id)inSender;
- (IBAction)	setSaveAs:(id)inSender;

- (IBAction)	setEngineStart:(id)inSender;
- (IBAction)	setEngineStop:(id)inSender;

- (IBAction)	setCleanLibrary:(id)inSender;
- (IBAction)	setMuteAllSources:(id)inSender;

// Context/Listener Controls
- (IBAction)	setListenerGainSlider:(id)inSender;
- (IBAction)	setRenderChannelsCheckbox:(id)inSender;
- (IBAction)	setRenderQualityCheckbox:(id)inSender;
- (IBAction)	setDistanceModelPU:(id)inSender;

- (IBAction)	setListenerDirectionSlider:(id)inSender;
- (IBAction)	setListenerElevationSlider:(id)inSender;

- (IBAction)	setDopplerFactorSlider:(id)inSender;
- (IBAction)	setSpeedOfSoundSlider:(id)inSender;
- (IBAction)	setListenerVelocitySlider:(id)inSender;

- (IBAction)	setBackButton:(id)inSender;
- (IBAction)	setForwardButton:(id)inSender;
- (IBAction)	setHistoryButton:(id)inSender;
- (IBAction)	setBookmarkButton:(id)inSender;
- (IBAction)	setTagButton:(id)inSender;

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

- (IBAction)	setBPMSlider:(id)inSender;
- (IBAction)	setKeySlider:(id)inSender;
- (IBAction)	setScrubSlider:(id)inSender;

- (IBAction)	setPosXSlider:(id)inSender;
- (IBAction)	setPosYSlider:(id)inSender;
- (IBAction)	setZoomSlider:(id)inSender;

- (IBAction)	setCameraRecenter:(id)inSender;

- (void) controlTextDidEndEditing:(NSNotification *) aNotification;
- (void) myObserver:(NSNotification *)inNotification;
- (void) awakeFromNib;

@end
