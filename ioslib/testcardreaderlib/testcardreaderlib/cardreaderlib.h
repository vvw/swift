//
//  cardreaderlib.h
//  cardreaderlib
//
//  Created by vv on 16/2/17.
//  Copyright © 2016年 yingedu. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface cardreaderlib : NSObject

/*
 * 参数：filePath 图片路径
 * 返回结果：
 *    识别成功返回json 的字节数组，大小是40960 字节，调用者负责释放这部分内存
 *    识别失败，返回 "-1"
 */
+(char *)reconize:(NSString *) filePath;

/*
 * 底层核心识别函数
 * 参数：jsonBuffer   缓存指针，建议大小40960 字节，用来接收识别结果
 *      bufferSize   缓存大小，建议40960 字节
 *      version      是卡版本号，默认0
 *      bitsperpixel 图片的每个像素用多少位来存储
 *      width        宽
 *      height       高
 *      bitsBuffer
 */
+(int) readBitmapBuffer:(char*)jsonBuffer
             bufferSize:(int) siz
                version:(int) v
           bitsperpixel:(int)bpp
                  width:(int)w
                 height:(int)h
             bitsBuffer:(unsigned char *) bitsbuf
;
@end
