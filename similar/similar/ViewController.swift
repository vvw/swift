//
//  ViewController.swift
//  similar
//
//  Created by vvw on 10/12/15.
//  Copyright (c) 2015 vvw. All rights reserved.

// SmartString.swift
// https://gist.github.com/erica/be6c8d7435aad40fda7d

import Cocoa

class ViewController: NSViewController {
    @IBOutlet var text1: NSTextView!
    @IBOutlet var text2: NSTextView!
    @IBOutlet weak var button1: NSButton!
    @IBOutlet weak var label1: NSTextField!
    
    @IBAction func btnact(sender: AnyObject) {
        onClick (sender);
    }
    
    // 调用者负责释放内存
    func string2cstr (s:String) ->UnsafeMutablePointer<CChar> {
        let str = (s as NSString).UTF8String
        let len = Int ( strlen(str) )
        let buf = UnsafeMutablePointer<CChar>.alloc(len+1)
        buf.initialize(0)
        memcpy(buf, str,  Int(strlen(str)) )
        return buf
    }
    
    func freecstr(s:UnsafeMutablePointer<CChar>) {
        s.destroy()
        s.dealloc(Int (strlen(s)+1 ))
    }
    
    
    func onClick(obj:AnyObject?) {
        print ("has been click\n")
        
        var ss1 = string2cstr ((text1.textStorage?.string)!)
        var ss2 = string2cstr ((text2.textStorage?.string)!)
        let sim = test2(ss1, ss2);
        let results = NSString (format: "%.2f", sim)
        label1.stringValue = "相似度：\(results)  字串1长度：\(utf8strlen(ss1))  字串2长度：\(utf8strlen(ss2))"
        

        freecstr(ss1)
        freecstr(ss2)
        ss1 = nil
        ss2 = nil
        
        /*
        let s1 = txt1!
        let s2 = txt2!
        print ("txt1 is :\(s1) txt2 is : \(s2)\n")
        
        let cstr1 = (s1 as NSString).UTF8String
        let cstr2 = (s2 as NSString).UTF8String
        
        print ( String(UTF8String:cstr1)! + "hi,,,\n")
        
        let len1 = Int ( strlen (cstr1) )
        let len2 = Int ( strlen (cstr2) )

        
        let charbuf1 = UnsafeMutablePointer<CChar>.alloc(len1+1)
        charbuf1.initialize(0)
        let charbuf2 = UnsafeMutablePointer<CChar>.alloc(len2+1)
        charbuf2.initialize(0)
        
        memcpy(charbuf1, cstr1,  Int(strlen(cstr1)) )
        print ( String(UTF8String:charbuf1)! + "hello1,,,\n")
        memcpy(charbuf2, cstr2,  Int(strlen(cstr2)) )
        print ( String(UTF8String:charbuf2)! + "hello2,,,\n")
        

        let r2 = test2(charbuf1, charbuf2);
        label1.stringValue = "相似度：\(r2)  字串1长度：\(utf8strlen(charbuf1))  字串2长度：\(utf8strlen(charbuf2))"
*/
         /*
        charbuf1.destroy()
        charbuf1.dealloc(len1)
        charbuf1 = nil
        
        charbuf2.destroy()
        charbuf2.dealloc(len2)
        charbuf2 = nil*/
        
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let text = text1.textStorage?.string;
        let s1 = text!;
        print (s1)
        
        label1.stringValue = "abcdefg"
        
       
        let time:NSString
        let starttime = CACurrentMediaTime()
        var a = testjieba()
        let elaspedTime = CACurrentMediaTime() - starttime
        if elaspedTime < 1.0 {
            time = NSString (format: "%.1f ms", Double (elaspedTime * 1000))
        } else {
            time = NSString (format: "%.2f s", Double (elaspedTime))
        }
        
        print ("分词执行时间：\(time)\n")
        
        print ("\(String(UTF8String:a)!)")
        freecstr(a)
        a = nil
        /*
        let l = strlen (a)
        print ("\(String(UTF8String:a)!)")
        a.destroy()
        a.dealloc(Int(l))
        a = nil */
    }

    override var representedObject: AnyObject? {
        didSet {

        }
    }


}

