#import <Foundation/Foundation.h>
#import <objc/runtime.h>

/* Objective-C runtime-patcher for Qt cocoa-menu fix:
 * https://codereview.qt-project.org/#/c/107029/
 */

@interface MenuFixer : NSObject
@end

@implementation MenuFixer
- (BOOL)worksWhenModal
{
    return YES;
}
@end

void osx_init() {
    IMP replacedMethodImp = class_getMethodImplementation([MenuFixer class], @selector(worksWhenModal));
    Method replacedMethod = class_getInstanceMethod([MenuFixer class], @selector(worksWhenModal));
    const char* replacedMethodEnc = method_getTypeEncoding(replacedMethod);
    Class qMenuDelegate = objc_getClass("QCocoaMenuDelegate");
    class_replaceMethod(qMenuDelegate, @selector(worksWhenModal),
                        replacedMethodImp, replacedMethodEnc);
}
