//
//  TCocoaMonitorController.mm
//  Einstein
//
//  Created by Steven Frank on 2/26/11.
//

#import "TCocoaMonitorController.h"
#import "Monitor/TMacMonitor.h"
#import "TCocoaUserDefaults.h"

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


- (void)applicationWillTerminate:(NSNotification *)aNotification
{
	[[self window] setDelegate:nil];	// so windowWillClose doesn't get called
	[[NSNotificationCenter defaultCenter] removeObserver:self];
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


- (void)setMonitor:(TMacMonitor*)inMonitor
{
	monitor = inMonitor;
}


- (void)showWindow:(id)sender
{
	[super showWindow:sender];
	
	[[self window] setDelegate:self];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationWillTerminate:) name:NSApplicationWillTerminateNotification object:nil];
	
	[[NSUserDefaults standardUserDefaults] setBool:YES forKey:kOpenMonitorAtLaunch];
	[[NSUserDefaults standardUserDefaults] synchronize];
}


- (void)update
{
	NSString* screen = monitor->GetScreen();
	NSDictionary* attrs = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont userFixedPitchFontOfSize:10.0], NSFontAttributeName, nil];
	NSAttributedString* attrString = [[[NSAttributedString alloc] initWithString:screen attributes:attrs] autorelease];
	[[textView textStorage] setAttributedString:attrString];
}


- (void)windowWillClose:(NSNotification *)notification
{
	[[NSUserDefaults standardUserDefaults] setBool:NO forKey:kOpenMonitorAtLaunch];
	[[NSUserDefaults standardUserDefaults] synchronize];
}

@end
