// ==============================
// File:			TCocoaListenerWindow.mm
// Project:			Einstein
//
// Copyright 2015 Steve White.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

#import "TCocoaListenerWindow.h"
#import "TCocoaFileManager.h"

@interface TCocoaListenerWindow ()<NSTextViewDelegate> {
	NSMutableString *_pendingInput;
}

@property (assign) IBOutlet NSTextView *textView;
@property (strong, nonatomic) NSDictionary *textAttributes;

@end

@implementation TCocoaListenerWindow

- (instancetype) init {
	return [self initWithWindowNibName:NSStringFromClass([self class])];
}

- (void) dealloc {
	self.textAttributes = nil;
	self.title = nil;

#if !__has_feature(objc_arc)
	[_pendingInput release], _pendingInput = nil;
	[super dealloc];
#endif
}

- (void)windowDidLoad {
	[super windowDidLoad];
	
	_pendingInput = [[NSMutableString alloc] init];
	
	NSTextView *textView = self.textView;
	textView.font = [NSFont fontWithName:@"Menlo-Regular" size:11];
	// Yes, I set these in IB. But quote substiution was still YES for some reason...
	textView.automaticQuoteSubstitutionEnabled = NO;
	textView.automaticLinkDetectionEnabled = NO;
	textView.automaticDataDetectionEnabled = NO;
	textView.automaticDashSubstitutionEnabled = NO;
	textView.automaticTextReplacementEnabled = NO;
	textView.automaticSpellingCorrectionEnabled = NO;
	
	self.textAttributes = [[NSDictionary alloc] initWithObjectsAndKeys:textView.font, NSFontAttributeName, nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(windowWillClose:)
												 name:NSWindowWillCloseNotification
											   object:self.window];
}

- (IBAction) windowWillClose:(NSNotification *)aNotification {
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:NSWindowWillCloseNotification
												  object:self.window];

	TCocoaFileManager *fileManager = self.fileManager;
	fileManager->listener_was_closed(self.newt_fdesc);
}

- (NSString *) windowTitleForDocumentDisplayName:(NSString *)displayName {
	return self.title;
}

- (void) setTitle:(NSString *)title {
	_title = title;
	self.window.title = title;
}

- (void) appendString:(NSString *)string {
	if ([NSThread isMainThread] == NO) {
		[self performSelectorOnMainThread:_cmd
							   withObject:string
							waitUntilDone:NO];
		return;
	}
	
	NSTextView *textView = self.textView;
	NSAttributedString *attrString = [[NSAttributedString alloc] initWithString:string
																	 attributes:self.textAttributes];
	[textView.textStorage appendAttributedString:attrString];
#if !__has_feature(objc_arc)
	[attrString autorelease];
#endif
	
	// XXX: Be smarter. If the user has scrolled away, don't scroll back to the bottom
	[textView scrollToEndOfDocument:self];
}

- (BOOL)textView:(NSTextView *)textView shouldChangeTextInRange:(NSRange)affectedCharRange replacementString:(NSString *)replacementString;
{
	if (NSMaxRange(affectedCharRange) == [self.textView.textStorage length]) {
		BOOL shouldNotify = NO;
		BOOL shouldAccept = NO;
		@synchronized(_pendingInput) {
			if ([replacementString length] == 0) {
				NSRange deletionRange;
				deletionRange.length = MIN(affectedCharRange.length, _pendingInput.length);
				if (deletionRange.length > 0) {
					deletionRange.location = _pendingInput.length - deletionRange.length;
					[_pendingInput deleteCharactersInRange:deletionRange];
					shouldAccept = YES;
				}
			}
			else {
				[_pendingInput appendString: replacementString];
				shouldAccept = YES;
			}
			
			shouldNotify = ([_pendingInput rangeOfString:@"\n"].location != NSNotFound);
		}
		
		if (shouldNotify == YES) {
			TCocoaFileManager *fileManager = self.fileManager;
			fileManager->set_listener_has_input(self.newt_fdesc, true);
		}

		return shouldAccept;
	}
	
	NSRange range = { [[textView string] length], 0 };
	if ([replacementString length] == 0) {
		// we won't allow you to delete text in the middle...
		// but we'll still want to move the cursor to the end
	}
	else {
		// But inserting text, we'll allow it, but have to move to
		// the end of the text
		[textView insertText:replacementString replacementRange:range];
		range.location += [replacementString length];
	}
	
	[textView setSelectedRange: range];
	return NO;
}

- (KSInt32) writeInputIntoBuffer:(void *)buffer
					   maxLength:(KUInt32)maxLength
{
	NSUInteger availableData = 0;
	@synchronized(_pendingInput) {
		availableData = [_pendingInput length];
	}
	
	if (availableData == 0) {
		return 0;
	}
	
	NSUInteger readLength = MIN(maxLength - 1, availableData);
	@synchronized(_pendingInput) {
		NSRange leftover = NSMakeRange(NSNotFound,0);
		[_pendingInput getBytes:buffer
					  maxLength:maxLength
					 usedLength:&readLength
					   encoding:NSASCIIStringEncoding
						options:0
						  range:NSMakeRange(0, readLength)
				 remainingRange:&leftover];

		if (leftover.location != NSNotFound) {
			[_pendingInput deleteCharactersInRange:NSMakeRange(0, leftover.location)];
		}
	}
	
	if (availableData == readLength) {
		TCocoaFileManager *fileManager = self.fileManager;
		fileManager->set_listener_has_input(self.newt_fdesc, false);
	}
	
	return (KSInt32) readLength + 1;
}

@end
