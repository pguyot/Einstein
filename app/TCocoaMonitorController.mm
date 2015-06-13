//
//  TCocoaMonitorController.mm
//  Einstein
//
//  Created by Steven Frank on 2/26/11.
//

#import "TCocoaMonitorController.h"
#import "Monitor/TMacMonitor.h"

@interface TCocoaMonitorController ()
- (void)update;
@end

@implementation TCocoaMonitorController

- (void)awakeFromNib
{
	[commandField setDelegate:self];
}


- (void)dealloc
{
	[commandField setDelegate:nil];
	[super dealloc];
}


- (void)setMonitor:(TMacMonitor*)inMonitor
{
	monitor = inMonitor;
}


- (void)controlTextDidEndEditing:(NSNotification *)notification
{
	if ( monitor && [notification object] == commandField )
	{
		// Only execute the command if return is pressed. If, for example,
		// the textfield loses focus, DON'T trigger a partially
		// entered command.
		
		if ( [[[notification userInfo] objectForKey:@"NSTextMovement"] intValue] != NSReturnTextMovement )
			return;
		
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
