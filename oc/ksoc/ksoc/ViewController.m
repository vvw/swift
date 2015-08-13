//
//  ViewController.m
//  ksoc
//
//  Created by vvw on 8/11/15.
//  Copyright (c) 2015 vvw. All rights reserved.
//

#import "ViewController.h"
#import "MBProgressHUD.h"

@interface ViewController ()<UIWebViewDelegate>
@property (strong, nonatomic) UIWebView *webview;
@property (strong, nonatomic) MBProgressHUD *hudView;

@end

@implementation ViewController
@synthesize webview;
- (void)viewDidLoad {
    [super viewDidLoad];
    webview = [[UIWebView alloc]init];
    webview.scalesPageToFit = YES;
    NSURLRequest *reques=[[NSURLRequest alloc] init];
    reques=[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://new.ksbao.com?clientType=iphone"]];
    [webview loadRequest:reques];
    self.view = webview;
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType{
    NSLog(@"### link：%@",request.mainDocumentURL.description);
    
    NSString *absolutStr=request.URL.absoluteString;
    NSLog(@"### request.URL.absoluteString: %@", absolutStr);
    
    
    NSString *prefixStr=@"ios://";
    
    if ([absolutStr hasPrefix:prefixStr]) {
        NSString *urlkey=[absolutStr substringFromIndex:6];
        NSArray *pareArray=[urlkey componentsSeparatedByString:@"/"];
        NSString *action=[pareArray objectAtIndex:0];
        if ([action isEqualToString:@"iOSiap"]) {
            
            NSString *js_iOS=[self.webview stringByEvaluatingJavaScriptFromString:@"iOSfn();"];
            NSArray *arry=[js_iOS  componentsSeparatedByString:@"/"];
            NSString *UserID=[arry objectAtIndex:0];
            NSString *AppEName=[arry objectAtIndex:1];
            NSString *appName=[arry objectAtIndex:2];
            
            NSUserDefaults *userDefaults=[NSUserDefaults standardUserDefaults];
            [userDefaults setObject:UserID forKey:@"UserID"];
            [userDefaults setObject:AppEName forKey:@"AppEName"];
            [userDefaults setObject:appName forKey:@"AppName"];
            [userDefaults synchronize];
            
            NSString *usid =[[NSUserDefaults standardUserDefaults] stringForKey:@"UserID"];
            //[userDefaults]
            
            NSLog(@"AppEName:%@ appName:%@ UserID:%@",AppEName, appName, UserID);

            InPurchaseCtrler *ctrler = [[InPurchaseCtrler alloc]init];
            [self presentViewController:ctrler animated:YES completion:^{
                NSLog(@"######## Switched to InPurchaseCtrler");
            }];
        }
    }

    
    return true;
}


-(void)viewWillAppear:(BOOL)animated
{
    webview.delegate = self;
}

- (void)webViewDidStartLoad:(UIWebView *)webView{
    NSLog(@"## webViewDidStartLoad");
    if (self.hudView) {
        assert(false);
    }
    self.hudView = [[MBProgressHUD alloc] initWithView:self.view];
    [self.view addSubview:self.hudView];
    [self.view bringSubviewToFront:self.hudView];
    self.hudView.labelText =NSLocalizedString(@"loading ...", nil);
    [self.hudView show:YES];
}
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    NSLog(@"%@",[error localizedDescription]);
}
-(void)webViewDidFinishLoad:(UIWebView *)webView{
    NSLog(@"## webViewDidFinishLoad");
    if (self.hudView) {
        [self.hudView removeFromSuperview];
        self.hudView=nil;
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
