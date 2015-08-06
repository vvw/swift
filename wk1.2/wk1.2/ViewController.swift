//
//  ViewController.swift
//  wk1.2
//
//  Created by yt on 15/8/5.
//  Copyright (c) 2015年 yt. All rights reserved.
//

import UIKit
import WebKit


let MessageHandler = "didFetchAuthors"


class ViewController: UIViewController, WKNavigationDelegate, WKScriptMessageHandler {

    var authorsWebView: WKWebView?
    
    required init(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let configuration = WKWebViewConfiguration()
        //let hideBioScriptURL = NSBundle.mainBundle().pathForResource("fetchAuthors", ofType: "js")
        let hideBioScriptURL = NSBundle.mainBundle().pathForResource("sayhi", ofType: "js")
        let hideBioJS = String(contentsOfFile:hideBioScriptURL!, encoding:NSUTF8StringEncoding, error: nil)
        let hideBioScript = WKUserScript(source: hideBioJS!, injectionTime: .AtDocumentEnd, forMainFrameOnly: true)
        configuration.userContentController.addUserScript(hideBioScript)
        configuration.userContentController.addScriptMessageHandler(self, name: MessageHandler)

        self.authorsWebView = WKWebView(frame: self.view.frame, configuration: configuration)
        self.authorsWebView!.navigationDelegate = self
        //let URL = NSURL(string:"http://www.apple.com")
        let URL = NSURL(string:"http://www.raywenderlich.com/about");
        let request = NSURLRequest(URL:URL!)
        authorsWebView!.loadRequest(request)
        self.view.addSubview(authorsWebView!)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    func userContentController(userContentController: WKUserContentController, didReceiveScriptMessage message: WKScriptMessage) {
        if (message.name == MessageHandler) {
            let msg = message.body as! String
            NSLog("authors = \(msg)")
        }
    }
    
    // MARK: WKNavigationDelegate
    func webView(webView: WKWebView, didCommitNavigation navigation: WKNavigation!) {
        NSLog("commit")
    }
    
    func webView(webView: WKWebView, didFailNavigation navigation: WKNavigation!, withError error: NSError){
        NSLog("err: " + error.localizedDescription)
    }
    
    func webView(webView: WKWebView, didFailProvisionalNavigation navigation: WKNavigation!, withError error: NSError) {
        NSLog("err2: " + error.localizedDescription)
    }
    
    func webView(webView: WKWebView, didFinishNavigation navigation: WKNavigation!) {
        NSLog("finish")
    }
    
    func webView(webView: WKWebView, didStartProvisionalNavigation navigation: WKNavigation!) {
        NSLog("start")
    }


}

