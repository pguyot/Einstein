//
//  TCocoaPathHelper.m
//  Einstein
//
//  Created by Steven Frank on 2/20/17.
//
//

#include <string>
#import <Foundation/Foundation.h>

#import "TCocoaPathHelper.h"

NSString *TCocoaPathHelper_ApplicationSupportDirectory();

NSString *TCocoaPathHelper_FindOrCreateDirectory(NSSearchPathDirectory searchPathDirectory, NSSearchPathDomainMask domainMask, NSString *appendComponent, NSError **errorOut);


std::string
TCocoaPathHelper::GetSerialPipeBasePath()
{
	NSString *cocoaString = TCocoaPathHelper_ApplicationSupportDirectory();
	return std::string([cocoaString fileSystemRepresentation]);
}


NSString *
TCocoaPathHelper_FindOrCreateDirectory(NSSearchPathDirectory searchPathDirectory, NSSearchPathDomainMask domainMask, NSString *appendComponent, NSError **errorOut)
{
	// Search for the path
	NSArray* paths = NSSearchPathForDirectoriesInDomains(searchPathDirectory, domainMask, YES);
	if ( [paths count] == 0 )
	{
		return nil;
	}

	// Normally only need the first path
	NSString *resolvedPath = [paths objectAtIndex:0];

	if ( appendComponent )
	{
		resolvedPath = [resolvedPath stringByAppendingPathComponent:appendComponent];
	}

	// Create the path if it doesn't exist
	NSError *error;
	BOOL success = [[NSFileManager defaultManager] createDirectoryAtPath:resolvedPath withIntermediateDirectories:YES attributes:nil error:&error];
	if ( !success )
	{
		if ( errorOut )
		{
			*errorOut = error;
		}
		return nil;
	}

	// If we've made it this far, we have a success
	if ( errorOut )
	{
		*errorOut = nil;
	}
	return resolvedPath;
}


NSString *
TCocoaPathHelper_ApplicationSupportDirectory()
{
	//NSString *executableName = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleExecutable"];
	NSError *error = nil;
	NSString *result = TCocoaPathHelper_FindOrCreateDirectory(NSApplicationSupportDirectory, NSUserDomainMask, nil, &error);
	if ( error )
	{
		NSLog(@"Unable to find or create application support directory:\n%@", error);
	}
	return result;
}


