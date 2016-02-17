#import <UIKit/UIKit.h>
#import "MyfirstLib.h"
@interface ViewController : UIViewController
@end

@implementation ViewController
- (void)viewDidLoad {
    [super viewDidLoad];
    MyfirstLib *myFirstLib = [[MyfirstLib alloc] init];
    [myFirstLib logMyfirstLib];
    
}
@end