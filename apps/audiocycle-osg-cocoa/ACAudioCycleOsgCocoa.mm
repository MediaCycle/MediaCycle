/*
 *  ACAudioCycleOsgCocoa.mm
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 21/10/08
 *  @author Xavier Siebert
 * - getLibrary instead of getImageLibrary CHECK THIS
 *
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

#include<iostream>
#import <ACAudioCycleOsgCocoa.h>

////
/*
@interface ACAudioCycleOsgCocoa ()
 - (void)processOscMessage:(const char*)tagName;
 @end
 
 static void osc_callback(TiOscReceiverRef, const char *tagName, void *userData)
 {
 ACAudioCycleOsgCocoa *self = (ACAudioCycleOsgCocoa*)userData;
 
 //printf("osc received tag: %s\n", tagName);
 
 [self processOscMessage:tagName];
 
 }
*/
////

@implementation ACAudioCycleOsgCocoa

+ (void)initialize
{
	// to avoid linker from discarding that class
	[TiCocoaOsgView class];
	[ACOsgBrowserViewCocoa class];//CF
}

- (void)initCommonACAudioCycleOsgCocoa
{
////
	/*
	mOscReceiver = TiOscReceiverCreate("localhost", 12345);
	
	TiOscReceiverSetUserData(mOscReceiver, self);
	TiOscReceiverSetCallback(mOscReceiver, osc_callback);
	
	TiOscReceiverStart(mOscReceiver);
	 */
////	 
}

- init
{
	if(self = [super init])
	{
		[self initCommonACAudioCycleOsgCocoa];
	}
	
	return self;
}

- (void)dealloc
{
	////osc_browser->stop();
////
	/*
	TiOscReceiverStop(mOscReceiver);
	TiOscReceiverRelease(mOscReceiver);
	*/
////
	
	[super dealloc];
}

- initWithCoder:coder
{
	if(self/* = [super initWithCoder:coder]*/)
	{
		[self initCommonACAudioCycleOsgCocoa];
	}
	
	return self;
}

// SD TODO - OpenGL window should prompt update of other elements of the GUI interface. To be done.
- (void)myObserver:(NSNotification *)inNotification
{
	
}

- (void)updatedLibrary
{	
	media_cycle->setSelectedObject(0);
	
	[browser_osg_view prepareFromBrowser];
	
	[browser_osg_view setPlaying:YES];
	//[browser_osg_view updateTransformsFromBrowser];
	
	media_cycle->setNeedsDisplay(true);
}

- (void) awakeFromNib
{
	[[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(myObserver:) name: @"OALNotify" object: NULL];
	
	media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");
	media_cycle->addPlugin("/Users/dtardieu/src/Numediart/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-x86/plugins/audio/Debug/mc_audiofeatures.dylib");
	//media_cycle->addPlugin ("/Users/dtardieu/src/Numediart/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-x86/plugins/visualisation/Debug/mc_visualisation.dylib");
	//media_cycle->setVisualisationPlugin("Visualisation");
	
	//	media_cycle->addPlugin ("/Users/dtardieu/src/Numediart/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-x86/plugins/visualisation/Debug/mc_visualisation.dylib");	
	//	media_cycle->setVisualisationPlugin("Visualisation");
	
	//media_cycle->setNeighborhoodsPlugin("RandomNeighborhoods");
	//media_cycle->setPositionsPlugin("RandomPositions");
	
	audio_engine = new ACAudioFeedback();
	audio_engine->setMediaCycle(media_cycle);

	////osc_browser = new ACOscBrowser();
	////osc_browser->setMediaCycle(media_cycle);
	////osc_browser->start("localhost", 12345);
	
	//media_cycle->importLibrary("/dupont/dancers.acl_copy");
	//media_cycle->setClusterNumber(5);
	//media_cycle->setSelectedObject(0);
	
	[browser_osg_view setMediaCycle:media_cycle];
	[browser_osg_view prepareFromBrowser];
	[browser_osg_view setPlaying:YES];
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MAIN MENU
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
- (void)applicationWillTerminate:(NSNotification *)aNotification
{
	[browser_osg_view setPlaying:NO];
	delete audio_engine;
	delete media_cycle;
}

- (IBAction)	setAddToLibrary:(id)inSender
{
	int count;
	
	@synchronized(browser_osg_view)
	{
		// Create the File Open Dialog class.
		NSOpenPanel* openDlg = [NSOpenPanel openPanel];
		
		// Enable the selection of files in the dialog.
		[openDlg setCanChooseFiles:YES];
		[openDlg setAllowsMultipleSelection:YES];
		
		// Enable the selection of directories in the dialog.
		[openDlg setCanChooseDirectories:YES];
		
		// Display the dialog.  If the OK button was pressed,
		// process the files.
		if ( [openDlg runModalForDirectory:nil file:nil] == NSOKButton )
		{
			// Get an array containing the full filenames of all
			// files and directories selected.
			NSArray* paths = [openDlg filenames];
			
			// Loop through all the files and process them.
			for( count = 0; count < [paths count]; count++ )
			{
				NSString* path = [paths objectAtIndex:count];
				
				// Do something with the filename
				// SD TODO - Ask user for confirmation and display progress bar....
				media_cycle->importDirectory((string)[path UTF8String], 1);
			}
			
			// with this function call here, do not import twice!!!
			media_cycle->normalizeFeatures();
		}
		
		[self updatedLibrary];
	}
}

- (IBAction)	setOpen:(id)inSender
{
	@synchronized(browser_osg_view)
	{
	// Create the File Open Dialog class.
	NSOpenPanel* openDlg = [NSOpenPanel openPanel];
	
	// Enable the selection of files in the dialog.
	[openDlg setCanChooseFiles:YES];
	[openDlg setAllowsMultipleSelection:NO];
	
	// Enable the selection of directories in the dialog.
	[openDlg setCanChooseDirectories:NO];
	
	// Display the dialog.  If the OK button was pressed,
	// process the files.
	if ( [openDlg runModalForDirectory:nil file:nil] == NSOKButton )
	{
		// Get an array containing the full filenames of all
		// files and directories selected.
		NSArray* paths = [openDlg filenames];
		
		NSString* path = [paths objectAtIndex:0];
		
		media_cycle->importLibrary((string)[path UTF8String]); // XS instead of getImageLibrary CHECK THIS
		//media_cycle->libraryContentChanged();
	}
	[self updatedLibrary];
	}
}

- (IBAction)	setSaveAs:(id)inSender
{
	NSSavePanel* saveDlg = [NSSavePanel savePanel];
	
	[saveDlg setRequiredFileType:@"acl"];
	
	if ( [saveDlg runModalForDirectory:nil file:nil] == NSOKButton )
	{
		NSString* path = [saveDlg filename];
		
		media_cycle->saveAsLibrary((string)[path UTF8String]); // XS instead of getImageLibrary CHECK THIS
	}
}

- (IBAction)	setEngineStart:(id)inSender
{
	// SD TODO
	audio_engine->startAudioEngine();
	//audio_cycle->getAudioFeedback()->startAudioEngine();
}

- (IBAction)	setEngineStop:(id)inSender
{
	// SD TODO
	audio_engine->stopAudioEngine();
	//audio_cycle->getAudioFeedback()->stopAudioEngine();
}

- (IBAction)	setCleanLibrary:(id)inSender
{
	@synchronized(browser_osg_view)
	{
		media_cycle->cleanLibrary(); // XS instead of getImageLibrary CHECK THIS
		media_cycle->libraryContentChanged();
		[self updatedLibrary];
	}
}

- (IBAction)	setMuteAllSources:(id)inSender
{
	media_cycle->muteAllSources();
}
/*
- (IBAction)	setOscIp:(id)inSender
{
	NSString* 	value = [inSender stringValue];
	osc_ip = value;
	NSLog(@"OSC IP: %s",osc_ip);
}

- (IBAction)	setOscPort:(id)inSender
{
	float 	value = [inSender floatValue];
	osc_port = (int) value;
	NSLog(@"OSC port: %i",osc_port);
}

- (IBAction)	setOscStatus:(id)inSender
{
	NSLog(@"OSC server: %s %i",osc_ip,osc_port);
	//int	value = [inSender intValue];
	//if (value == 1)
	//{
	mOscReceiver = TiOscReceiverCreate((const char*)[osc_ip UTF8String], osc_port);
	TiOscReceiverSetUserData(mOscReceiver, self);
	TiOscReceiverSetCallback(mOscReceiver, osc_callback);
	TiOscReceiverStart(mOscReceiver);
	//}	
}
*/
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BROWSER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark ***** Browser *****

- (IBAction)	setBackButton:(id)inSender
{
	media_cycle->setBack();
}

- (IBAction)	setForwardButton:(id)inSender
{
	media_cycle->setForward();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BROWSER CONTROLS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark ***** Browser Controls *****

- (IBAction)setWeight1Slider:(id)inSender
{
	float	value = [inSender floatValue];
	
	media_cycle->setWeight(0, value);
}

- (IBAction)setWeight2Slider:(id)inSender
{
	float	value = [inSender floatValue];
	
	media_cycle->setWeight(1, value);
}

- (IBAction)setWeight3Slider:(id)inSender
{
	float	value = [inSender floatValue];
	
	media_cycle->setWeight(2, value);
}

- (IBAction)setClusterNumberSlider:(id)inSender
{
	int	value = [inSender intValue];
	
	media_cycle->setClusterNumber(value);
}

- (IBAction)setCameraRecenter:(id)inSender
{
	media_cycle->setCameraRecenter();
}

- (void)dispatchMessage:(NSDictionary*)msg
{
	// main thread
	
	if([[msg objectForKey:@"name"] isEqual:@"movexy"])
	{
		float x = [[msg objectForKey:@"x"] floatValue], y = [[msg objectForKey:@"y"] floatValue];
		
		// do something from the main thread
		media_cycle->setCameraPosition(x, y);
	}
	else if([[msg objectForKey:@"name"] isEqual:@"zoom"])
	{
		float x = [[msg objectForKey:@"val"] floatValue];
		
		// do something from the main thread
		media_cycle->setCameraZoom(x);
	}
}

////
/*
- (void)processOscMessage:(const char*)tagName
{
	id pool = [NSAutoreleasePool new];

	if(strcasecmp(tagName, "/audiocycle/test") == 0)
	{
		std::cout << "OSC communication established" << std::endl;
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/xy") == 0)
	{
		float x = 0.0, y = 0.0;
		media_cycle->getCameraPosition(x,y);
		TiOscReceiverReadFloat(mOscReceiver, &x);
		TiOscReceiverReadFloat(mOscReceiver, &y);

		float zoom = media_cycle->getCameraZoom();
		float angle = media_cycle->getCameraRotation();
		float xmove = x*cos(-angle)-y*sin(-angle);
		float ymove = y*cos(-angle)+x*sin(-angle);
		media_cycle->setCameraPosition(xmove/2/zoom , ymove/2/zoom); // norm [-1;1] = 2 (instead of 100 from mediacycle-osg)
		NSLog(@"zoom: %f setCameraPosition: %f %f",zoom,xmove/2/zoom ,ymove/2/zoom);
		media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/hover/xy") == 0)
	{
		float x = 0.0, y = 0.0;
		TiOscReceiverReadFloat(mOscReceiver, &x);
		TiOscReceiverReadFloat(mOscReceiver, &y);
		
		media_cycle->hoverCallback(x,y);
		//media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/zoom") == 0)
	{
		float zoom, refzoom = media_cycle->getCameraZoom();
		TiOscReceiverReadFloat(mOscReceiver, &zoom);
		//zoom = zoom*600/50; // refzoom +
		media_cycle->setCameraZoom((float)zoom);
		media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/angle") == 0)
	{
		float angle;//, refangle = media_cycle->getCameraRotation();
		TiOscReceiverReadFloat(mOscReceiver, &angle);
		media_cycle->setCameraRotation((float)angle);
		media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/library/load") == 0)
	{
		char *lib_path = NULL;
		lib_path = new char[500]; // wrong magic number!
		TiOscReceiverReadString(mOscReceiver, lib_path, 500); // wrong magic number!
		std::cout << "Importing file library '" << lib_path << "'..." << std::endl;
		media_cycle->importLibrary(lib_path); // XS instead of getImageLibrary CHECK THIS
		[self updatedLibrary]; // change for non-objective-c code
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/library/clear") == 0)
	{
		media_cycle->cleanLibrary(); // XS instead of getImageLibrary CHECK THIS
		media_cycle->libraryContentChanged();
		[self updatedLibrary]; // change for non-objective-c code
	}	
	else if(strcasecmp(tagName, "/audiocycle/1/browser/recenter") == 0)
	{
		media_cycle->setCameraRecenter();
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/playclosestloop") == 0)
	{	
		media_cycle->pickedObjectCallback(-1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/muteall") == 0)
	{	
		media_cycle->muteAllSources();
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/bpm") == 0)
	{
		float bpm;
		TiOscReceiverReadFloat(mOscReceiver, &bpm);
		int clicked_loop = media_cycle->getClickedLoop();
		if (clicked_loop > -1)
		{
			audio_engine->setLoopSynchroMode(clicked_loop, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(clicked_loop, ACAudioEngineScaleModeResample);
			audio_engine->setBPM((float)bpm);
		}
	}	
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/scrub") == 0)
	{
		float scrub;
		TiOscReceiverReadFloat(mOscReceiver, &scrub);
		
		 int clicked_loop = media_cycle->getClickedLoop();
		 if (clicked_loop > -1)
		 {
			 //media_cycle->pickedObjectCallback(-1);
			 audio_engine->setLoopSynchroMode(clicked_loop, ACAudioEngineSynchroModeManual);
			 audio_engine->setLoopScaleMode(clicked_loop, ACAudioEngineScaleModeVocode);
			 audio_engine->setScrub((float)scrub*10000); // temporary hack to scrub between 0 an 1
		 }
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/pitch") == 0)
	{
		float pitch;
		TiOscReceiverReadFloat(mOscReceiver, &pitch);
	
		int clicked_loop = media_cycle->getClickedLoop();
		if (clicked_loop > -1)
		{
			
			//if (!is_pitching)
			//{	
			//	is_pitching = true;
			//	is_scrubing = false;
			 
				//media_cycle->pickedObjectCallback(-1);
				audio_engine->setLoopSynchroMode(clicked_loop, ACAudioEngineSynchroModeAutoBeat);
				audio_engine->setLoopScaleMode(clicked_loop, ACAudioEngineScaleModeResample);
			//}
			audio_engine->setSourcePitch(clicked_loop, (float) pitch); 
		}
		 
	}

	// ...
	 //void setKey(int key);

	[pool release];
}
////
*/
@end
