//
//  TCocoaMonitorController.mm
//  Einstein
//
//  Created by Steven Frank on 2/26/11.
//  Copyright 2011 Panic. All rights reserved.
//

#import "TCocoaMonitorController.h"
#import "Monitor/TMacMonitor.h"

@interface TCocoaMonitorController (Private)
- (void)update;
@end

@implementation TCocoaMonitorController

- (void)awakeFromNib
{
	[commandField setDelegate:self];
}


- (void)setMonitor:(TMacMonitor*)inMonitor
{
	monitor = inMonitor;
}


- (void)controlTextDidEndEditing:(NSNotification *)notification
{
	if ( monitor && [notification object] == commandField )
	{
		[self executeCommand:[commandField stringValue]];
		[commandField setStringValue:@""];
		
		[self update];
	}
}


- (void)executeCommand:(NSString*)command
{
	monitor->ExecuteCommand([command UTF8String]);
	[self performSelector:@selector(update) withObject:nil afterDelay:1];
}


- (void)update
{
	NSString* screen = monitor->GetScreen();
	NSDictionary* attrs = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont userFixedPitchFontOfSize:10.0], NSFontAttributeName, nil];
	NSAttributedString* attrString = [[[NSAttributedString alloc] initWithString:screen attributes:attrs] autorelease];
	[[textView textStorage] setAttributedString:attrString];
}

@end
