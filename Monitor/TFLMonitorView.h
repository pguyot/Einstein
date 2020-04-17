//
//  TMacMonitorView.h
//  Einstein
//
//  Created by Steven Frank on 6/29/15.
//
//

#if 0 // rewrite this for FLTK

#import <Cocoa/Cocoa.h>

class TMacMonitor;

@interface TMacMonitorView : NSView

- (void)setController:(TCocoaMonitorController *)controller;

- (void)addHistoryLine:(NSString *)line type:(int)type;

- (void)updateWithMonitor:(TMacMonitor *)monitor;

@end

#endif
