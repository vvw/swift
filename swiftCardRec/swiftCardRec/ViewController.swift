//
//  ViewController.swift
//  swiftCardRec
//
//  Created by vv on 16/1/15.
//  Copyright © 2016年 vv. All rights reserved.
//

import UIKit
import CoreFoundation


class ViewController: UIViewController {
    
    func getDocumentsURL() -> NSURL {
        let documentsURL = NSFileManager.defaultManager().URLsForDirectory(.DocumentDirectory, inDomains: .UserDomainMask)[0]
        return documentsURL
    }
    
    func fileInDocumentsDirectory(filename: String) -> String {
        
        let fileURL = getDocumentsURL().URLByAppendingPathComponent(filename)
        return fileURL.path!
        
    }

    func otheMeth() {
        // 1
        let fileURL = NSBundle.mainBundle().URLForResource("IMG_1119", withExtension: "JPG")
        
        // 2
        let beginImage = CIImage(contentsOfURL: fileURL!)
        
        // 3
        let filter = CIFilter(name: "CISepiaTone")
        filter!.setValue(beginImage, forKey: kCIInputImageKey)
        filter!.setValue(0.5, forKey: kCIInputIntensityKey)
        
        // 4
        let newImage = UIImage(CIImage: filter!.outputImage!)
        let width = CGImageGetWidth(newImage.CGImage);
    
    }

    override func viewDidLoad() {
        super.viewDidLoad()

        let imgeName = "IMG_1119.png"
        let img = UIImage(named:imgeName)!;
        //let wi = CGImageGetWidth(ghostImage.CGImage);

        //let imgeName = "IMG_1119.JPG";
       
        //let img = UIImage(contentsOfFile: imagePath);
        //let img = UIImage(contentsOfFile: "/Users/vv/Desktop/swiftCardRec/swiftCardRec/\(imgeName)")!;
        let w = CGImageGetWidth(img.CGImage);
        let h = CGImageGetHeight(img.CGImage);
        
        let bitsPerPixel = CGImageGetBitsPerPixel(img.CGImage);  // 多少比特一个像素
        let bitsPerComponent = CGImageGetBitsPerComponent(img.CGImage);
        let alphaInfo = CGImageGetAlphaInfo(img.CGImage);  // 是否有透明通道

        assert(bitsPerPixel == 32 && bitsPerComponent == 8 &&
            alphaInfo == CGImageAlphaInfo.NoneSkipLast, "No Suported Image type: \(imgeName)");
        
        
        let pixel = UnsafeMutablePointer<CUnsignedChar>.alloc(w * h * sizeof(UInt32))
        
        let ghostContext = CGBitmapContextCreate(pixel , w, h, bitsPerComponent, (bitsPerPixel / 8) * w, CGImageGetColorSpace(img.CGImage), CGImageGetBitmapInfo(img.CGImage).rawValue)!
        
        CGContextDrawImage(ghostContext, CGRectMake(0, 0, img.size.width, img.size.height),img.CGImage)

        
        
        for var i=0; i<w * h * sizeof(UInt32);i++ {
            //pixel[i] = 0;  // 每个像素都设成全黑
        }
        
        
        // 4. Create a new UIImage
        let newCGImage = CGBitmapContextCreateImage(ghostContext)!;
        let processedImage = UIImage(CGImage:newCGImage)
        
        
        let imageView = UIImageView(frame: CGRectMake(0, 25, 250, 250)); // set as you wantvar image = UIImage(named: "myImage.png");
        imageView.image = processedImage//img;
        self.view.addSubview(imageView);
        
        /*
        // 后面转成1 字节每像素，单色。所以是 1 * width, 而不是原来的值
        let targetImage = CGBitmapContextCreate(nil , w, h, bitsPerComponent, 1 * w, CGImageGetColorSpace(img.CGImage), CGImageGetBitmapInfo(img.CGImage).rawValue)!
        
        
        let srcdata = CGDataProviderCopyData(CGImageGetDataProvider(img.CGImage));
        let dstData = CGBitmapContextGetData(targetImage);
        //UInt32 *sourceData = (UInt32*)[((__bridge_transfer NSData*) CGDataProviderCopyData(CGImageGetDataProvider(c))) bytes];

        
       
        
        
        
       // let targetImage = CGBitmapContextCreate(nil, w, h, 8, CGImageGetWidth(img?.CGImage),
         //   CGColorSpaceCreateDeviceGray(), CGImageAlphaInfo.NoneSkipLast);
        
        let imageView = UIImageView(frame: CGRectMake(0, 25, 250, 250)); // set as you wantvar image = UIImage(named: "myImage.png");
        imageView.image = img;
        self.view.addSubview(imageView);*/
        
        
        
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

