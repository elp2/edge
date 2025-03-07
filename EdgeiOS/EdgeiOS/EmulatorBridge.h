#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, GameboyButton) {
    GameboyButtonA,
    GameboyButtonB,
    GameboyButtonStart,
    GameboyButtonSelect,
    GameboyButtonUp,
    GameboyButtonLeft,
    GameboyButtonDown,
    GameboyButtonRight
};

@interface EmulatorBridge : NSObject

// Singleton access.
+ (EmulatorBridge *)sharedInstance;

// Emulator control
- (void)pauseEmulation;
- (void)resumeEmulation;

// Input handling.
- (void)didPressButton:(GameboyButton)button;
- (void)didReleaseButton:(GameboyButton)button;

// Display.
- (uint32_t *)pixelData;

@end

NS_ASSUME_NONNULL_END 
