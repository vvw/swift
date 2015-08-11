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
    reques=[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://new.xxx.com?clientType=iphone"]];
    [webview loadRequest:reques];
    self.view = webview;
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType{
    NSLog(@"### link：%@",request.mainDocumentURL.description);
    
    NSString *absolutStr=request.URL.absoluteString;
    NSLog(@"### request.URL.absoluteString: %@", absolutStr);
    return true;
}


-(void)viewWillAppear:(BOOL)animated
{
    webview.delegate = self;
}

- (void)webViewDidStartLoad:(UIWebView *)webView{
    NSLog(@"## webViewDidStartLoad");
}
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    NSLog(@"%@",[error localizedDescription]);
}
-(void)webViewDidFinishLoad:(UIWebView *)webView{
    NSLog(@"## webViewDidFinishLoad");
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
