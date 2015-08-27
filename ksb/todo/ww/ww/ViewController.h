//
//  ViewController.h
//  ww
//
//  Created by vvw on 8/26/15.
//  Copyright (c) 2015 vvw. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController<UIWebViewDelegate,UIScrollViewDelegate>
@property (weak, nonatomic) IBOutlet UIWebView *webView;

@end

