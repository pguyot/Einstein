//
//  TCocoaMonitorController.h
//  Einstein
//
//  Created by Steven Frank on 2/26/11.
//  Copyright 2011 Panic. All rights reserved.
//

#import <Cocoa/Cocoa.h>

class TMacMonitor;


@interface TCocoaMonitorController : NSWindowController 
{
	IBOutlet NSTextField* commandField;
	IBOutlet NSTextView* textView;

	TMacMonitor* monitor;
}

- (void)executeCommand:(NSString*)command;
- (void)setMonitor:(TMacMonitor*)inMonitor;

@end
