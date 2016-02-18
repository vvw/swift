//
//  cardreaderlib.m
//  cardreaderlib
//
//  Created by vv on 16/2/17.
//  Copyright © 2016年 yingedu. All rights reserved.
//

#import "cardreaderlib.h"
#import <UIKit/UIKit.h>
#import "stdlib.h"

int readEssBitmapBuffer(char * jsonBuffer,int bufferSize,int version,int bpp,int width,int height, unsigned char * bitsBuffer);


@implementation cardreaderlib

+(int) readBitmapBuffer:(char*) jsonBuffer
             bufferSize:(int)   siz
                version:(int)   v
           bitsperpixel:(int)   bpp
                  width:(int)   w
                 height:(int)   h
             bitsBuffer:(unsigned char *) bitsbuf
{
    //return siz;
    return readEssBitmapBuffer(jsonBuffer, siz, v, bpp, w, h, bitsbuf);
}

// 参数：filePath 图片路径
// 返回结果：
//    识别成功返回json 的字节数组，大小是40960 字节，调用者负责释放这部分内存
//    识别失败，返回 "-1"
+(char *)reconize:(NSString *) filePath {
    if([filePath isEqualToString:@""]){
        return "-1";
    }
    UIImage *uimg = [UIImage imageNamed:filePath];
    if(uimg.size.height<400 || uimg.size.width<400){ // 最小支持图片大小是 400*400
        return "-1";
    }
    int sw,sh,dw,dh;
    int bpc,bpp,stride;
    unsigned char * pdata=NULL;
    sw=uimg.size.width;
    sh=uimg.size.height;
    
    int minSize = 1280;
    double rr = (double)sw / sh;
    // 新的宽高
    dw = (int)(rr * minSize);
    dh = minSize;
    
    // 缩放
    UIGraphicsBeginImageContext(CGSizeMake(dw, dh));
    [uimg drawInRect:CGRectMake(0, 0, dw, dh)];
    UIImage *simg = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    CGImageRef cimg=simg.CGImage;
    CFDataRef cdata = CGDataProviderCopyData(CGImageGetDataProvider(cimg));
    bpc=CGImageGetBitsPerComponent(cimg);
    if(bpc!=8){
        return "-1";
    }
    bpp=CGImageGetBitsPerPixel(cimg);
    if(bpp==8 || bpp==24 || bpp==32){
        dw=CGImageGetWidth(cimg);
        dh=CGImageGetHeight(cimg);
        stride=CGImageGetBytesPerRow(cimg);
    }
    else{
        return "-1";
    }
    pdata=(unsigned char *)CFDataGetBytePtr(cdata); // 图片的像素数据
    int bufsiz = 40960;
    char *cptr = (char*)malloc(bufsiz);
    memset(cptr, 0, bufsiz);
    //char  ab[40960];
    //char * cptr=&ab[0];
    //int jsize=1;//readEssBitmapBuffer(cptr,40960,0,bpp,dw,dh,pdata);//
    int jsize = [cardreaderlib readBitmapBuffer:cptr
                                     bufferSize:bufsiz version:0 bitsperpixel:bpp width:dw height:dh bitsBuffer:pdata
                 ];
    
    
    
    int si = strlen(cptr) + 1;  // json 长度很小也表示识别失败
    if(jsize==0 || si < 1000){
        //NSLog(@"识别失败！");
        return "-1";
    }
    
    // 识别成功，返回json 数据的长度
    //NSLog(@"%s", cptr);
    return cptr;
}

@end
