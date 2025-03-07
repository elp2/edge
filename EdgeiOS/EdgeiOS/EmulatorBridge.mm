#import "EmulatorBridge.h"

#include "system.h"

@interface EmulatorBridge () {
    std::unique_ptr<System> system_;
}

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
        NSError *error = nil;
        
        // Set up audio session
        self.audioSession = [AVAudioSession sharedInstance];
        if (![self.audioSession setCategory:AVAudioSessionCategoryPlayback
                                    error:&error]) {
            NSLog(@"Failed to set audio session category: %@", error);
        }
        
        if (![self.audioSession setActive:YES error:&error]) {
            NSLog(@"Failed to activate audio session: %@", error);
        }
        
        system_ = std::make_unique<System>("roms/pocket.gb");
    }
    return self;
}

#pragma mark - Emulator Control

- (void)pauseEmulation {
    // TODO.
}

- (void)resumeEmulation {
    // TODO.
}

#pragma mark - Input Handling

- (void)didPressButton:(GameboyButton)button {
    // TODO.
}

- (void)didReleaseButton:(GameboyButton)button {
    // TODO.
}

#pragma mark - Display

- (uint32_t *)pixelData {
    return NULL;
}

#pragma mark - Cleanup

- (void)dealloc {
    NSError *error = nil;
    if (![self.audioSession setActive:NO error:&error]) {
        NSLog(@"Failed to deactivate audio session: %@", error);
    }
    
    // C++ cleanup happens automatically through RAII
}

@end 
