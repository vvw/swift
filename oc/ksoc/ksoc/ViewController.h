//
//  ViewController.h
//  ksoc
//
//  Created by vvw on 8/11/15.
//  Copyright (c) 2015 vvw. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController
@end

@interface InPurchaseCtrler : UIViewController
    
@end

@interface InPurchaseCtrler ()<UIWebViewDelegate>
@property (strong, nonatomic) UIWebView *webview;
@end

@implementation InPurchaseCtrler
@synthesize webview;
- (void)viewDidLoad {
    [super viewDidLoad];
    webview = [[UIWebView alloc]init];
    UIButton *btn = [[UIButton alloc] initWithFrame:CGRectMake(30, 52, 260, 54)];
    //日均
    UILabel *lab_averagePriceSingle=[[UILabel alloc] initWithFrame:CGRectMake(125,15, 50, 24)];
    [lab_averagePriceSingle setText:[NSString stringWithFormat:@"¥%0.2f",0.56]];
    [lab_averagePriceSingle setTextColor:[UIColor grayColor]];
    [lab_averagePriceSingle setBackgroundColor:[UIColor clearColor]];
    [lab_averagePriceSingle setFont:[UIFont boldSystemFontOfSize:18]];

    [btn setTag:1];
    //[btnContainer addSubview:btnImage];
    [btn addSubview:lab_averagePriceSingle];
    [webview addSubview:btn];
    webview.scalesPageToFit = YES;
    NSURLRequest *reques=[[NSURLRequest alloc] init];
    reques=[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://www.apple.com"]];
    [webview loadRequest:reques];
    self.view = webview;
}
-(void)viewWillAppear:(BOOL)animated
{
    webview.delegate = self;
}
- (void)webViewDidStartLoad:(UIWebView *)webView{
    NSLog(@"## InPurchaseCtrler webViewDidStartLoad");
}
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    NSLog(@"## InPurchaseCtrler %@",[error localizedDescription]);
}
-(void)webViewDidFinishLoad:(UIWebView *)webView{
    NSLog(@"## InPurchaseCtrler webViewDidFinishLoad");
}
@end