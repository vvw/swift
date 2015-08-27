
#import "AppDelegate.h"
#import "ViewController.h"

@interface AppDelegate ()


@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    UIStoryboard *storyboard=[UIStoryboard storyboardWithName:@"Main" bundle:nil];
    ViewController *viewController=[storyboard instantiateViewControllerWithIdentifier:@"viewController"];
    UINavigationController *_navTest = [[UINavigationController alloc] initWithRootViewController:viewController];
    self.window.rootViewController = _navTest;
    
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 7) {
        
        self.window.frame =  CGRectMake(0,20,self.window.frame.size.width,self.window.frame.size.height-20);
        
        UIApplication *myApp = [UIApplication sharedApplication];
        
        [myApp setStatusBarStyle: UIStatusBarStyleLightContent];
        
    }else{
        NSLog(@"其他");
    }
    
    return YES;
    
}

@end
