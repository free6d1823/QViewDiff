#include "imagelib.h"

#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)


// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)

//////
/// \brief YuyvToRgb32 YUV420 to RGBA 32
/// \param pYuv
/// \param width
/// \param stride
/// \param height
/// \param pRgb     output RGB32 buffer
/// \param uFirst   true if pYuv is YUYV, false if YVYU
///
void YuyvToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb)
{
    //YVYU - format
    bool uFirst = true;
    int nBps = width*4;
    unsigned char* pY1 = pYuv;

    unsigned char* pV;
    unsigned char* pU;

    if (uFirst) {
        pU = pY1+1; pV = pU+2;
    } else {
        pV = pY1+1; pU = pV+2;
    }


    unsigned char* pLine1 = pRgb;

    unsigned char y1,u,v;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j+=2)
        {
            y1 = pY1[2*j];
            u = pU[2*j];
            v = pV[2*j];
            pLine1[j*4] = YUV2B(y1, u, v);//b
            pLine1[j*4+1] = YUV2G(y1, u, v);//g
            pLine1[j*4+2] = YUV2R(y1, u, v);//r
            pLine1[j*4+3] = 0xff;

            y1 = pY1[2*j+2];
            pLine1[j*4+4] = YUV2B(y1, u, v);//b
            pLine1[j*4+5] = YUV2G(y1, u, v);//g
            pLine1[j*4+6] = YUV2R(y1, u, v);//r
            pLine1[j*4+7] = 0xff;
        }
        pY1 += stride;
        pV += stride;
        pU += stride;
        pLine1 += nBps;

    }
}
void Yuy420ToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb)
{
    bool uFirst = false;
    //YVU420 - format 4Y:1V:1U
    int nBps = width*4;
    unsigned char* pY1 = pYuv;
    unsigned char* pY2 = pYuv+stride;

    unsigned char* pV;
    unsigned char* pU;
//try 422 plan
    if (uFirst) {
        pU = pY1+stride*height; pV = pU+width*height/4;
    } else {
        pV = pY1+stride*height; pU = pV+stride*height/4;
    }


    unsigned char* pLine1 = pRgb;
    unsigned char* pLine2 = pRgb+nBps;

    unsigned char y1,y2,u,v;
    for (int i=0; i<height; i+=2)
    {
        for (int j=0; j<width; j++)
        {
            y1 = pY1[j];
            u = pU[j/2];
            v = pV[j/2];
            pLine1[j*4] = YUV2B(y1, u, v);//b
            pLine1[j*4+1] = YUV2G(y1, u, v);//g
            pLine1[j*4+2] = YUV2R(y1, u, v);//r
            pLine1[j*4+3] = 0xff;

            y2 = pY2[j];

            pLine2[j*4] = YUV2B(y2, u, v);//b
            pLine2[j*4+1] = YUV2G(y2, u, v);//g
            pLine2[j*4+2] = YUV2R(y2, u, v);//r
            pLine2[j*4+3] = 0xff;

            j++;

            y1 = pY1[j];
            pLine1[j*4] = YUV2B(y1, u, v);//b
            pLine1[j*4+1] = YUV2G(y1, u, v);//g
            pLine1[j*4+2] = YUV2R(y1, u, v);//r
            pLine1[j*4+3] = 0xff;

            y2 = pY2[j];

            pLine2[j*4] = YUV2B(y2, u, v);//b
            pLine2[j*4+1] = YUV2G(y2, u, v);//g
            pLine2[j*4+2] = YUV2R(y2, u, v);//r
            pLine2[j*4+3] = 0xff;

        }
        pY1 = pY2 + stride;
        pY2 = pY1 + stride;
        pV += stride/2;
        pU += stride/2;
        pLine1 = pLine2 + nBps;
        pLine2 = pLine1 + nBps;

    }
}

void Yuy422ToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb)
{
    bool uFirst = false;
    //YVU422 - format 4Y:2V:2U
    int nBps = width*4;
    unsigned char* pY1 = pYuv;

    unsigned char* pV;
    unsigned char* pU;

    if (uFirst) {
        pU = pY1+stride*height; pV = pU+width*height/2;
    } else {
        pV = pY1+stride*height; pU = pV+stride*height/2;
    }


    unsigned char* pLine1 = pRgb;

    unsigned char y1,u,v;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            y1 = pY1[j];
            u = pU[j/2];
            v = pV[j/2];
            pLine1[j*4] = YUV2B(y1, u, v);//b
            pLine1[j*4+1] = YUV2G(y1, u, v);//g
            pLine1[j*4+2] = YUV2R(y1, u, v);//r
            pLine1[j*4+3] = 0xff;

            j++;
            y1 = pY1[j];

            pLine1[j*4] = YUV2B(y1, u, v);//b
            pLine1[j*4+1] = YUV2G(y1, u, v);//g
            pLine1[j*4+2] = YUV2R(y1, u, v);//r
            pLine1[j*4+3] = 0xff;

        }
        pY1 +=  stride;
        pV += stride/2;
        pU += stride/2;
        pLine1 += nBps;

    }
}

void Yuy444ToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb)
{
    bool uFirst = false;

    //YVU420 - format 4Y:1V:1U
    int nBps = width*4;
    unsigned char* pY1 = pYuv;
    unsigned char* pY2 = pYuv+stride;

    unsigned char* pV;
    unsigned char* pU;
//try 422 plan
    if (uFirst) {
        pU = pY1+stride*height; pV = pU+width*height/4;
    } else {
        pV = pY1+stride*height; pU = pV+stride*height/4;
    }


    unsigned char* pLine1 = pRgb;
    unsigned char* pLine2 = pRgb+nBps;

    unsigned char y1,y2,u,v;
    for (int i=0; i<height; i+=2)
    {
        for (int j=0; j<width; j++)
        {
            y1 = pY1[j];
            u = pU[j/2];
            v = pV[j/2];
            pLine1[j*4] = YUV2B(y1, u, v);//b
            pLine1[j*4+1] = YUV2G(y1, u, v);//g
            pLine1[j*4+2] = YUV2R(y1, u, v);//r
            pLine1[j*4+3] = 0xff;

            y2 = pY2[j];

            pLine2[j*4] = YUV2B(y2, u, v);//b
            pLine2[j*4+1] = YUV2G(y2, u, v);//g
            pLine2[j*4+2] = YUV2R(y2, u, v);//r
            pLine2[j*4+3] = 0xff;

            j++;

            y1 = pY1[j];
            pLine1[j*4] = YUV2B(y1, u, v);//b
            pLine1[j*4+1] = YUV2G(y1, u, v);//g
            pLine1[j*4+2] = YUV2R(y1, u, v);//r
            pLine1[j*4+3] = 0xff;

            y2 = pY2[j];

            pLine2[j*4] = YUV2B(y2, u, v);//b
            pLine2[j*4+1] = YUV2G(y2, u, v);//g
            pLine2[j*4+2] = YUV2R(y2, u, v);//r
            pLine2[j*4+3] = 0xff;

        }
        pY1 = pY2 + stride;
        pY2 = pY1 + stride;
        pV += stride/2;
        pU += stride/2;
        pLine1 = pLine2 + nBps;
        pLine2 = pLine1 + nBps;

    }
}
