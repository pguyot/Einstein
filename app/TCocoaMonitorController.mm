//
//  TCocoaMonitorController.mm
//  Einstein
//
//  Created by Steven Frank on 2/26/11.
//

#import "TCocoaMonitorController.h"
#import "Monitor/TMacMonitor.h"
#import "TCocoaUserDefaults.h"
#import "TMacMonitorView.h"
#import "TMonitorCore.h"

@interface TCocoaMonitorController ()
- (void)update;
@end

@implementation TCocoaMonitorController

- (void)awakeFromNib
{
}


- (void)dealloc
{
	[super dealloc];
}


- (void)addHistoryLine:(NSString *)line type:(int)type
{
	[view addHistoryLine:line type:type];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification
{
	[[self window] setDelegate:nil];	// so windowWillClose doesn't get called
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}


- (void)executeCommand:(NSString*)command
{
	[self addHistoryLine:[NSString stringWithFormat:@"> %@", command] type:MONITOR_LOG_USER_INPUT];
	
	monitor->ExecuteCommand([command UTF8String]);
	[self performSelector:@selector(update) withObject:nil afterDelay:0.0];
	
	if ( [command isEqualToString:@"run"] )
	{
		[stopStartButton setTitle:@"Stop"];
	}
	else if ( [command isEqualToString:@"stop"] )
	{
		[stopStartButton setTitle:@"Run"];
	}
}


- (void)setMonitor:(TMacMonitor*)inMonitor
{
	monitor = inMonitor;
	monitor->SetController(self);
}


- (void)showWindow:(id)sender
{
	[super showWindow:sender];
	
	[[self window] setDelegate:self];
	
	[view setController:self];
	[[self window] makeFirstResponder:view];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationWillTerminate:) name:NSApplicationWillTerminateNotification object:nil];
	
	[[NSUserDefaults standardUserDefaults] setBool:YES forKey:kOpenMonitorAtLaunch];
	[[NSUserDefaults standardUserDefaults] synchronize];
}


- (IBAction)stepInto:(id)sender
{
	monitor->ExecuteCommand("step");
	[self performSelector:@selector(update) withObject:nil afterDelay:0.0];
}


- (IBAction)stepOver:(id)sender
{
	monitor->ExecuteCommand("trace");
	[self performSelector:@selector(update) withObject:nil afterDelay:0.0];
}


- (void)stopStart:(id)sender
{
	if ( monitor )
	{
		if ( monitor->IsHalted() )
		{
			monitor->ExecuteCommand("run");
		}
		else
		{
			monitor->ExecuteCommand("stop");
		}
		
		[stopStartButton setEnabled:YES];
		[self performSelector:@selector(update) withObject:nil afterDelay:0.0];
	}
	else
	{
		[self performSelector:@selector(update) withObject:nil afterDelay:0.0];
		[stopStartButton setEnabled:NO];
	}
}


- (void)update
{
	[view updateWithMonitor:monitor];
	
	if ( monitor->IsHalted() )
	{
		[stepIntoButton setEnabled:YES];
		[stepOverButton setEnabled:YES];
		[stopStartButton setTitle:@"Run"];
	}
	else
	{
		[stepIntoButton setEnabled:NO];
		[stepOverButton setEnabled:NO];
		[stopStartButton setTitle:@"Stop"];
	}
}


- (void)windowWillClose:(NSNotification *)notification
{
	[[NSUserDefaults standardUserDefaults] setBool:NO forKey:kOpenMonitorAtLaunch];
	[[NSUserDefaults standardUserDefaults] synchronize];
}

@end
