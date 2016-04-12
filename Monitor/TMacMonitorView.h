//
//  TMacMonitorView.h
//  Einstein
//
//  Created by Steven Frank on 6/29/15.
//
//

#import <Cocoa/Cocoa.h>

class TMacMonitor;

@interface TMacMonitorView : NSView

- (void)setController:(TCocoaMonitorController *)controller;

- (void)addHistoryLine:(NSString *)line type:(int)type;

- (void)updateWithMonitor:(TMacMonitor *)monitor;

@end
