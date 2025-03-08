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

- (void)initializeSDL;
- (void)loadROM:(NSString *)romName;
- (void)advanceOneInstruction;

// Emulator control
- (void)pauseEmulation;
- (void)resumeEmulation;

// Input handling.
- (void)didPressButton:(GameboyButton)button;
- (void)didReleaseButton:(GameboyButton)button;

// Display.
- (const uint32_t *)pixels;

@end

NS_ASSUME_NONNULL_END 
