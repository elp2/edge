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

// Display.
- (const uint32_t *)pixels;

@end

NS_ASSUME_NONNULL_END 
