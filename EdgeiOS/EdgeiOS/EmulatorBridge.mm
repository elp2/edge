#import "EmulatorBridge.h"

#define SDL_MAIN_HANDLED 1
#include <SDL3//SDL.h>
#include <SDL3/SDL_main.h>

#include "system.h"

@interface EmulatorBridge () {
    std::unique_ptr<System> system_;
}

@property (nonatomic, assign) BOOL isRunning;
@property (nonatomic, assign) BOOL sdlInitialized;
@property (nonatomic, strong) AVAudioSession *audioSession;

@end

@implementation EmulatorBridge

#pragma mark - Singleton

+ (EmulatorBridge *)sharedInstance {
    static EmulatorBridge *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[EmulatorBridge alloc] init];
    });
    return sharedInstance;
}

#pragma mark - Initialization

- (instancetype)init {
    if (self = [super init]) {

    }
    return self;
}

- (void)initializeSDL {
    if (_sdlInitialized) {
        return;
    }

    NSError *error = nil;
    self.audioSession = [AVAudioSession sharedInstance];
    if (![self.audioSession setCategory:AVAudioSessionCategoryPlayback
                                error:&error]) {
        NSLog(@"Failed to set audio session category: %@", error);
    }
    
    if (![self.audioSession setActive:YES error:&error]) {
        NSLog(@"Failed to activate audio session: %@", error);
    }

    SDL_SetMainReady();

    _sdlInitialized = YES;
}

- (void)loadROM:(NSString *)romName {
    NSAssert(_sdlInitialized, @"SDL Must be initialized.");
    NSBundle* bundle = [NSBundle mainBundle];
    NSArray<NSString*>* files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[bundle bundlePath] error:nil];

    NSString* romPath = [[bundle bundlePath] stringByAppendingPathComponent:romName];
    const char* cPath = [romPath UTF8String];
    if (cPath) {
        std::string cppPath(cPath);
        system_ = std::make_unique<System>(cppPath);
        _isRunning = true;
    } else {
    }
}

#pragma mark - Emulator Control

- (void)advanceOneFrame {
    if (!_isRunning) {
        return;
    }
    system_->AdvanceOneFrame();
}

#pragma mark - Input Handling

- (void)didPressButton:(GameboyButton)button {
    // TODO.
}

- (void)didReleaseButton:(GameboyButton)button {
    // TODO.
}

#pragma mark - Display

- (const uint32_t *)pixels {
    NSAssert(_sdlInitialized, @"SDL Must be initialized");
    return system_->pixels();
}

#pragma mark - Cleanup

- (void)dealloc {
    NSError *error = nil;
    if (![self.audioSession setActive:NO error:&error]) {
        NSLog(@"Failed to deactivate audio session: %@", error);
    }
}

@end 
