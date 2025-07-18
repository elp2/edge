#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "SaveStateWrapper.h"

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

- (void)saveState;
- (void)loadPreviouslySavedState;
- (void)rewindState;
- (NSArray<SaveStateWrapper *> *)getSaveStates;
- (void)loadState:(int)slot;
- (void)startEmulator;
- (void)pauseEmulator;
- (void)resumeEmulator;
- (void)endEmulator;

- (const uint32_t *)pixels;

+ (void)copyBundledROMs;

+ (void)createDirectories;
+ (NSString *)romsDirectory;

@end

NS_ASSUME_NONNULL_END 
