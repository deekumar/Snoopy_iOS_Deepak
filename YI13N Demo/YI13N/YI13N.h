//
//  YI13N.h
//  ymobile_instrumentation
//
//  Created by Tim Tully on Jan 12, 2011.
//  Copyright 2011 Yahoo!. All rights reserved.
//


#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

typedef enum {
	YI13NLogLevelInternal = 0,
	YI13NLogLevelError = 1,
	YI13NLogLevelWarn = 2,
	YI13NLogLevelInfo = 3,
	YI13NLogLevelDebug = 4,
	YI13NLogLevelTrace = 5
} YI13NLogLevel;

typedef enum{
	YI13NEventBufferTypeInMemory,
	YI13NEventBufferTypeLogfile,  // Not actually usable at the moment, preserved for posterity
	YI13NEventBufferTypeSQLite
} YI13NEventBufferType; 


/**
 * The YI13NDelegate is a protocol that is used by delegates for YI13N.  
 * 
 * Only one entry point: dataUploaded:.  This delegate can be used to receive callbacks when data is uploaded
 * by the library.
 */
@protocol YI13NDelegate
   @required
    /**
     * @param msg msg is a JSON encoded string containing the data payload you can parse containing response data
     * from the YQL table.  
     */
    -(void) dataUploaded:(NSString*) msg;
@end

@class YI13NPrivate;

// Constants when logging events
extern NSString *const YI13NULTKeyNameTimestamp;
extern NSString *const YI13NULTKeyNameSection;
extern NSString *const YI13NULTKeyNameLink;
extern NSString *const YI13NULTKeyNamePosition;
extern NSString *const YI13NULTKeyNamePVID;

// Optional Config options that will overwrite what is in the User-Agent
extern NSString *const YI13NConfigOptionAppName;
extern NSString *const YI13NConfigOptionAppVersion;

// Mandatory App level spaceid used for things like app start|stop
extern NSString *const YI13NConfigOptionAppSpaceid;

// Frequency by which buffer is flushed by background thread

/** The frequency with which the background thread should flush data buffers */
extern NSString *const YI13NConfigOptionFlushFrequencySeconds;
extern NSString *const YI13NConfigOptionMaxInMemoryBufferSizeBytes;
extern NSString *const YI13NConfigOptionMaxInMemoryEventSizeBytes;
extern NSString *const YI13NConfigOptionMaxDatabaseMaxNumEvents;

// which YQL environment to send data to?
extern NSString *const YI13NConfigOptionYQLMode;
// The YWA Project Id to send data to
extern NSString *const YI13NConfigOptionYWAProjectID;
// This is deprecated as of 3.0, but left in the header for compatibility
extern NSString *const YI13NConfigOptionDisableBackgroundThread;
extern NSString *const YI13NConfigOptionAllowRapid;
extern NSString *const YI13NConfigOptionUseCompression;
extern NSString *const YI13NConfigOptionCompressionLevel;
// This is deprecated as of 3.0, but left in the header for compatibility
extern NSString *const YI13NConfigOptionDisableFGBGEvents;
// Enables logging over https rather than http. (Data sent to https YQL)
extern NSString *const YI13NConfigOptionUseHTTPS;
// Turns on console logging output
extern NSString *const YI13NConfigOptionEnableConsoleLogging;
// Puts library in debug mode - provides more extensive diagnostics and stats from YQL
extern NSString *const YI13NConfigOptionDebugMode;
// The default name you want to log for the screen view we log implicitly on app start
extern NSString *const YI13NConfigOptionSplashScreenName;

// Config Values
// Use YQL debug environment
extern NSString *const YI13NConfigValueYQLModeDebug;
extern NSString *const YI13NConfigValueYQLModeStaging;
extern NSString *const YI13NConfigValueYQLModeProd;
extern NSString *const YI13NConfigValueYQLModeCustom;
extern NSString *const YI13NConfigValueYQLCustomHostname;
extern NSString *const YI13NConfigValueCompressionDefault;
extern NSString *const YI13NConfigValueCompressionBest;
extern NSString *const YI13NConfigValueCompressionSpeed;

/**
 * YI13N is a singleton used to buffer screenview and event data to the device, which is then buffered and batch uploaded
 * to YQL. 
 *
 * To get a pointer to it, simply call sharedInstance, but be sure that startWithBufferType:andConfig: get called at most
 * once, preferrably during app startup.  Once you have a pointer to the instance, make calls to any variation of the
 * logScreenView: or logEventWithSpaceid: style selectors, of which there are *many* variations.
 *
 * For a list of configuration object key names and descriptions, please see the 2.xx docs for now, until we can 
 * hack appledocs to show constants, or update the 2.xx docs: http://i13n.corp.yahoo.com/yi13n/ios.php
 *
 */
@interface YI13N : NSObject {
@private
	NSDictionary *config;
	BOOL startCalled;
    BOOL reportLocation;
	YI13NPrivate *private_;
	NSDictionary *cookieJarContext;
	NSString *currentPVID;
    NSUInteger appSpaceid;
    id<YI13NDelegate> delegate;
    BOOL enableConsoleLogging;
    BOOL debug;
}

/** @name Accessing and Initializing a YI13N instance */

/**
 * Returns a pointer to the singleton instance.
 *
 * Ensure that you call startWithBufferType:andConfig: once at some point in order to actually *startup* the lib.
 *
 * @return Returns a pointer to the YI13N Singleton.
 **/
+ (YI13N*)sharedInstance;

/**
 * Starts and initializes the library: this starts up the background thread and instantiates the buffering
 * mechanism used to buffer data between flushes to YQL.
 *
 * @param type The YI13NEventBufferType for the library - used to determine how you want to buffer data.
 * @param conf The NSDictionary instance containing key/value pairs used to configure how you want to use the library.
 *
 * @return Returns success/failure of calling this selector.  
 */
- (BOOL)startWithBufferType:(YI13NEventBufferType) type andConfig:(NSDictionary*) conf;

/** @name Logging Screenviews/Pageviews */

/**
 * Logs a "Screen View" record in ULT and YWA.  
 * 
 * A screenview can be thought as being similar to the pageview concept in the desktop world.  Since this particular
 * selector has no spaceid associated, the default one you specified in your conf object is used instead.
 * @param screenName The string representation of a logical "screen" or "page" that is shown in your app.
 */
- (void) logScreenview:(NSString*) screenName;

/**
 * Logs a "Screen View" record in ULT and YWA.
 *
 * @param screenName The string representation of a logical "screen" or "page" that is shown in your app.
 * @param spaceid The spaceid representing the screen the user is seeing.
 */
- (void) logScreenview:(NSString*) screenName withSpaceid:(NSUInteger) spaceid;
/**
 * Logs a "Screen View" record in ULT and YWA.
 *
 * @param screenName The string representation of a logical "screen" or "page" that is shown in your app.
 * @param pageparams A map representing a set of key/value pairs that describe the "context" of your screen view.  The 
 * number of pairs is not necessarily limited, but the strlen(key) must be <= 8. 
 */
- (void) logScreenview:(NSString*) screenName withPageParams:(NSDictionary*) pageparams;

/**
 * Logs a "Screen View" record in ULT and YWA.
 *
 * @param screenName The string representation of a logical "screen" or "page" that is shown in your app.
 * @param spaceid The spaceid representing the screen the user is seeing.
 * @param pageparams A map representing a set of key/value pairs that describe the "context" of your screen view.  The
 * number of pairs is not necessarily limited, but the strlen(key) must be <= 8.
 */
- (void) logScreenview:(NSString*) screenName withSpaceid:(NSUInteger) spaceid
        withPageParams:(NSDictionary*) pageparams;

/**
 * Logs a "Screen View" record in ULT and YWA.
 *
 * @param screenName The string representation of a logical "screen" or "page" that is shown in your app.
 * @param spaceid The spaceid representing the screen the user is seeing.
 * @param pageparams A map representing a set of key/value pairs that describe the "context" of your screen view.  The
 * number of pairs is not necessarily limited, but the strlen(key) must be <= 8.
 * @param level The log level for the message that prioritizes this record within the internal buffering.  At this moment 
 * the value is not respected.
 */
- (void) logScreenview:(NSString*) screenName withSpaceid:(NSUInteger) spaceid
        withPageParams:(NSDictionary*) pageparams withPriority:(YI13NLogLevel) level;

/**
 * Logs a "Screen View" record in ULT and YWA.
 *
 * @param screenName The string representation of a logical "screen" or "page" that is shown in your app.
 * @param spaceid The spaceid representing the screen the user is seeing.
 * @param pageparams A map representing a set of key/value pairs that describe the "context" of your screen view.  The
 * number of pairs is not necessarily limited, but the strlen(key) must be <= 8.
 * @param linkdata An array of NSDictionary where each dictionary instance represents a given link that you want to track.
 * A link is a map of key/value pairs that describe it.
 */
- (void) logScreenview:(NSString*) screenName withSpaceid:(NSUInteger) spaceid
        withPageParams:(NSDictionary*) pageparams withLinks:(NSArray*) linkdata;
/**
 * Logs a "Screen View" record in ULT and YWA.
 *
 * @param screenName The string representation of a logical "screen" or "page" that is shown in your app.
 * @param spaceid The spaceid representing the screen the user is seeing.
 * @param pageparams A map representing a set of key/value pairs that describe the "context" of your screen view.  The
 * number of pairs is not necessarily limited, but the strlen(key) must be <= 8.
 * @param linkdata An array of NSDictionary where each dictionary instance represents a given link that you want to track.
 * A link is a map of key/value pairs that describe it.
 * @param level The log level for the message that prioritizes this record within the internal buffering.  At this moment
 * the value is not respected.
 */
- (void) logScreenview:(NSString*) screenName withSpaceid:(NSUInteger) spaceid
        withPageParams:(NSDictionary*) pageparams withLinks:(NSArray*) linkdata withPriority:(YI13NLogLevel) level;

/**
 * Logs an exception.  A message and stacktrace is derived from the exception parameter.
 * @param exception The NSException instance that was received by the uncaught exception handler. To properly use this,
 * call NSSetUncaughtExceptionHandler and pass it a pointer to a function that takes an NSException* as its 
 * argument.  Then pass in the pointer to logException:.
 */
- (void) logException:(NSException *)exception;


/** @name Logging Clicks on Links in the App */

/**
 * Logs a click in ULT and YWA.  
 *
 * @param spaceid The spaceid representing the screen the user is seeing.
 * @param pageparams A map representing a set of key/value pairs that describe the "context" of your screen view.  The
 * number of pairs is not necessarily limited, but the strlen(key) must be <= 8.
 * @param clickinfo A map representing a set of key/value pairs that describe the click that occurred in your app. The
 * number of pairs is not necessarily limited, but the strlen(key) must be <= 8.
 */
- (void) logClickWithSpaceid:(NSUInteger) spaceid withPageParams:(NSDictionary*) pageparams
               withClickInfo:(NSDictionary*) clickinfo;
/**
 * Logs a click in ULT and YWA.
 *
 * @param spaceid The spaceid representing the screen the user is seeing.
 * @param pageparams A map representing a set of key/value pairs that describe the "context" of your screen view.  The
 * number of pairs is not necessarily limited, but the strlen(key) must be <= 8.
 * @param clickinfo A map representing a set of key/value pairs that describe the click that occurred in your app. The
 * number of pairs is not necessarily limited, but the strlen(key) must be <= 8.
 * @param level The log level for the message that prioritizes this record within the internal buffering.  At this moment
 * the value is not respected.
 */
- (void) logClickWithSpaceid:(NSUInteger) spaceid withPageParams:(NSDictionary*) pageparams
               withClickInfo:(NSDictionary*) clickinfo withPriority:(YI13NLogLevel) level;

// End logClick

/** @name Logging Events That Occur in the App */

/**
 * Logs an event that occurs in your app. What an event entails is mostly up to you and is mostly freeform (you can
 * always query the data on the grid.)
 *
 * Example events are login/logouts, taps, overlays, ViewController transitions,
 * etc.
 *
 * By default the library will associate the app-level spaceid you specified in your config object as the
 * spaceid associated with this event.
 *
 * @param name The name of the event that occurred.
 */
- (void) logEvent:(NSString*) name;

/**
 * Logs an event that occurs in your app. What an event entails is mostly up to you and is mostly freeform (you can
 * always query the data on the grid.)
 *
 * Example events are login/logouts, taps, overlays, ViewController transitions,
 * etc.
 *
 * By default the library will associate the app-level spaceid you specified in your config object as the
 * spaceid associated with this event.
 *
 * @param name The name of the event that occurred.
 * @param pageparams The key/value pairs representing the "page parameters", or context of the app when this event
 * occurred.
 */
- (void) logEvent:(NSString*) name withPageParams:(NSDictionary*) pageparams;

/**
 * Logs an event that occurs in your app. What an event entails is mostly up to you and is mostly freeform (you can 
 * always query the data on the grid.)  
 *
 * Example events are login/logouts, taps, overlays, ViewController transitions,
 * etc.
 *
 * @param spaceid The spaceid on the screen where the event occurred.
 * @param name The name of the event that occurred.
 */
- (void) logEventWithSpaceid:(NSUInteger) spaceid withName:(NSString*) name;
/**
 * Logs an event that occurs in your app. What an event entails is mostly up to you and is mostly freeform (you can
 * always query the data on the grid.)
 *
 * Example events are login/logouts, taps, overlays, ViewController transitions,
 * etc.
 *
 * @param spaceid The spaceid on the screen where the event occurred.
 * @param name The name of the event that occurred.
 * @param level The priority of the event that occurred.  This value is currently not respected.
 */
- (void) logEventWithSpaceid:(NSUInteger) spaceid withName:(NSString*) name withPriority:(YI13NLogLevel) level;

/**
 * Logs an event that occurs in your app. What an event entails is mostly up to you and is mostly freeform (you can
 * always query the data on the grid.)
 *
 * Example events are login/logouts, taps, overlays, ViewController transitions,
 * etc.
 *
 * @param spaceid The spaceid on the screen where the event occurred.
 * @param name The name of the event that occurred.
 * @param pageparams The key/value pairs representing the "page parameters", or context of the app when this event 
 * occurred.
 */
- (void) logEventWithSpaceid:(NSUInteger) spaceid withName:(NSString*) name withPageParams:(NSDictionary*) pageparams;

/**
 * Logs an event that occurs in your app. What an event entails is mostly up to you and is mostly freeform (you can
 * always query the data on the grid.)
 *
 * Example events are login/logouts, taps, overlays, ViewController transitions,
 * etc.
 *
 * @param spaceid The spaceid on the screen where the event occurred.
 * @param name The name of the event that occurred.
 * @param pageparams The key/value pairs representing the "page parameters", or context of the app when this event
 * occurred.
 * @param level The priority of the event that occurred.  This value is currently not respected.
 */
- (void) logEventWithSpaceid:(NSUInteger) spaceid withName:(NSString*) name withPageParams:(NSDictionary*) pageparams
                withPriority:(YI13NLogLevel) level;

/**
 * Logs an event that occurs in your app. What an event entails is mostly up to you and is mostly freeform (you can
 * always query the data on the grid.)
 *
 * Example events are login/logouts, taps, overlays, ViewController transitions,
 * etc.
 *
 * @param spaceid The spaceid on the screen where the event occurred.
 * @param name The name of the event that occurred.
 * @param pageparams The key/value pairs representing the "page parameters", or context of the app when this event
 * occurred.
 * @param linkdata An array of NSDictionary where each dictionary instance represents a given link that you want to track.
 * A link is a map of key/value pairs that describe it.
 */
- (void) logEventWithSpaceid:(NSUInteger) spaceid withName:(NSString*) name withPageParams:(NSDictionary*) pageparams
                   withLinks:(NSArray*) linkdata;

/**
 * Logs an event that occurs in your app. What an event entails is mostly up to you and is mostly freeform (you can
 * always query the data on the grid.)
 *
 * Example events are login/logouts, taps, overlays, ViewController transitions,
 * etc.
 *
 * @param spaceid The spaceid on the screen where the event occurred.
 * @param name The name of the event that occurred.
 * @param pageparams The key/value pairs representing the "page parameters", or context of the app when this event
 * occurred.
 * @param linkdata An array of NSDictionary where each dictionary instance represents a given link that you want to track.
 * A link is a map of key/value pairs that describe it.
 * @param level The priority of the event that occurred.  This value is currently not respected.
 */
- (void) logEventWithSpaceid:(NSUInteger) spaceid withName:(NSString*) name withPageParams:(NSDictionary*) pageparams
                   withLinks:(NSArray*) linkdata withPriority:(YI13NLogLevel) level;

/**
 * Logs an event that occurs in your app. What an event entails is mostly up to you and is mostly freeform (you can
 * always query the data on the grid.)
 *
 * Example events are login/logouts, taps, overlays, ViewController transitions,
 * etc.
 *
 * @param spaceid The spaceid on the screen where the event occurred.
 * @param name The name of the event that occurred.
 * @param outcome The outcome that occurred based on the event, in other words, the outcome that came about from the
 * event.
 */
- (void) logEventWithSpaceid:(NSUInteger) spaceid withName:(NSString*) name withOutcome:(NSString*) outcome;

/**
 * Logs an event that occurs in your app. What an event entails is mostly up to you and is mostly freeform (you can
 * always query the data on the grid.)
 *
 * Example events are login/logouts, taps, overlays, ViewController transitions,
 * etc.
 *
 * @param spaceid The spaceid on the screen where the event occurred.
 * @param name The name of the event that occurred.
 * @param pageparams The key/value pairs representing the "page parameters", or context of the app when this event
 * occurred.
 * @param outcome The outcome that occurred based on the event, in other words, the outcome that came about from the
 * event.
 */
- (void) logEventWithSpaceid:(NSUInteger) spaceid withName:(NSString*) name withPageParams:(NSDictionary*) pageparams
                 withOutcome:(NSString*) outcome;

/**
 * Logs an event that occurs in your app. What an event entails is mostly up to you and is mostly freeform (you can
 * always query the data on the grid.)
 *
 * Example events are login/logouts, taps, overlays, ViewController transitions,
 * etc.
 *
 * @param spaceid The spaceid on the screen where the event occurred.
 * @param name The name of the event that occurred.
 * @param level The priority of the event that occurred.  This value is currently not respected.
 * @param outcome The outcome that occurred based on the event, in other words, the outcome that came about from the
 * event.
 */
- (void) logEventWithSpaceid:(NSUInteger) spaceid withName:(NSString*) name withPriority:(YI13NLogLevel) level
                 withOutcome:(NSString*) outcome;

/** @name Location Tracking */

/**
 * Sets the current location context for a user.
 *
 * If set, future events for the app user are associated with this latitude/longitude combination.
 * @param latitude The latitude of the user
 * @param longitude The longitude of the user
 */
/*
- (void) setLatitude:(CLLocationDegrees) latitude
           longitude:(CLLocationDegrees) longitude;
*/

/** @name Setting/Removing Batch Parameters in the App */

/**
 * Associated the param key with the param value and stores this pairing as a "Batch Parameter".  Batch parameters
 * are similar to ULT page parameters and ultimately wind up as page parameters.  The key difference is that
 * batch parameters are optimizations of page parameters in the sense that they're "factored out" page parameters.  Recall
 * that this library batches data.  An optimization would be to factor out common key/value pairs across rows to 
 * optimize space and data transmission costs.  This is what batch parameters are: factored out page parameters across rows.
 * 
 * When you know that a given key/value pair should appear in all rows' page parameters, then use batch parameters instead.
 *
 * @param key The key that will be stored for this batch parameter pairing.
 * @param value The value that will be stored with the key argument.
 */
- (void) setBatchParam:(id)value forKey:(NSString*) key;

/**
 *
 * @param key The key that is associated with the batch param pairing you'd like to remove.
 */
- (void) removeBatchParamWithKey:(NSString*) key;

/** @name OneTrack Setup */

/**
 * Sets the OneTrack property id necessary for your data to appear in OneTrack.
 * If you do not set this value, the data will not appear in reports.
 *
 * @param property The OneTrack property you've been assigned to associate with your data.
 */
- (void) setOneTrackPropertyId:(NSUInteger) property;

- (NSString*) userAgent;

/** @name Library Control */


/**
 * Shuts down the YI13N instance, which really means it stops the background
 * flushing thread and drains any existing in-memory buffers and resets the state.
 *
 * This shouldn't be needed to be used for many reasons, but one strong use case is when you want to 
 * reset the library between Yahoo login sessions.
 *
 **/
- (void) shutdown;

/**
 * Manually invoke a flush of event buffer contents.  
 *
 * Calling this method is not typically necessary
 * since the background thread that the library spins up at startup time will call this periodically.
 * Internally the library is calling this function on your behalf in the background thread.
 **/
- (void)flush;


/**
 * This sets the YI13NDelegate.  Note that we are NOT retaining the newDelegate param.
 * 
 * @param newDelegate The YI13NDelegate that conforms to YI13NDelegate and whose selector is called when
 * data is uploaded to YQL.
 **/
- (void)setDelegate:(id<YI13NDelegate>)newDelegate;

/** @name YI13NDelegate Accessors */


/**
 * Accessor for the YI13NDelegate.
 **/
- (id)delegate;



/**
 * Returns a configuration option for the given key.
 *
 * @param key The key that maps to the value to be retrieved.
 **/
- (id) configOption:(NSString*) key;

@property (nonatomic, assign) BOOL startCalled;
@property BOOL enableConsoleLogging, reportLocation;
@property BOOL debug;
@property (readonly) NSDictionary *configuration;
@property (nonatomic, readonly) NSUInteger appSpaceid;
@property (copy) NSDictionary *cookieJarContext;
@property (copy, readonly) NSString *currentPVID;
@end
