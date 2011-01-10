//
//  ACAudioCycleOsgiPadController.h
//  MediaCycle
//
//  @author Stéphane Dupont
//  @date 01/01/11
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

#include "ACAudioCycleOsgiPadView.h"

#import <UIKit/UIKit.h>

@interface ACAudioCycleOsgiPadController : UIViewController {

	ACAudioCycleOsgiPadView *osg_view;
	UIButton *on_button;
}

@property (nonatomic, retain) IBOutlet UIView *osg_view;
@property (nonatomic, retain) IBOutlet UIButton *on_button;

// SD TODO - These should probablky be moved to a OSG view specific controller, not the general application controller here
- (void)createGestureRecognizers;
- (IBAction)handlePinchGesture:(UIGestureRecognizer *)sender;
- (IBAction)handlePanGesture:(UIGestureRecognizer *)sender;
- (IBAction)handleSwipeGesture:(UIGestureRecognizer *)sender;
- (IBAction)handleRotationGesture:(UIGestureRecognizer *)sender;

- (IBAction)set_on_button:(id)inSender;

@end

