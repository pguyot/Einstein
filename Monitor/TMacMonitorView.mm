//
//  TMacMonitorView.mm
//  Einstein
//
//  Created by Steven Frank on 6/29/15.
//
//

#import "TCocoaMonitorController.h"
#import "TMacMonitorView.h"
#import "TMacMonitor.h"

const int historyVisible = 32;
const int historySize = 1024;

@interface TMacMonitorView ()
{
	BOOL started;
	BOOL halted;
	NSString *alarm;
	NSString *intRaised;
	NSString *intCtrlReg;
	NSString *regVal[16];
	BOOL regValChanged[16];
	NSString *tmRegVal[16];
	NSString *cpsr;
	NSString *spsr;
	NSString *frozenTimer;
	NSString *rtc;
	NSString *fm;
	NSString *ic1;
	NSString *ic2;
	NSString *ic3;
	NSString *symbol;
	NSAttributedString *disasm[5];
	NSAttributedString *history[historySize];
	CGFloat historyOffset;
	NSMutableString *command;

	TCocoaMonitorController *controller;

	NSFont *font;
	NSDictionary *labelAttrs;
	NSDictionary *symbolAttrs;
	NSDictionary *valueAttrs;
	NSDictionary *changedValueAttrs;
	NSDictionary *disasmAddrAttrs;
	NSDictionary *disasmBaseHighlightedAttrs;
	NSDictionary *disasmBaseAttrs;
	NSDictionary *disasmStatusAttrs;
	NSDictionary *errorAttrs;
}
@end

#define FONT_HEIGHT 13
#define FONT_WIDTH 7


@implementation TMacMonitorView

- (id)initWithFrame:(NSRect)frameRect
{
	if ( self = [super initWithFrame:frameRect] )
	{
		font = [NSFont userFixedPitchFontOfSize:11.0];
#if !__has_feature(objc_arc)
		[font retain];
#endif

		labelAttrs =
		@{
		  NSFontAttributeName: font,
		  NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.22 green:0.75 blue:1.0 alpha:1.0]
		  };
#if !__has_feature(objc_arc)
		[labelAttrs retain];
#endif

		symbolAttrs =
		@{
		  NSFontAttributeName: font,
		  NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.22 alpha:1.0]
		  };
#if !__has_feature(objc_arc)
		[symbolAttrs retain];
#endif
		valueAttrs =
		@{
		  NSFontAttributeName: font,
		  NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.7 green:0.7 blue:0.7 alpha:1.0]
		  };
#if !__has_feature(objc_arc)
		[valueAttrs retain];
#endif
		changedValueAttrs =
		@{
		  NSFontAttributeName: font,
		  NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0]
		  };

#if !__has_feature(objc_arc)
		[changedValueAttrs retain];
#endif

		disasmAddrAttrs =
		@{
		  NSFontAttributeName: font,
		  NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.5 green:0.7 blue:0.3 alpha:1.0]
		  };
#if !__has_feature(objc_arc)
		[disasmAddrAttrs retain];
#endif
		disasmBaseHighlightedAttrs =
		@{
		  NSFontAttributeName: font,
		  NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.8 green:1.0 blue:0.6 alpha:1.0],
		  };
#if !__has_feature(objc_arc)
		[disasmBaseHighlightedAttrs retain];
#endif
		disasmBaseAttrs =
		@{
		  NSFontAttributeName: font,
		  NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.1 green:0.7 blue:0.1 alpha:1.0]
		  };
#if !__has_feature(objc_arc)
		[disasmBaseAttrs retain];
#endif
		disasmStatusAttrs =
		@{
		  NSFontAttributeName: font,
		  NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.8 green:0.5 blue:0.2 alpha:1.0]
		  };

#if !__has_feature(objc_arc)
		[disasmStatusAttrs retain];
#endif
		errorAttrs =
		@{
		  NSFontAttributeName: font,
		  NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.8 green:0.2 blue:0.2 alpha:1.0]
		  };
#if !__has_feature(objc_arc)
		[errorAttrs retain];
#endif

		command = [[NSMutableString alloc] init];

		historyOffset = (CGFloat)(historySize - historyVisible);
	}

	return self;
}


- (BOOL)acceptsFirstResponder
{
	return YES;
}


- (void)addHistoryLine:(NSString *)line type:(int)type
{
	// Reset scrolling history to bottom line
	historyOffset = (CGFloat)(historySize - historyVisible);

	// Scroll up
#if !__has_feature(objc_arc)
	[history[0] release];
#endif
	for ( int i = 1; i < historySize; ++i )
	{
		history[i - 1] = history[i];
	}

	// Add new

	if ( type == MONITOR_LOG_ERROR )
	{
		history[historySize-1] = [[NSAttributedString alloc] initWithString:line attributes:errorAttrs];
	}
	else if ( type == MONITOR_LOG_CODE )
	{
		history[historySize-1] = [[NSAttributedString alloc] initWithString:line attributes:disasmBaseAttrs];
	}
	else if ( type == MONITOR_LOG_USER_INPUT )
	{
		history[historySize-1] = [[NSAttributedString alloc] initWithString:line attributes:labelAttrs];
	}
	else
	{
		history[historySize-1] = [[NSAttributedString alloc] initWithString:line attributes:valueAttrs];
	}
}


- (BOOL)canBecomeKeyView
{
	return YES;
}


- (void)drawRect:(NSRect)dirtyRect
{
	[super drawRect:dirtyRect];

	NSRect frame = [self frame];

	[[NSColor blackColor] set];
	NSRectFill(frame);

	if ( halted )
	{
		NSPoint textPoint = frame.origin;
		textPoint.x += 4;
		textPoint.y = textPoint.y + frame.size.height - FONT_HEIGHT - 1;

		[@"  R0" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  R1" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  R2" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  R3" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  R4" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  R5" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  R6" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  R7" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  R8" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  R9" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" R10" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" R11" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" R12" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" R13" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  LR" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  PC" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"CPSR" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"SPSR" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		// blank
		textPoint.y -= FONT_HEIGHT;

		[@" Tmr" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" TM0" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" TM1" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" TM2" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" TM3" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" RTC" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" Alm" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  IR" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" ICR" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@"  FM" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" IC1" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" IC2" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		[@" IC3" drawAtPoint:textPoint withAttributes:labelAttrs];
		textPoint.y -= FONT_HEIGHT;

		// Disassembly

		// blank
		textPoint.y -= FONT_HEIGHT;

		[symbol drawAtPoint:textPoint withAttributes:symbolAttrs];
		textPoint.y -= FONT_HEIGHT;

		textPoint.x += FONT_WIDTH * 5;

		for ( int i = 0; i < 5; ++i )
		{
			NSMutableAttributedString *tempStr = [[NSMutableAttributedString alloc] initWithAttributedString:disasm[i]];

			if ( i == 0 )
			{
				[tempStr setAttributes:disasmBaseHighlightedAttrs range:NSMakeRange(0, [tempStr length])];
			}
			else
			{
				[tempStr setAttributes:disasmBaseAttrs range:NSMakeRange(0, [tempStr length])];
			}

			[tempStr setAttributes:disasmAddrAttrs range:NSMakeRange(0, 8)];

			NSRange statusRange = [[tempStr string] rangeOfString:@"  ="];
			if ( statusRange.location != NSNotFound )
			{
				statusRange.length = [tempStr length] - statusRange.location;
				[tempStr setAttributes:symbolAttrs range:statusRange];
			}

			statusRange = [[tempStr string] rangeOfString:@"(will skip)"];
			if ( statusRange.location != NSNotFound )
				[tempStr setAttributes:disasmStatusAttrs range:statusRange];

			statusRange = [[tempStr string] rangeOfString:@"(will do it)"];
			if ( statusRange.location != NSNotFound )
				[tempStr setAttributes:disasmStatusAttrs range:statusRange];

			[tempStr drawAtPoint:textPoint];

			textPoint.y -= FONT_HEIGHT;
		}

		// Register values

		textPoint = frame.origin;
		textPoint.x += 4 + (FONT_WIDTH * 5);
		textPoint.y = textPoint.y + frame.size.height - FONT_HEIGHT - 1;

		for ( int i = 0; i < 16; ++ i)
		{
			if ( regValChanged[i] )
				[regVal[i] drawAtPoint:textPoint withAttributes:changedValueAttrs];
			else
				[regVal[i] drawAtPoint:textPoint withAttributes:valueAttrs];

			textPoint.y -= FONT_HEIGHT;
		}

		[cpsr drawAtPoint:textPoint withAttributes:valueAttrs];
		textPoint.y -= FONT_HEIGHT;

		[spsr drawAtPoint:textPoint withAttributes:valueAttrs];
		textPoint.y -= FONT_HEIGHT;

		// Skip down to Tmr
		textPoint.y -= FONT_HEIGHT;

		[frozenTimer drawAtPoint:textPoint withAttributes:valueAttrs];
		textPoint.y -= FONT_HEIGHT;

		for ( int i = 0; i < 4; ++ i)
		{
			[tmRegVal[i] drawAtPoint:textPoint withAttributes:valueAttrs];
			textPoint.y -= FONT_HEIGHT;
		}

		[rtc drawAtPoint:textPoint withAttributes:valueAttrs];
		textPoint.y -= FONT_HEIGHT;

		[alarm drawAtPoint:textPoint withAttributes:valueAttrs];
		textPoint.y -= FONT_HEIGHT;

		[intRaised drawAtPoint:textPoint withAttributes:valueAttrs];
		textPoint.y -= FONT_HEIGHT;

		[intCtrlReg drawAtPoint:textPoint withAttributes:valueAttrs];
		textPoint.y -= FONT_HEIGHT;

		[fm drawAtPoint:textPoint withAttributes:valueAttrs];
		textPoint.y -= FONT_HEIGHT;

		[ic1 drawAtPoint:textPoint withAttributes:valueAttrs];
		textPoint.y -= FONT_HEIGHT;

		[ic2 drawAtPoint:textPoint withAttributes:valueAttrs];
		textPoint.y -= FONT_HEIGHT;

		[ic3 drawAtPoint:textPoint withAttributes:valueAttrs];
		textPoint.y -= FONT_HEIGHT;

		// History

		textPoint = frame.origin;
		textPoint.x += 4 + (FONT_WIDTH * 19);
		textPoint.y = textPoint.y + frame.size.height - FONT_HEIGHT - 1;

		for ( int i = 0; i < historyVisible; ++ i)
		{
			[history[i+(int)historyOffset] drawAtPoint:textPoint];
			textPoint.y -= FONT_HEIGHT;
		}

		// Command line

		textPoint = frame.origin;
		textPoint.x += 4;
		textPoint.y = textPoint.y + frame.size.height - (FONT_HEIGHT * 41) - 1;

		[[NSString stringWithFormat:@"> %@_", command] drawAtPoint:textPoint withAttributes:labelAttrs];

		// Separator lines

		[[NSColor colorWithCalibratedRed:0.25 green:0.25 blue:0.25 alpha:1.0] set];

		// Vertical, between registers and history

		[NSBezierPath strokeLineFromPoint:NSMakePoint(FONT_WIDTH * 18, frame.size.height - (FONT_HEIGHT * 32.5)) toPoint:NSMakePoint(FONT_WIDTH * 18, frame.size.height)];

		// Horizontal, between SPSR and Tmr

		[NSBezierPath strokeLineFromPoint:NSMakePoint(0, frame.size.height - (FONT_HEIGHT * 18.5)) toPoint:NSMakePoint(FONT_WIDTH * 18, frame.size.height - (FONT_HEIGHT * 18.5))];

		// Horizontal, between registers and disassembly

		[NSBezierPath strokeLineFromPoint:NSMakePoint(0, frame.size.height - (FONT_HEIGHT * 32.5)) toPoint:NSMakePoint(frame.size.width, frame.size.height - (FONT_HEIGHT * 32.5))];

		// Horizontal, between disassembly and input line

		[NSBezierPath strokeLineFromPoint:NSMakePoint(0, frame.size.height - (FONT_HEIGHT * 39.5)) toPoint:NSMakePoint(frame.size.width, frame.size.height - (FONT_HEIGHT * 39.5))];
	}
	else
	{
		NSPoint textPoint = frame.origin;
		textPoint.x += 20;
		textPoint.y += 20;

		if ( started )
		{
			[@"Emulator is running. Click Stop to stop it." drawAtPoint:textPoint withAttributes:labelAttrs];
		}
		else
		{
			[@"Emulator has not yet started." drawAtPoint:textPoint withAttributes:labelAttrs];
		}
	}
}


- (void)keyDown:(NSEvent *)theEvent
{
	if ( !started || !halted )
		return;

	if ( [[theEvent characters] characterAtIndex:0] == '\r' )
	{
		if ( [command length] > 0 )
		{
			[controller executeCommand:command];
			[command setString:@""];
			[self setNeedsDisplay:YES];
		}
		else
		{
			[controller stepInto:self];
		}
	}
	else if ( [[theEvent characters] characterAtIndex:0] == 0x7f )
	{
		if ( [command length] > 0 )
		{
			[command deleteCharactersInRange:NSMakeRange([command length] - 1, 1)];
			[self setNeedsDisplay:YES];
		}
	}
	else
	{
		[command appendString:[theEvent characters]];
		[self setNeedsDisplay:YES];
	}
}

- (void)scrollWheel:(NSEvent *)event
{
	if ( !started || !halted )
		return;

	CGFloat dy = [event scrollingDeltaY];
	if ([event hasPreciseScrollingDeltas])
		dy /= FONT_HEIGHT;
	if (dy!=0.0) {
		CGFloat prevOffset = historyOffset;
		historyOffset -= dy;
		if (historyOffset<0.0) historyOffset = 0.0;
		if (historyOffset>(int)(historySize-historyVisible))
			historyOffset = (CGFloat)(historySize-historyVisible);
		if (prevOffset!=historyOffset) [self setNeedsDisplay:YES];
	}
}

- (void)setController:(TCocoaMonitorController *)inController
{
	controller = inController;
}


- (void)updateWithMonitor:(TMacMonitor *)monitor
{
	started = YES;

	[self setNeedsDisplay:YES];

	halted = monitor->IsHalted();

	for ( int i = 0; i < 16; ++i )
	{
		NSString* newRegVal = [monitor->GetRegister(i) copy];
		regValChanged[i] = ![newRegVal isEqualToString:regVal[i]];
#if !__has_feature(objc_arc)
		[regVal[i] release];
#endif
		regVal[i] = newRegVal;
	}
#if !__has_feature(objc_arc)
	[cpsr release];
#endif
	cpsr = [monitor->GetCPSR() copy];
#if !__has_feature(objc_arc)
	[spsr release];
#endif
	spsr = [monitor->GetSPSR() copy];
#if !__has_feature(objc_arc)
	[frozenTimer release];
#endif
	frozenTimer = [monitor->GetFrozenTimer() copy];

	for ( int i = 0; i < 4; ++i )
	{
#if !__has_feature(objc_arc)
		[tmRegVal[i] release];
#endif
		tmRegVal[i] = [monitor->GetTimerMatchRegister(i) copy];
	}
#if !__has_feature(objc_arc)
	[rtc release];
#endif
	rtc = [monitor->GetRealTimeClock() copy];
#if !__has_feature(objc_arc)
	[alarm release];
#endif
	alarm = [monitor->GetAlarm() copy];
#if !__has_feature(objc_arc)
	[intRaised release];
#endif
	intRaised = [monitor->GetIntRaised() copy];
#if !__has_feature(objc_arc)
	[intCtrlReg release];
#endif
	intCtrlReg = [monitor->GetIntCtrlReg() copy];
#if !__has_feature(objc_arc)
	[fm release];
#endif
	fm = [monitor->GetFIQMask() copy];
#if !__has_feature(objc_arc)
	[ic1 release];
#endif
	ic1 = [monitor->GetIntEDReg1() copy];
#if !__has_feature(objc_arc)
	[ic2 release];
#endif
	ic2 = [monitor->GetIntEDReg2() copy];
#if !__has_feature(objc_arc)
	[ic3 release];
#endif
	ic3 = [monitor->GetIntEDReg3() copy];
#if !__has_feature(objc_arc)
	[symbol release];
#endif
	symbol = [monitor->GetSymbol() copy];

	for ( int i = 0; i < 5; ++i )
	{
#if !__has_feature(objc_arc)
		[disasm[i] release];
#endif
		disasm[i] = [[NSAttributedString alloc] initWithString:monitor->GetDisasmLine(i)];
	}
}

@end
