#include "ichoimgproc.h"
#include "imgprocutil.h"

namespace choshiimgproc{

IChoImgProc::IChoImgProc()
{
}

IChoImgProc::~IChoImgProc()
{
}

bool IChoImgProc::init( BaseChoImgProcParam *param )
{
    m_param = param;
    return true;
}

bool IChoImgProc::clear()
{
    return true;
}

bool IChoImgProc::execute()
{
    return true;
}

// TODO move this function to ChoImgProc
void IChoImgProc::createDstImage()
{
    cv::Mat *choshiImg = m_param->getChoshiImage();
    cv::Mat *dstImg = m_param->getDstImage();
    int width = m_param->getImageWidth();
    int height = m_param->getImageHeight();
    ImgProcColorMode eMode = m_param->getColorMode();
    cv::Vec3d color;
    cv::Vec3b rgb8;
    for( int y = 0; y < height; y++ ){
        for( int x = 0; x < width; x++ ){
            color.val[0] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[choshiresource::CHANNEL_R];
            color.val[1] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[choshiresource::CHANNEL_G];
            color.val[2] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[choshiresource::CHANNEL_B];

            /* select convert algorithm */
            switch( eMode ){
            case COLORMODE_RGB:
            {
                color.val[0] = floor( color.val[0] );
                color.val[1] = floor( color.val[1] );
                color.val[2] = floor( color.val[2] );
                if( 0 > color.val[0] )
                    color.val[0] = 0;
                if( 0 > color.val[1] )
                    color.val[1] = 0;
                if( 0 > color.val[2] )
                    color.val[2] = 0;
                if( 255 < color.val[0] )
                    color.val[0] = 255;
                if( 255 < color.val[1] )
                    color.val[1] = 255;
                if( 255 < color.val[2] )
                    color.val[2] = 255;
                rgb8.val[0] = (uchar)color.val[2];
                rgb8.val[1] = (uchar)color.val[1];
                rgb8.val[2] = (uchar)color.val[0];
                break;
            }
            case COLORMODE_HVC:
            {
                cv::Vec3d rgbLinear = ImgProcUtil::calculateReverseMTM( color, NTSC_C );
                rgb8 = ImgProcUtil::convertIntegerRGB( rgbLinear );
                break;
            }
            case COLORMODE_LAB:
            {
                break;
            }
            default:
                break;
            }
            dstImg->at<cv::Vec3b>( y, x ) = rgb8;
        }
    }
}

ImgProcColorMode IChoImgProc::convertColor( ConvertColorMode eMode )
{
    if( eMode >= CONVERTMODE_NUM ){
        // TODO DebugMessage
        return COLORMODE_NUM;
    }

    ImgProcColorMode retVal = COLORMODE_NUM;

    cv::Mat *choshiImg = m_param->getChoshiImage();
    int width = m_param->getImageWidth();
    int height = m_param->getImageHeight();
    cv::Vec3d color;
    cv::Vec3d convertColor;
    for( int y = 0; y < height; y++ ){
        for( int x = 0; x < width; x++ ){
            color.val[0] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[0];
            color.val[1] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[1];
            color.val[2] = choshiImg->at<CVVEC_CHOSHI>( y, x ).val[2];

            /* select convert algorithm */
            switch( eMode ){
            case RGB2LAB:
            {
                color = ImgProcUtil::convertLinearRGB( color );
                convertColor = ImgProcUtil::calculateLAB( color, NTSC_C );
                retVal = COLORMODE_LAB;
                break;
            }
            case RGB2LAB10:
            {
                color = ImgProcUtil::convertLinearRGB( color );
                convertColor = ImgProcUtil::calculateLAB10( color, NTSC_C );
                retVal = COLORMODE_LAB10;
                break;
            }
            case RGB2HVC:
            {
                color = ImgProcUtil::convertLinearRGB( color );
                convertColor = ImgProcUtil::calculateMTM( color, NTSC_C );
                retVal = COLORMODE_HVC;
                break;
            }
            case RGB2HVC10:
            {
                color = ImgProcUtil::convertLinearRGB( color );
                convertColor = ImgProcUtil::calculateMTM( color, NTSC_C );
                retVal = COLORMODE_HVC10;
                break;
            }
            default: break;
            } // TODO imprement reverse conversion
            choshiImg->at<CVVEC_CHOSHI>( y, x ).val[0] = convertColor.val[0];
            choshiImg->at<CVVEC_CHOSHI>( y, x ).val[1] = convertColor.val[1];
            choshiImg->at<CVVEC_CHOSHI>( y, x ).val[2] = convertColor.val[2];
        }
    }
    return retVal;
}



} /* namespace choshiimgproc */
