//
//  TCocoaMonitorController.h
//  Einstein
//
//  Created by Steven Frank on 2/26/11.
//

#import <Cocoa/Cocoa.h>

class TMacMonitor;
@class TMacMonitorView;

@interface TCocoaMonitorController : NSWindowController <NSTextFieldDelegate, NSWindowDelegate>
{
	IBOutlet TMacMonitorView* view;
	IBOutlet NSButton* stopStartButton;
	IBOutlet NSButton* stepIntoButton;
	IBOutlet NSButton* stepOverButton;

	TMacMonitor* monitor;
}

- (void)update;

- (void)addHistoryLine:(NSString *)line type:(int)type;

- (IBAction)stopStart:(id)sender;
- (IBAction)stepInto:(id)sender;
- (IBAction)stepOver:(id)sender;

- (void)executeCommand:(NSString*)command;
- (void)setMonitor:(TMacMonitor*)inMonitor;

@end
