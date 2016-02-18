
#import "ViewController.h"
#import "cardreaderlib.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    NSString *filePath=[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"00.jpg"];
    if([filePath isEqualToString:@""]){
        return;
    }
    
    char *buf = [cardreaderlib reconize:filePath];

    
    if (strcmp(buf, "-1") == 0) {
        NSLog(@"识别失败！");
    } else {
        NSLog(@"%s", buf);
        free(buf);  // 用完记得释放内存
    }

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

@end
