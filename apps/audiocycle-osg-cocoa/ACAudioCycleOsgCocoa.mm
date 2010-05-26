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
@interface ACAudioCycleOsgCocoa ()
 - (void)processOscMessage:(const char*)tagName;
@end
 
static void osc_callback(ACOscBrowserRef, const char *tagName, void *userData)
{
	ACAudioCycleOsgCocoa *self = (ACAudioCycleOsgCocoa*)userData;
	//printf("osc received tag: %s\n", tagName);
	[self processOscMessage:tagName];
}
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
	// SD TOTO - srand() is called in other places, so no randomization actually.
	int library_size = media_cycle->getLibrarySize();
	int rand_node;
	if (library_size==0) {
		rand_node = 0;
	}
	else {
		rand_node = rand();
		rand_node = rand_node % library_size;
	}
	media_cycle->setReferenceNode(rand_node);
	// XSCF 250310 added these 3
	media_cycle->pushNavigationState();
	media_cycle->getBrowser()->setState(AC_CHANGING);
	media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
	
	[browser_osg_view prepareFromBrowser];
	
	[browser_osg_view setPlaying:YES];
	//[browser_osg_view updateTransformsFromBrowser];
	
	media_cycle->setNeedsDisplay(true);
}

- (void) awakeFromNib
{	
	[[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(myObserver:) name: @"OALNotify" object: NULL];
	
	media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");
	std::string build_type ("Release");
	#ifdef USE_DEBUG
		build_type = "Debug";
	#endif
	media_cycle->addPlugin("../../../plugins/audio/" + build_type + "/mc_audio.dylib");
	//int vizplugloaded = media_cycle->addPlugin("../../../plugins/visualisation/" + build_type + "/mc_visualisation.dylib");
	int vizplugloaded = media_cycle->addPlugin("/dupont/development/workdir-new/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-x86/plugins/visualisation/" + build_type + "/mc_visualisation.dylib");
	if ( vizplugloaded == 0 )
	{
		//media_cycle->setVisualisationPlugin("Visualisation");
		//media_cycle->setVisualisationPlugin("PCAVis");
		//media_cycle->setVisualisationPlugin("Vis2Desc");
		//media_cycle->setNeighborhoodsPlugin("RandomNeighborhoods");
		//media_cycle->setNeighborhoodsPlugin("ParetoNeighborhoods");
		//media_cycle->setNeighborhoodsPlugin("RandomNeighborhoods");
		//media_cycle->setPositionsPlugin("NodeLinkTreeLayoutPositions");
		//media_cycle->setNeighborhoodsPlugin("EuclideanNeighborhoods");
		//media_cycle->setPositionsPlugin("RadialTreeLayoutPositions");
	}
	audio_engine = new ACAudioFeedback();
	audio_engine->setMediaCycle(media_cycle);

	osc_feedback = NULL;
	osc_browser = NULL;
	
	//media_cycle->importLibrary("/dupont/dancers.acl_copy");
	//media_cycle->setClusterNumber(5);
	// XSCF251003 added this
	//media_cycle->updateClusters(true);
	//media_cycle->setNeedsDisplay(true);
	
	//media_cycle->setReferenceNode(0);
	// XSCF 250310 added these 3
	//media_cycle->pushNavigationState();
	//media_cycle->updateNextPositions(); // TODO is it required ?? .. hehehe
	//media_cycle->setState(AC_CHANGING);
	
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
	delete osc_browser;
	delete osc_feedback;//osc_feedback destructor calls ACOscFeedback::release()
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
	int n;
	
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
		
		media_cycle->importACLLibrary((string)[path UTF8String]); // XS instead of getImageLibrary CHECK THIS
		media_cycle->normalizeFeatures();
		media_cycle->libraryContentChanged();
		n = media_cycle->getNumberOfMediaNodes();
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
		media_cycle->cleanUserLog();
		media_cycle->libraryContentChanged();
		[self updatedLibrary];
	}
}

- (IBAction)	setMuteAllSources:(id)inSender
{
	media_cycle->muteAllSources();
}

- (IBAction)	setOscControlIp:(id)inSender
{
	NSString* 	value = [inSender stringValue];
	osc_control_ip = value;
	NSLog(@"OSC IP: %s",osc_control_ip);
}

- (IBAction)	setOscControlPort:(id)inSender
{
	float 	value = [inSender floatValue];
	osc_control_port = (int) value;
	NSLog(@"OSC port: %i",osc_control_port);
}

- (IBAction)	setOscControlStatus:(id)inSender
{
	osc_control_ip = [mOscControlIp stringValue];
	osc_control_port = [mOscControlPort intValue];
	NSLog(@"OSC server: %s %i",osc_control_ip,osc_control_port);
	int	value = [inSender intValue];
	if (value == 1)
	{
		osc_browser = new ACOscBrowser();
		mOscReceiver = osc_browser->create((const char*)[osc_control_ip UTF8String], osc_control_port);
		osc_browser->setUserData(mOscReceiver, self);
		osc_browser->setCallback(mOscReceiver, osc_callback);
		osc_browser->start(mOscReceiver);
	}
	else
	{
		osc_browser->stop(mOscReceiver);
	}
}
 
- (IBAction)	setOscFeedbackIp:(id)inSender
{
	NSString* 	value = [inSender stringValue];
	osc_feedback_ip = value;
	NSLog(@"OSC IP: %s",osc_feedback_ip);
}
 
- (IBAction)	setOscFeedbackPort:(id)inSender
{
	float 	value = [inSender floatValue];
	osc_feedback_port = (int) value;
	NSLog(@"OSC port: %i",osc_feedback_port);
}
 
- (IBAction)	setOscFeedbackStatus:(id)inSender
{
	osc_feedback_ip = [mOscFeedbackIp stringValue];
	osc_feedback_port = [mOscFeedbackPort intValue];
	NSLog(@"OSC server: %s %i",*osc_feedback_ip,osc_feedback_port);
	int	value = [inSender intValue];
	if (value == 1)
	{
		osc_feedback = new ACOscFeedback();
		osc_feedback->create((const char*)[osc_feedback_ip UTF8String], osc_feedback_port);
	}
	else
	{
		osc_feedback->release();
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BROWSER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark ***** Browser *****

- (IBAction)	setBackButton:(id)inSender
{
	media_cycle->goBack();
}

- (IBAction)	setForwardButton:(id)inSender
{
	media_cycle->goForward();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BROWSER CONTROLS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark ***** Browser Controls *****

- (IBAction)	setNeighboursModeRadio:(id)inSender
{
	NSButtonCell *selCell = [inSender selectedCell];
    int value = [selCell tag];
	if (value==0) {		// pareto
		media_cycle->setNeighborhoodsPlugin("ParetoNeighborhoods");
	}
	else if (value==1) { // euclidean
		media_cycle->setNeighborhoodsPlugin("EuclideanNeighborhoods");
	}	
}

- (IBAction)	setWeight1Check:(id)inSender
{
	int	value = [inSender intValue];
	if (value == 1) {
		media_cycle->setWeight(0, 1);
	}
	else {
		media_cycle->setWeight(0, 0);
	}
	media_cycle->updateDisplay(true, 0);
}

- (IBAction)	setWeight2Check:(id)inSender
{
	int	value = [inSender intValue];
	if (value == 1) {
		media_cycle->setWeight(1, 1);
	}
	else {
		media_cycle->setWeight(1, 0);
	}
	media_cycle->updateDisplay(true, 0);
}

- (IBAction)	setWeight3Check:(id)inSender
{
	int	value = [inSender intValue];
	if (value == 1) {
		media_cycle->setWeight(2, 1);
	}
	else {
		media_cycle->setWeight(2, 0);
	}
	media_cycle->updateDisplay(true, 0);
}

- (IBAction)setWeight1Slider:(id)inSender
{
	float	value = [inSender floatValue];
	
	media_cycle->setWeight(0, value);
	media_cycle->updateDisplay(true, 0); //XS 250310 was: media_cycle->updateClusters(true);
	// XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay

}

- (IBAction)setWeight2Slider:(id)inSender
{
	float	value = [inSender floatValue];
	
	media_cycle->setWeight(1, value);
	media_cycle->updateDisplay(true, 0); //XS 250310 was: media_cycle->updateClusters(true);
	// XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay

}

- (IBAction)setWeight3Slider:(id)inSender
{
	float	value = [inSender floatValue];
	
	media_cycle->setWeight(2, value);
	media_cycle->updateDisplay(true, 0); //XS 250310 was: media_cycle->updateClusters(true);
	// XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay

}

- (IBAction)setClusterNumberSlider:(id)inSender
{
	int	value = [inSender intValue];
	
	media_cycle->setClusterNumber(value);
	// XSCF251003 added this
	media_cycle->updateDisplay(true, 0); //XS 250310 was: media_cycle->updateClusters(true);
	// XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay
	
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

- (void)processOscMessage:(const char*)tagName
{
	id pool = [NSAutoreleasePool new];

	if(strcasecmp(tagName, "/audiocycle/test") == 0)
	{
		std::cout << "OSC communication established" << std::endl;
		
		if (osc_feedback)
		{
			osc_feedback->messageBegin("/audiocycle/received");
			osc_feedback->messageEnd();
			osc_feedback->messageSend();
		}
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/xy") == 0)
	{
		float x = 0.0, y = 0.0;
		media_cycle->getCameraPosition(x,y);
		osc_browser->readFloat(mOscReceiver, &x);
		osc_browser->readFloat(mOscReceiver, &y);

		float zoom = media_cycle->getCameraZoom();
		float angle = media_cycle->getCameraRotation();
		float xmove = x*cos(-angle)-y*sin(-angle);
		float ymove = y*cos(-angle)+x*sin(-angle);
		media_cycle->setCameraPosition(xmove/2/zoom , ymove/2/zoom); // norm [-1;1] = 2 (instead of 100 from mediacycle-osg)
		NSLog(@"zoom: %f setCameraPosition: %f %f",zoom,xmove/2/zoom ,ymove/2/zoom);
		media_cycle->setNeedsDisplay(true);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/hover/xy") == 0)
	{
		float x = 0.0, y = 0.0;
		osc_browser->readFloat(mOscReceiver, &x);
		osc_browser->readFloat(mOscReceiver, &y);
		
		media_cycle->hoverCallback(x,y);
		int closest_node = media_cycle->getClosestNode();
		//CF debug the objective-c accessor on renderer!
		/*float distance = browser_osg_view.renderer->getDistanceMouse()[closest_node];
		if (osc_feedback)
		{
			osc_feedback->messageBegin("/audiocycle/closest_node_at");
			osc_feedback->messageAppendFloat(distance);
			osc_feedback->messageEnd();
			osc_feedback->messageSend();
		}
		*/
		//media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/zoom") == 0)
	{
		float zoom;//, refzoom = media_cycle->getCameraZoom();
		osc_browser->readFloat(mOscReceiver, &zoom);
		//zoom = zoom*600/50; // refzoom +
		media_cycle->setCameraZoom((float)zoom);
		media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/angle") == 0)
	{
		float angle;//, refangle = media_cycle->getCameraRotation();
		osc_browser->readFloat(mOscReceiver, &angle);
		media_cycle->setCameraRotation((float)angle);
		media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/library/load") == 0)
	{
		char *lib_path = NULL;
		lib_path = new char[500]; // wrong magic number!
		osc_browser->readString(mOscReceiver, lib_path, 500); // wrong magic number!
		std::cout << "Importing file library '" << lib_path << "'..." << std::endl;
		media_cycle->importACLLibrary(lib_path);
		media_cycle->normalizeFeatures();
		media_cycle->libraryContentChanged();
		std::cout << "File library imported" << std::endl;
		[self updatedLibrary]; // change for non-objective-c code
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/library/clear") == 0)
	{
		media_cycle->cleanLibrary();
		media_cycle->cleanUserLog();
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
		osc_browser->readFloat(mOscReceiver, &bpm);
		//int clicked_node = media_cycle->getClickedNode();
		int clicked_node = media_cycle->getClosestNode();
		if (clicked_node > -1)
		{
			audio_engine->setLoopSynchroMode(clicked_node, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(clicked_node, ACAudioEngineScaleModeResample);
			audio_engine->setBPM((float)bpm);
		}
	}	
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/scrub") == 0)
	{
		float scrub;
		osc_browser->readFloat(mOscReceiver, &scrub);
		
		//int clicked_node = media_cycle->getClickedNode();
		int clicked_node = media_cycle->getClosestNode();
		 if (clicked_node > -1)
		 {
			 //media_cycle->pickedObjectCallback(-1);
			 audio_engine->setLoopSynchroMode(clicked_node, ACAudioEngineSynchroModeManual);
			 audio_engine->setLoopScaleMode(clicked_node, ACAudioEngineScaleModeVocode);//ACAudioEngineScaleModeVocode
			 audio_engine->setScrub((float)scrub*100); // temporary hack to scrub between 0 an 1
		 }
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/pitch") == 0)
	{
		float pitch;
		osc_browser->readFloat(mOscReceiver, &pitch);
	
		//int clicked_node = media_cycle->getClickedNode();
		int clicked_node = media_cycle->getClosestNode();
		if (clicked_node > -1)
		{
			//if (!is_pitching)
			//{	
			//	is_pitching = true;
			//	is_scrubing = false;
			//media_cycle->pickedObjectCallback(-1);
			audio_engine->setLoopSynchroMode(clicked_node, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(clicked_node, ACAudioEngineScaleModeResample);
			//}
			audio_engine->setSourcePitch(clicked_node, (float) pitch); 
		}
		 
	}
	// ...
	//void setKey(int key);
	[pool release];
}
////
@end
