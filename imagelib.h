#ifndef IMAGELIB_H
#define IMAGELIB_H

typedef void (* ColorspaceConvertFunc)(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb);

void YuyvToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb);
void Yuy420ToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb);
void Yuy422ToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb);
void Yuy444ToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb);


#endif // IMAGELIB_H
