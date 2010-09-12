//
//  iEinsteinViewController.m
//  iEinstein
//
//  Created by Matthias Melcher on 12.09.10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "iEinsteinView.h"

@implementation iEinsteinView



- (id)init {
  return self;
}


- (void)dealloc {
  [super dealloc];
}


- (void)drawRect:(CGRect)rect {
  CGContextRef context = UIGraphicsGetCurrentContext(); 
  CGContextSetRGBStrokeColor(context, 1.0, 1.0, 0.0, 1.0); // yellow line
  
  CGContextBeginPath(context);
  
  CGContextMoveToPoint(context, 50.0, 50.0); //start point
  CGContextAddLineToPoint(context, 250.0, 100.0);
  CGContextAddLineToPoint(context, 250.0, 350.0);
  CGContextAddLineToPoint(context, 50.0, 350.0); // end path
  
  CGContextClosePath(context); // close path
  
  CGContextSetLineWidth(context, 8.0); // this is set from now on until you explicitly change it
  
  CGContextStrokePath(context); // do actual stroking
  
  CGContextSetRGBFillColor(context, 0.0, 1.0, 0.0, 0.5); // green color, half transparent
  CGContextFillRect(context, CGRectMake(20.0, 250.0, 128.0, 128.0)); // a square at the bottom left-hand corner
  
  // setNeedsDisplay
}


@end
