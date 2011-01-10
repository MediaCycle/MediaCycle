//
//  ACAudioCycleOsgiPadController.m
//  MediaCycle
//
//  @author Stéphane Dupont
//  @date 05/01/11
//  @copyright (c) 2011 – UMONS - Numediart
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

#import "ACAudioCycleOsgiPadController.h"

@implementation ACAudioCycleOsgiPadController

@synthesize osg_view;
@synthesize on_button;

- (void)createGestureRecognizers {

    UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc]
										  initWithTarget:self action:@selector(handlePanGesture:)];
	panGesture.minimumNumberOfTouches = 2;
    [self.osg_view addGestureRecognizer:panGesture];
    [panGesture release];
	
	UISwipeGestureRecognizer *swipeGesture = [[UISwipeGestureRecognizer alloc]
										  initWithTarget:self action:@selector(handleSwipeGesture:)];
    //swipeGesture.minimumNumberOfTouches = 2;
	[self.osg_view addGestureRecognizer:swipeGesture];
    [swipeGesture release];
	
	UIRotationGestureRecognizer *rotationGesture = [[UIRotationGestureRecognizer alloc]
										  initWithTarget:self action:@selector(handleRotationGesture:)];
    [self.osg_view addGestureRecognizer:rotationGesture];
    [rotationGesture release];
	
    UIPinchGestureRecognizer *pinchGesture = [[UIPinchGestureRecognizer alloc]
											  initWithTarget:self action:@selector(handlePinchGesture:)];
    [self.osg_view addGestureRecognizer:pinchGesture];
    [pinchGesture release];
}

- (IBAction)handlePinchGesture:(UIGestureRecognizer *)sender {
    CGFloat factor = [(UIPinchGestureRecognizer *)sender scale];
	
	/*
    self.view.transform = CGAffineTransformMakeScale(factor, factor);
	 */
}

- (IBAction)handlePanGesture:(UIPanGestureRecognizer *)sender {
    CGPoint translate = [sender translationInView:self.view];
	
	/*
    CGRect newFrame = currentImageFrame;
    newFrame.origin.x += translate.x;
    newFrame.origin.y += translate.y;
    sender.view.frame = newFrame;
	
    if (sender.state == UIGestureRecognizerStateEnded)
        currentImageFrame = newFrame;
	 */
}

- (IBAction)handleSwipeGesture:(UISwipeGestureRecognizer *)sender {
	
}

- (IBAction)handleRotationGesture:(UIRotationGestureRecognizer *)sender {
	
}

// The designated initializer. Override to perform setup that is required before the view is loaded.
/*
 - (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
	
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    
	if (self) {
        // Custom initialization
		
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	
	// SD TODO - these are not received as soon as the OSG viewer is launched
	//			 find out why
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	
    [super viewDidLoad];

	[self createGestureRecognizers];
	
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations
 return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */

- (void)dealloc {
    [super dealloc];
}

- (IBAction)set_osg_button:(id)inSender {

}

@end
