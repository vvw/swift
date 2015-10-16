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
        let sim = test2(ss1, ss2)
        let results = NSString (format: "%.2f", sim)
        label1.stringValue = "相似度：\(results)  字串1长度：\(utf8strlen(ss1))  字串2长度：\(utf8strlen(ss2))"

        freecstr(ss1)
        freecstr(ss2)
        ss1 = nil
        ss2 = nil
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let text = text1.textStorage?.string;
        let s1 = text!;
        print (s1)
        
        label1.stringValue = "abcdefg"
        
       
        var time:NSString
        var starttime = CACurrentMediaTime()
        var cstr = string2cstr("我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。")
        var a = testjieba(cstr)
        var elaspedTime = CACurrentMediaTime() - starttime
        if elaspedTime < 1.0 {
            time = NSString (format: "%.1f ms", Double (elaspedTime * 1000))
        } else {
            time = NSString (format: "%.2f s", Double (elaspedTime))
        }
        
        print ("分词执行时间：\(time)\n")
        
        print ("\(String(UTF8String:a)!)")
        
        /*
        starttime = CACurrentMediaTime()
        test2(string2cstr("我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。"),
            string2cstr("我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。"))
        elaspedTime = CACurrentMediaTime() - starttime
        if elaspedTime < 1.0 {
            time = NSString (format: "%.1f ms", Double (elaspedTime * 1000))
        } else {
            time = NSString (format: "%.2f s", Double (elaspedTime))
        }
        
        print ("相似度执行时间：\(time)\n")*/
        
        freecstr(a)
        a = nil
        freecstr(cstr)
        cstr = nil

    }

    override var representedObject: AnyObject? {
        didSet {

        }
    }


}

