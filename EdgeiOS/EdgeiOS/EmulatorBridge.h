#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#include "constants.h"

NS_ASSUME_NONNULL_BEGIN

@interface EmulatorBridge : NSObject

// Singleton access
+ (EmulatorBridge *)sharedInstance;

// Public properties for button states
@property (nonatomic, assign) BOOL dpadUp;
@property (nonatomic, assign) BOOL dpadDown;
@property (nonatomic, assign) BOOL dpadLeft;
@property (nonatomic, assign) BOOL dpadRight;
@property (nonatomic, assign) BOOL buttonA;
@property (nonatomic, assign) BOOL buttonB;
@property (nonatomic, assign) BOOL buttonSelect;
@property (nonatomic, assign) BOOL buttonStart;

- (void)initializeSDL;
- (void)loadROM:(NSString *)romName;
- (void)advanceOneFrame;


// Pauses a running emulator.
- (void)pauseEmulator;
// Resumes a paused emulator.
- (void)resumeEmulator;
// End this session, saving if necessary.
- (void)endEmulator;

// Display.
- (const uint32_t *)pixels;

+ (void)copyBundledROMs;

+ (void)createDirectories;
+ (NSString *)romsDirectory;

@end

NS_ASSUME_NONNULL_END 
