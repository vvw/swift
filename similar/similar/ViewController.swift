//
//  ViewController.swift
//  similar
//
//  Created by vvw on 10/12/15.
//  Copyright (c) 2015 vvw. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {
    @IBOutlet var text1: NSTextView!
    @IBOutlet var text2: NSTextView!
    @IBOutlet weak var button1: NSButton!
    @IBOutlet weak var label1: NSTextField!
    
    @IBAction func btnact(sender: AnyObject) {
        onClick (sender);
    }
    func onClick(obj:AnyObject?) {
        print ("has been click\n")
        var txt1 = text1.textStorage?.string
        var txt2 = text2.textStorage?.string
        var s1 = txt1!
        var s2 = txt2!
        print ("txt1 is :\(s1) txt2 is : \(s2)\n")
        
        var cstr1 = (s1 as NSString).UTF8String
        var cstr2 = (s2 as NSString).UTF8String
        
        print ( String(UTF8String:cstr1)! + "hi,,,\n")
        
        var len1 = Int ( strlen (cstr1) )
        var len2 = Int ( strlen (cstr2) )

        
        var charbuf1 = UnsafeMutablePointer<CChar>.alloc(len1+1)
        charbuf1.initialize(0)
        var charbuf2 = UnsafeMutablePointer<CChar>.alloc(len2+1)
        charbuf2.initialize(0)
        
        memcpy(charbuf1, cstr1,  Int(strlen(cstr1)) )
        print ( String(UTF8String:charbuf1)! + "hello1,,,\n")
        memcpy(charbuf2, cstr2,  Int(strlen(cstr2)) )
        print ( String(UTF8String:charbuf2)! + "hello2,,,\n")
        
        
        var r2 = test2(charbuf1, charbuf2);
        label1.stringValue = "相似度：\(r2)  字串1长度：\(utf8strlen(charbuf1))  字串2长度：\(utf8strlen(charbuf2))"
        
        charbuf1.destroy()
        charbuf1.dealloc(len1)
        charbuf1 = nil
        
        charbuf2.destroy()
        charbuf2.dealloc(len2)
        charbuf2 = nil
        
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        var text = text1.textStorage?.string;
        var s1 = text!;
        print (s1)
        
        label1.stringValue = "abcdefg"
        
        
        
        //label1.setValue("");
    }

    override var representedObject: AnyObject? {
        didSet {

        }
    }


}

