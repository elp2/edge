#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SaveStateWrapper : NSObject

// Initialize by wrapping an existing C++ State pointer
- (instancetype)initWithCppState:(void *)cppState;

// Get the slot number for this state
- (int)getSlot;

// Get the slot number as an Int for SwiftUI compatibility
@property (nonatomic, readonly) NSInteger slot;

// Get the state directory path
- (NSString *)getStateDir;

// Get the screenshot path
- (NSString *)getScreenshotPath;

@end

NS_ASSUME_NONNULL_END 
