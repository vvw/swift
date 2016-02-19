//
//  ViewController.swift
//  QRReaderDemo
//
//  Created by Simon Ng on 23/11/14.
//  Copyright (c) 2014 AppCoda. All rights reserved.
//

import Dispatch
import UIKit
import AVFoundation

//class ViewController: UIViewController, AVCaptureMetadataOutputObjectsDelegate {
class ViewController: UIViewController, AVCaptureVideoDataOutputSampleBufferDelegate {
    
    @IBOutlet weak var messageLabel:UILabel!
    
    var captureSession:AVCaptureSession?
    var videoPreviewLayer:AVCaptureVideoPreviewLayer?
    var qrCodeFrameView:UIView?
    
    var takePic = false
    var changetakePic = false
    var dida = false
    var ready = true  // 只有ready 等于true 识别线程才开始识别
    
    var gameTimer: NSTimer!
    var lock:NSLock = NSLock()
    //let queue = TaskQueue()

    
    var arrCMSampleBuffer = [CMSampleBuffer]()
    
    func runTimedCode() {
        dida = true;
    }
    
    func stopTimer(t:NSTimer) {
        t.invalidate()
    }
    
    func starTimer(interval:Double, ctrl:ViewController) -> NSTimer{
        return NSTimer.scheduledTimerWithTimeInterval(interval, target: ctrl, selector: "runTimedCode", userInfo: nil, repeats: true)
        
    }
    
    func listenVolumeButton() {
        let audioSession = AVAudioSession.sharedInstance()
        do {
            try audioSession.setActive(true);
            audioSession.addObserver(self, forKeyPath: "outputVolume",
                options: NSKeyValueObservingOptions.New, context: nil)
        } catch {
            
        }
    }
    
    func capture() ->Void {
        // Get an instance of the AVCaptureDevice class to initialize a device object and provide the video
        // as the media type parameter.
        let captureDevice = AVCaptureDevice.defaultDeviceWithMediaType(AVMediaTypeVideo)
        
        do {
            // Get an instance of the AVCaptureDeviceInput class using the previous device object.
            let input = try AVCaptureDeviceInput(device: captureDevice)
            
            // Initialize the captureSession object.
            captureSession = AVCaptureSession()
            // Set the input device on the capture session.
            captureSession?.addInput(input)
            
            // Initialize a AVCaptureMetadataOutput object and set it as the output device to the capture session.
            //let captureMetadataOutput = AVCaptureMetadataOutput()
            let videoOutput = AVCaptureVideoDataOutput()
            //captureSession?.addOutput(captureMetadataOutput)
            captureSession?.addOutput(videoOutput)
            
            // Set delegate and use the default dispatch queue to execute the call back
            // captureMetadataOutput.setMetadataObjectsDelegate(self, queue: dispatch_get_main_queue())
            //videoOutput.setSampleBufferDelegate(self, queue: dispatch_get_main_queue())
            videoOutput.setSampleBufferDelegate(self, queue: dispatch_queue_create("sample buffer delegate", DISPATCH_QUEUE_SERIAL))
            
            
            // Detect all the supported bar code
            //captureMetadataOutput.metadataObjectTypes = supportedBarCodes
            
            //let availableMetadataObjectTypes =  captureMetadataOutput.availableMetadataObjectTypes as! [String]
            
            
            // captureMetadataOutput.metadataObjectTypes = availableMetadataObjectTypes
            
            
            // Initialize the video preview layer and add it as a sublayer to the viewPreview view's layer.
            videoPreviewLayer = AVCaptureVideoPreviewLayer(session: captureSession)
            videoPreviewLayer?.videoGravity = AVLayerVideoGravityResizeAspectFill
            videoPreviewLayer?.frame = view.layer.bounds
            view.layer.addSublayer(videoPreviewLayer!)
            
            // Start video capture
            captureSession?.startRunning()
            
            gameTimer = starTimer(2, ctrl:self) // NSTimer.scheduledTimerWithTimeInterval(2, target: self, selector: "runTimedCode", userInfo: nil, repeats: true)
            
        } catch {
            // If any error occurs, simply print it out and don't continue any more.
            print(error)
            return
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        capture()
        
        // Move the message label to the top view
        view.bringSubviewToFront(messageLabel)
        
        // Initialize QR Code Frame to highlight the QR code
        qrCodeFrameView = UIView()
        
        if let qrCodeFrameView = qrCodeFrameView {
            qrCodeFrameView.layer.borderColor = UIColor.greenColor().CGColor
            qrCodeFrameView.layer.borderWidth = 2
            view.addSubview(qrCodeFrameView)
            view.bringSubviewToFront(qrCodeFrameView)
        }
        
        listenVolumeButton()
        
        let queue = TaskQueue()
        
        //queue.tasks +=~ {
          //  print ("...")
        //}
        
        /*
        dispatch_async(serialQueue, {
            
            println("This is second Method")
            
            for index in 1...5 {
                
                sleep(2)
                println("\(index) times 5 is \(index * 5)")
            }
            
        })*/
    }
    
    func readCard(sampleBuffer:CMSampleBuffer) {
        let desript = CMSampleBufferGetFormatDescription(sampleBuffer)
        let pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer)!
        let cameraImage = CIImage(CVPixelBuffer: pixelBuffer)
        
        let context1 = CIContext(options: nil)
        let cg:CGImage = context1.createCGImage(cameraImage, fromRect: cameraImage.extent)
        let img = UIImage(CGImage: cg)
        
        var w = CGImageGetWidth(img.CGImage)
        var h = CGImageGetHeight(img.CGImage)
        if (w < 400 || h < 400) {
            assert(false, "image too small.")
        }
        
        // iphone6 1000*750
        
        //var minSize = 1280  // 高度固定，重新计算宽度  // 5s
        var minSize = 1080  // 5s 1920 * 1080
        //let minSize = 750  // 高度固定，重新计算宽度    // 6
        if (h == 750) {
            minSize = 750;
        }
        
        let rr:CDouble = CDouble( w ) / CDouble(h);
        let w2:CInt = CInt(rr * CDouble(minSize))
        
        UIGraphicsBeginImageContext(CGSizeMake(CGFloat(w2), CGFloat(minSize)));  // 缩放，w2宽，minSize 高
        img.drawInRect(CGRectMake(0, 0, CGFloat(w2), CGFloat(minSize)))
        let simg = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        
        let cimg=simg.CGImage;
        let cdata = CGDataProviderCopyData(CGImageGetDataProvider(cimg));
        let bpc=CGImageGetBitsPerComponent(cimg);
        
        let bpp=CGImageGetBitsPerPixel(cimg);
        let dw:Int,dh:Int,stride:Int
        if(bpp==8 || bpp==24 || bpp==32){
            
        } else {
            assert(false, "bpp not is 8, 24, 32")
        }
        dw=CGImageGetWidth(cimg);
        dh=CGImageGetHeight(cimg);
        stride=CGImageGetBytesPerRow(cimg);
        let pdata = UnsafeMutablePointer<UInt8>(CFDataGetBytePtr(cdata))
        
        let sz:Int32 = 40960;
        let buffer = UnsafeMutablePointer<Int8>.alloc(Int(sz));
        let jsize = readEssBitmapBuffer(buffer,40960,0,Int32(bpp),Int32(dw),Int32(dh),pdata);
        
        //print (String.fromCString(buffer))
        
        self.messageLabel.text = "json length is: \(jsize)"
        
        if (jsize > 10000) {
            ready = false  // 只别成功，不要再识别了
            print ("success!!!!")
            print (String.fromCString(buffer)!)
           
        } else {  // 识别失败，继续下一次识别
            print ("rec fail!")
            
            func callback (ac:UIAlertAction!) ->Void {
                //print("pressed ... ")
                //self.capture()
                self.gameTimer = self.starTimer(2, ctrl: self) // 重启计时器
                //ready = true
            }
            //self.stopTimer(self.gameTimer)  // 停止计时器
            //SCMessageBox.show(self, title: "识别结果", contentMsg: String.fromCString(buffer)!, buttonString: "再来一次！", blockHandler: callback)
            
        }
        
        


        free(buffer)    /**/

    }
    
    
    func captureOutput(captureOutput: AVCaptureOutput!, didOutputSampleBuffer sampleBuffer: CMSampleBuffer!, fromConnection connection: AVCaptureConnection!)
    {
        if (self.dida == true && ready == true) {
            let count = arrCMSampleBuffer.count
            if (count <= 10) {
                arrCMSampleBuffer.append(sampleBuffer)
                //ready = true
                let queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0) // 加入GCD线程任务队列，识别题卡
                dispatch_async(queue) {
                    //let someData = getData(username, password)
                    let samplebuffer:CMSampleBuffer? = self.arrCMSampleBuffer.popLast()
                    if (samplebuffer == nil) {
                    
                    } else {
                        //print ("xxxxxxxxx \(self.arrCMSampleBuffer.count)")
                        self.readCard(samplebuffer!)
                    }
                    /* do something with someData */
                }
            }
        }
        
        if (self.takePic == true) {
            // 停止捕获
            // self.captureSession?.stopRunning()
            //self.videoPreviewLayer?.removeFromSuperlayer()
            //self.captureSession = nil
            //self.videoPreviewLayer = nil
        }/**/
        
        let desript = CMSampleBufferGetFormatDescription(sampleBuffer)
        let pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer)!
        let cameraImage = CIImage(CVPixelBuffer: pixelBuffer)
        
        dispatch_async(dispatch_get_main_queue())
            {
                
                if (self.dida == true) { // 存在新的定时器时钟滴答信号
                    self.dida = false
                    
                    /*
                    
                    let context1 = CIContext(options: nil)
                    let cg:CGImage = context1.createCGImage(cameraImage, fromRect: cameraImage.extent)
                    let img = UIImage(CGImage: cg)
                    
                    var w = CGImageGetWidth(img.CGImage)
                    var h = CGImageGetHeight(img.CGImage)
                    if (w < 400 || h < 400) {
                        assert(false, "image too small.")
                    }
                    
                    // iphone6 1000*750
                    
                    //var minSize = 1280  // 高度固定，重新计算宽度  // 5s
                    var minSize = 1080  // 5s 1920 * 1080
                    //let minSize = 750  // 高度固定，重新计算宽度    // 6
                    if (h == 750) {
                        minSize = 750;
                    }
                    
                    let rr:CDouble = CDouble( w ) / CDouble(h);
                    let w2:CInt = CInt(rr * CDouble(minSize))
                    
                    UIGraphicsBeginImageContext(CGSizeMake(CGFloat(w2), CGFloat(minSize)));  // 缩放，w2宽，minSize 高
                    img.drawInRect(CGRectMake(0, 0, CGFloat(w2), CGFloat(minSize)))
                    let simg = UIGraphicsGetImageFromCurrentImageContext();
                    UIGraphicsEndImageContext();
                    
                    let cimg=simg.CGImage;
                    let cdata = CGDataProviderCopyData(CGImageGetDataProvider(cimg));
                    let bpc=CGImageGetBitsPerComponent(cimg);
                    
                    let bpp=CGImageGetBitsPerPixel(cimg);
                    let dw:Int,dh:Int,stride:Int
                    if(bpp==8 || bpp==24 || bpp==32){
                        
                    } else {
                        assert(false, "bpp not is 8, 24, 32")
                    }
                    dw=CGImageGetWidth(cimg);
                    dh=CGImageGetHeight(cimg);
                    stride=CGImageGetBytesPerRow(cimg);
                    let pdata = UnsafeMutablePointer<UInt8>(CFDataGetBytePtr(cdata))
                    
                    let sz:Int32 = 40960;
                    let buffer = UnsafeMutablePointer<Int8>.alloc(Int(sz));
                    let jsize = readEssBitmapBuffer(buffer,40960,0,Int32(bpp),Int32(dw),Int32(dh),pdata);
                    
                    //print (String.fromCString(buffer))
                    
                    self.messageLabel.text = "json length is: \(jsize)"
                    
                    
                    func callback (ac:UIAlertAction!) ->Void {
                        print("pressed ... ")
                        //self.capture()
                        self.gameTimer = self.starTimer(2, ctrl: self) // 重启计时器
                    }
                    self.stopTimer(self.gameTimer)  // 停止计时器
                    SCMessageBox.show(self, title: "识别结果", contentMsg: String.fromCString(buffer)!, buttonString: "再来一次！", blockHandler: callback)
                    
                    free(buffer) */   /**/
                    
                }
        }
    }
    
    override func observeValueForKeyPath(keyPath: String?, ofObject object: AnyObject?, change: [String : AnyObject]?, context: UnsafeMutablePointer<Void>) {
        
        if (keyPath == "outputVolume") {
            print (keyPath);
            if (takePic == false) {
                takePic = true
                changetakePic = true
                print (takePic)
            } else {
                print (takePic)
            }
        }
        
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}

