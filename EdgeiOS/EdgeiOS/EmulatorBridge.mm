#import "EmulatorBridge.h"

#define SDL_MAIN_HANDLED 1
#include <SDL3//SDL.h>
#include <SDL3/SDL_main.h>

#include "system.h"
#include "state.h"
#include "state_controller.h"

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
    std::string romPath([[[[self class] romsDirectory] stringByAppendingPathComponent:romName] UTF8String]);
    std::string statesPath([[[[self class] statesDirectory] stringByAppendingPathComponent:romName] UTF8String]);
    NSLog(@"loadROM: ROM path = %s", romPath.c_str());
    NSLog(@"loadROM: States path = %s", statesPath.c_str());
    system_ = std::make_unique<System>(romPath, statesPath);
}

#pragma mark - Emulator Control

- (void)startEmulator {
    _isRunning = true;
}

- (void)advanceOneFrame {
    if (!_isRunning) {
        return;
    }
    system_->SetButtons(self.dpadUp, self.dpadDown, self.dpadLeft, self.dpadRight, self.buttonA, self.buttonB, self.buttonSelect, self.buttonStart);
    system_->AdvanceOneFrame();
}

- (void)pauseEmulator {
    NSLog(@"TODO: pauseEmulator");
}

- (void)resumeEmulator {
    NSLog(@"TODO: resumeEmulator");
}

- (void)endEmulator {
    NSLog(@"TODO: endEmulator");
    // Make sure we're in a paused state, with audio off.
    [self pauseEmulator];
    // TODO: Save state.
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

#pragma mark - Directories

+ (void)createDirectories {
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error;
    
    NSString *romsPath = [self romsDirectory];
    NSString *statesPath = [self statesDirectory];
    
    [fileManager createDirectoryAtPath:romsPath withIntermediateDirectories:YES attributes:nil error:&error];
    [fileManager createDirectoryAtPath:statesPath withIntermediateDirectories:YES attributes:nil error:&error];
    
    NSURL *romsURL = [NSURL fileURLWithPath:romsPath];
    [romsURL setResourceValue:@YES forKey:NSURLIsExcludedFromBackupKey error:&error];
}

+ (NSString *)romsDirectory {
  NSString *documentsPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
  return [documentsPath stringByAppendingPathComponent:@"ROMs"];
}

+ (NSString *)statesDirectory {
  NSString *documentsPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
  return [documentsPath stringByAppendingPathComponent:@"States"];
}

+ (void)copyBundledROMs {
  NSFileManager *fileManager = [NSFileManager defaultManager];
  NSString *bundlePath = [[NSBundle mainBundle] resourcePath];
  NSError *error;
  
  NSArray *bundleContents = [fileManager contentsOfDirectoryAtPath:bundlePath error:&error];
  NSArray *ROMs = [bundleContents filteredArrayUsingPredicate:
        [NSPredicate predicateWithFormat:@"self ENDSWITH[c] '.gb'"]];

  for (NSString *rom in ROMs) {
    NSString *sourcePath = [bundlePath stringByAppendingPathComponent:rom];
    NSString *destPath = [[self romsDirectory] stringByAppendingPathComponent:rom];
    
    if (![fileManager fileExistsAtPath:destPath]) {
      [fileManager copyItemAtPath:sourcePath toPath:destPath error:&error];
    }
  }
}

#pragma mark - State

- (void)saveState {
    system_->SaveState();
}

- (void)loadPreviouslySavedState {
    system_->LoadPreviouslySavedState();
}

- (void)rewindState {
    system_->RewindState();
}

- (NSArray<SaveStateWrapper *> *)getSaveStates {    
    std::vector<std::unique_ptr<::State>> cppStates = system_->GetSaveStates();

    
    NSMutableArray<SaveStateWrapper *> *result = [NSMutableArray array];
    for (const auto& cppState : cppStates) {
        SaveStateWrapper *state = [[SaveStateWrapper alloc] initWithCppState:cppState.get()];
        [result addObject:state];
    }
    
    return result;
}

- (void)loadState:(int)slot {
    system_->LoadStateSlot(slot);
}

@end 
