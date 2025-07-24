#import "SaveStateWrapper.h"
#import <UIKit/UIKit.h>
#include "state.h"
#include <memory>

@interface SaveStateWrapper () {
    std::unique_ptr<::State> cppState_;
}

@end

@implementation SaveStateWrapper

- (instancetype)initWithCppState:(void *)cppState {
    if (self = [super init]) {
        ::State* statePtr = static_cast<::State*>(cppState);
        cppState_ = std::make_unique<::State>(*statePtr);
    }
    return self;
}

- (int)getSlot {
    return cppState_->GetSlot();
}

- (NSInteger)slot {
    return (NSInteger)cppState_->GetSlot();
}

- (NSString *)getStateDir {
    std::string stateDir = cppState_->GetStateDir();
    return [NSString stringWithUTF8String:stateDir.c_str()];
}

- (NSString *)getScreenshotPath {
    std::string screenshotPath = cppState_->GetScreenshotPath();
    return [NSString stringWithUTF8String:screenshotPath.c_str()];
}

- (UIImage *)getScreenshotImage {
    NSString *screenshotPath = [self getScreenshotPath];
    UIImage *image = [UIImage imageWithContentsOfFile:screenshotPath];
    return image;
}

- (NSDate *)getSaveDate {
    time_t saveTime = cppState_->GetSaveTime();
    if (saveTime > 0) {
        return [NSDate dateWithTimeIntervalSince1970:saveTime];
    }
    return nil;
}

@end 