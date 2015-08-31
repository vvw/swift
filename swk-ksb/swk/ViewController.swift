//
//  ViewController.swift
//  swk
//
//  Created by vvw on 7/31/15.
//  Copyright © 2015 vvw. All rights reserved.
//

//  inject js code to hide a DOM element
/*
var styleNode = document.createElement("style");
styleNode.textContent = '#user-details {display: none}';
document.documentElement.appendChild(styleNode);
*/

import Foundation
import UIKit
import WebKit

class ViewController: UIViewController,WKNavigationDelegate {

    var webview: WKWebView?
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    func loadjs(fnameWithoutExt fname: String) ->String? {
        let hideBioScriptURL = NSBundle.mainBundle().pathForResource(fname, ofType: "js")
        assert(hideBioScriptURL != nil, "### js not found: \(fname).js\n")
        let string:String?
        do {
            string = try String(contentsOfFile: hideBioScriptURL!, encoding:NSUTF8StringEncoding)
        } catch {
            string = nil
        }
        return string
    }
    
    func injectjsConfig (js:String) ->WKWebViewConfiguration {
        let configuration = WKWebViewConfiguration()
        let script = WKUserScript(source: js, injectionTime: .AtDocumentStart, forMainFrameOnly: true)
        configuration.userContentController.addUserScript(script)
        return configuration
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        let js = loadjs(fnameWithoutExt:"hideBio")
        assert(js != nil, "js load fail!")
        let config = injectjsConfig (js!);
        self.view.frame = CGRectMake(0,20,self.view.frame.size.width,self.view.frame.size.height-20);
        webview = WKWebView(frame: self.view.frame, configuration: config)
        
        let req = NSURLRequest( URL: NSURL( string: "http://new.ksbao.com/html/default.html")!)

        //let req = NSURLRequest( URL: NSURL( string: "http://www.baidu.com")!)
        //let req = NSURLRequest( URL: NSURL( string: "http://www.raywenderlich.com/u/funkyboy")!)
        webview?.loadRequest(req)
        //self.window.frame =  CGRectMake(0,20,self.window.frame.size.width,self.window.frame.size.height-20);
       
        self.view.addSubview(webview!)
    }
    
    // MARK: WKNavigationDelegate
    func webView(webView: WKWebView, didCommitNavigation navigation: WKNavigation!) {
        NSLog("commit")
    }
    
    func webView(webView: WKWebView, didFailNavigation navigation: WKNavigation!, withError error: NSError) {
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

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

