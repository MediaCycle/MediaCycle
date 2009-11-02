/*
 *  ACController.mm
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

#import "ACController.h"

@interface ACController ()
- (void)processOscMessage:(const char*)tagName;
@end

static void osc_callback(TiOscReceiverRef, const char *tagName, void *userData)
{
	ACController *self = (ACController*)userData;
	
	printf("osc received tag: %s\n", tagName);
	
	[self processOscMessage:tagName];
}

@implementation ACController

+ (void)initialize
{
	// to avoid linker from discarding that class
	[TiCocoaOsgView class];
}

- (void)initCommonACController
{
	/*mOscReceiver = TiOscReceiverCreate(NULL, 12345);
	
	TiOscReceiverSetUserData(mOscReceiver, self);
	TiOscReceiverSetCallback(mOscReceiver, osc_callback);
	
	TiOscReceiverStart(mOscReceiver);
	 */
}

- init
{
	if(self = [super init])
	{
		[self initCommonACController];
	}
	
	return self;
}

- (void)dealloc
{
	/*TiOscReceiverStop(mOscReceiver);
	TiOscReceiverRelease(mOscReceiver);
	*/
	
	[super dealloc];
}

- initWithCoder:coder
{
	if(self/* = [super initWithCoder:coder]*/)
	{
		[self initCommonACController];
	}
	
	return self;
}

// SD TODO - OpenGL window should prompt update of other elements of the GUI interface. To be done.
- (void)myObserver:(NSNotification *)inNotification
{
	
}

- (void) awakeFromNib
{
	[[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(myObserver:) name: @"OALNotify" object: NULL];
	
	media_cycle = new MediaCycle(MEDIA_TYPE_IMAGE,"/tmp/","mediacycle.acl");
	
	NSLog(@"DANCERS!");
	
	//media_cycle->importLibrary("/dupont/dancers.acl_copy");
	
	NSLog(@"DANCERS!");
	
	//media_cycle->setClusterNumber(5);
	
	NSLog(@"DANCERS!");
	
	//media_cycle->setSelectedObject(0);
	
	[browser_osg_view setMediaCycle:media_cycle];

	//[browser_osg_view prepareFromBrowser];
	
	[browser_osg_view setPlaying:YES];
	
	NSLog(@"DANCERS!");
}

- (void)updatedLibrary
{	
	media_cycle->setSelectedObject(0);
	
	[browser_osg_view prepareFromBrowser];
	
	[browser_osg_view setPlaying:YES];
	//[browser_osg_view updateTransformsFromBrowser];
	
	media_cycle->setNeedsDisplay(true);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MAIN MENU
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
- (void)applicationWillTerminate:(NSNotification *)aNotification
{
	[browser_osg_view setPlaying:NO];
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
			media_cycle->normalizeFeatures(); // XS instead of getImageLibrary CHECK THIS
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
	//audio_cycle->getAudioFeedback()->startAudioEngine();
}

- (IBAction)	setEngineStop:(id)inSender
{
	// SD TODO
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

- (void)processOscMessage:(const char*)tagName
{
	id pool = [NSAutoreleasePool new];
	if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/xy") == 0)
	{
		float val1 = 0.0, val2 = 0.0;
		
		TiOscReceiverReadFloat(mOscReceiver, &val1);
		TiOscReceiverReadFloat(mOscReceiver, &val2);
		
		id msg = [NSDictionary dictionaryWithObjectsAndKeys:@"movexy", @"name", [NSNumber numberWithFloat:val1], @"x", [NSNumber numberWithFloat:val2], @"y", nil];
		
		[self performSelectorOnMainThread:@selector(dispatchMessage:) withObject:msg waitUntilDone:YES];
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/zoom") == 0)
	{
		float val1 = 0.0;
		
		TiOscReceiverReadFloat(mOscReceiver, &val1);
		
		id msg = [NSDictionary dictionaryWithObjectsAndKeys:@"zoom", @"name", [NSNumber numberWithFloat:val1], @"val", nil];
		
		[self performSelectorOnMainThread:@selector(dispatchMessage:) withObject:msg waitUntilDone:YES];
	}
	// ...
	
	
	[pool release];
}

@end
