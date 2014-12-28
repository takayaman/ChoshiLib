#include "imgprocutil.h"
#include <math.h>

namespace choshiimgproc{

#define LABFUNC_THRESHOLD 0.008856
#define NTSC_XN 0.98072166484219
#define NTSC_ZN 1.182253809803959
#define MAX_ITERATION 1000
#define ACCEPT_ERROR 0.000000000001

ImgProcUtil::ImgProcUtil()
{
}

cv::Vec3d ImgProcUtil::convertLinearRGB( cv::Vec3b rgb8 )
{
    cv::Vec3d rgbLinear;
    for( int i = 0; i < 3; i++ ){
        rgbLinear.val[i] = (double)rgb8.val[i] / 255.0;
    }
    return rgbLinear;
}

cv::Vec3b ImgProcUtil::convertIntegerRGB( cv::Vec3d rgbLinear )
{
    cv::Vec3d rgb8;
    for( int i = 0; i < 3; i++ ){
        rgb8.val[i] = floor( rgbLinear.val[i] * 255.0 );
        if( 0 > rgb8.val[i] )
            rgb8.val[i] = 0;
        if( 255 < rgb8.val[i] )
            rgb8.val[i] = 255;
    }
    return cv::Vec3b( (uchar)rgb8.val[0], (uchar)rgb8.val[1], (uchar)rgb8.val[2] );
}

cv::Vec3d ImgProcUtil::calculateXYZ( cv::Vec3d rgbLinear, DefColorAndWhite eMode )
{
    cv::Vec3d dColor;
    double X = calcX( rgbLinear, eMode );
    double Y = calcY( rgbLinear, eMode );
    double Z = calcZ( rgbLinear, eMode );
    dColor.val[0] = X;
    dColor.val[1] = Y;
    dColor.val[2] = Z;

    return dColor;
}

cv::Vec3d ImgProcUtil::calculateXYZ10( cv::Vec3d rgbLinear, DefColorAndWhite eMode )
{
    cv::Vec3d dColor;
    double X = calcX10( rgbLinear, eMode );
    double Y = calcY10( rgbLinear, eMode );
    double Z = calcZ10( rgbLinear, eMode );
    dColor.val[0] = X;
    dColor.val[1] = Y;
    dColor.val[2] = Z;
    return dColor;
}

double ImgProcUtil::calcX( cv::Vec3d color, DefColorAndWhite eMode )
{
    cv::Vec3d coeffient;
    switch( eMode ){
    case SRGB_D65:
    {
        break;
    }
    case ADOBE_D65:
    {
        break;
    }
    case NTSC_C:
    {
        coeffient = cv::Vec3d(1.02 * 0.608, 1.02 * 0.174, 1.02 * 0.200);
        break;
    }
    default:
        break;
    }
    return color.dot( coeffient );
}

double ImgProcUtil::calcY( cv::Vec3d color, DefColorAndWhite eMode )
{
    cv::Vec3d coeffient;
    switch( eMode ){
    case SRGB_D65:
    {
        break;
    }
    case ADOBE_D65:
    {
        break;
    }
    case NTSC_C:
    {
        coeffient = cv::Vec3d( 0.299, 0.587, 0.144 );
        break;
    }
    default:
        break;
    }
    return color.dot( coeffient );
}

double ImgProcUtil::calcZ( cv::Vec3d color, DefColorAndWhite eMode )
{
    cv::Vec3d coeffient;
    switch( eMode ){
    case SRGB_D65:
    {
        break;
    }
    case ADOBE_D65:
    {
        break;
    }
    case NTSC_C:
    {
        coeffient = cv::Vec3d( 0.0, 0.847 * 0.066, 0.847 * 1.112 );
        break;
    }
    default:
        break;
    }
    return color.dot( coeffient );
}

double ImgProcUtil::calcX10( cv::Vec3d color, DefColorAndWhite eMode )
{
    cv::Vec3d coeffient;
    switch( eMode ){
    case SRGB_D65:
    {
        break;
    }
    case ADOBE_D65:
    {
        break;
    }
    case NTSC_C:
    {
        break;
    }
    default:
        break;
    }
    return color.dot( coeffient );
}

double ImgProcUtil::calcY10( cv::Vec3d color, DefColorAndWhite eMode )
{
    cv::Vec3d coeffient;
    switch( eMode ){
    case SRGB_D65:
    {
        break;
    }
    case ADOBE_D65:
    {
        break;
    }
    case NTSC_C:
    {
        break;
    }
    default:
        break;
    }
    return color.dot( coeffient );
}

double ImgProcUtil::calcZ10( cv::Vec3d color, DefColorAndWhite eMode )
{
    cv::Vec3d coeffient;
    switch( eMode ){
    case SRGB_D65:
    {
        break;
    }
    case ADOBE_D65:
    {
        break;
    }
    case NTSC_C:
    {
        break;
    }
    default:
        break;
    }
    return color.dot( coeffient );
}

/* calculate MTM */
cv::Vec3d ImgProcUtil::calculateMTM( cv::Vec3d rgb, DefColorAndWhite eMode )
{
    cv::Vec3d xyzColor = calculateXYZ( rgb, eMode );
    double M1 = calcM1( xyzColor );
    double M2 = calcM2( xyzColor );
    double M3 = calcM3( xyzColor );
    double AdamsH = calcAdamsH( xyzColor );
    double S1 = ( 8.88 + 0.966 * cos( AdamsH ) ) * M1;
    double S2 = ( 8.025 + 2.558 * sin( AdamsH ) ) * M2;
    /* calculate MTMH */
    double H = atan2( S2, S1 ) * 180 / M_PI;
    if( H < 0 )
        H += 360;
    /* calculate MTMV */
    double V = M3 / 0.23;
    if(V < 0)
        V = 0;
    /* calculate MTMC */
    double C = sqrt( pow( S1, 2.0 ) + pow( S2, 2.0 ) );
    if(C < 0)
        C = 0;
    cv::Vec3d mtmColor( H, V, C );
    return mtmColor;
}

double ImgProcUtil::calcV( double a )
{
    if( LABFUNC_THRESHOLD > a ){
        return 11.6 * (( pow(29/3, 0.333333 ) * a + 16 ) / 116) - 1.6;
    }
    return 11.6 * pow( a, 0.333333 ) - 1.6;
}

double ImgProcUtil::calcM1( cv::Vec3d xyzColor )
{
    return calcV( xyzColor.val[0] ) - calcV( xyzColor.val[1] );
}

double ImgProcUtil::calcM2( cv::Vec3d xyzColor )
{
    return 0.4 * ( calcV( xyzColor.val[2] ) - calcV( xyzColor.val[1] ) );
}

double ImgProcUtil::calcM3( cv::Vec3d xyzColor )
{
    return 0.23 * ( calcV( xyzColor.val[1] ) );
}

double ImgProcUtil::calcAdamsH( cv::Vec3d xyzColor )
{
    double M1 = calcM1( xyzColor );
    double M2 = calcM2( xyzColor );
    return atan2( M2, M1 );
}

double ImgProcUtil::calcAdamsV( cv::Vec3d xyzColor )
{
    return calcM3( xyzColor );
}

double ImgProcUtil::calcAdamsC( cv::Vec3d xyzColor )
{
    double M1 = calcM1( xyzColor );
    double M2 = calcM2( xyzColor );
    return sqrt( pow( M1, 2.0 ) + pow( M2, 2.0 ) );
}

/* calculateReverseMTM TODO implement D45 mode */
cv::Vec3d ImgProcUtil::calculateReverseMTM( cv::Vec3d hvc, DefColorAndWhite eMode )
{
    double tempH1 = 1;
    double tempH2 = 1;
    double tempS1 = 0;
    double tempS2 = 0;
    double S1 = 0;
    double S2 = 0;
    double deltaS1 = 0;
    double deltaS2 = 0;
    double deltaH1 = 0;
    double deltaH2 = 0;
    double deltaValue = 0;
    double determinant = 0;

    cv::Vec3d reverseXYZ = cv::Vec3d( 0, 0, 0 );
    cv::Vec3d reverseRGB = cv::Vec3d( 0, 0, 0 );

    reverseXYZ.val[1] = pow(( ( hvc.val[1] + 1.6 ) / 11.6 ), 3.0); /* Y = ((MTMV+1.6)/11.6)^3 */
    S1 = hvc.val[2] * cos( hvc.val[0] * M_PI / 180 );
    S2 = hvc.val[2] * sin( hvc.val[0] * M_PI / 180 );

    /* calculate equation of H1, H2 using newton method */
    for( int i = 0; i < MAX_ITERATION; i++ ){
        double doubleTempH1 = pow( tempH1, 2.0 );
        double doubleTempH2 = pow( tempH2, 2.0 );

        double tempDenominator0 = doubleTempH1 + 0.16 * doubleTempH2;
        tempDenominator0 = pow( tempDenominator0, (1.0/2.0) ); /* (H1^2 + 0.16*H2^2)^(1/2) */
        double tempDenominator1 = doubleTempH1 + 0.16 * doubleTempH2;
        tempDenominator1 = pow( tempDenominator1, (3.0/2.0) ); /* (H1^2 + 0.16*H2^2)^(3/2) */
        tempS1 = tempH1 * ( 8.88 + 0.966 * tempH1 / tempDenominator0 );
        tempS2 = tempH2 * ( 3.21 + 0.409 * tempH2 / tempDenominator0 );
        /* calculate error */
        deltaS1 = tempS1 - S1;
        deltaS2 = tempS2 - S2;

        /* calculate inverse matrix
         * each element is partial differentiation
         * a11: roundS1/roundH1, a12:roundS1/roundH2, a21:roundS2/roundH1, a22:roundS2/roundH2
         */
        double a11 = 8.88 + 2 * 0.966 * tempH1 / tempDenominator0 - 0.966 * pow( tempH1, 3.0 ) / tempDenominator1;
        double a12 = -0.16 * 0.966 * doubleTempH1 * tempH2 / tempDenominator1;
        double a21 = -0.409 * tempH1 * doubleTempH2 / tempDenominator1;
        double a22 = 3.21 + 2.0 * 0.409 * tempH2 / tempDenominator0 - 0.16 * 0.409 * pow( tempH2, 3.0 ) / tempDenominator1;
        determinant = a11 * a22 - a12 * a21;
        double b11 = a22 / determinant;
        double b12 = -a12 / determinant;
        double b21 = -a21 / determinant;
        double b22 = a11 / determinant;
        /* calculate error */
        deltaH1 = b11 * deltaS1 + b12 * deltaS2;
        deltaH2 = b21 * deltaS1 + b22 * deltaS2;
        tempH1 = tempH1 - deltaH1;
        tempH2 = tempH2 - deltaH2;
        deltaValue = pow(deltaH1, 2.0) + pow(deltaH2, 2.0);

        /* end condition */
        if( deltaValue < ACCEPT_ERROR * (pow(tempH1, 2.0) + pow(tempH2, 2.0)) ){
            reverseXYZ.val[0] = 0.98 * pow( ((hvc.val[1] + tempH1 + 1.6) / 11.6), 3.0 );
            reverseXYZ.val[2] = 1.181 * pow( ((hvc.val[1] + tempH2 + 1.6) / 11.6), 3.0);
            break;
        }
    }

    if( 0 == reverseXYZ.val[0] ){
        // TODO DebugMessage
        return cv::Vec3d( -1, -1, -1 );
    }

    reverseRGB.val[0] = reverseXYZ.dot( cv::Vec3d( 1.908, -0.535, -0.274 ) );
    reverseRGB.val[1] = reverseXYZ.dot( cv::Vec3d( -0.986, 2.005, -0.0823 ) );
    reverseRGB.val[2] = reverseXYZ.dot( cv::Vec3d( 0.0583, -0.119, 0.904 ) );

    return reverseRGB;
}

/* calculateLAB */
cv::Vec3d ImgProcUtil::calculateLAB( cv::Vec3d rgb, DefColorAndWhite eMode )
{
    cv::Vec3d xyzColor = calculateXYZ( rgb, eMode );
    xyzColor.val[0] = xyzColor.val[0] / 1.02;
    xyzColor.val[2] = xyzColor.val[2] / 0.847;
    double L = calcLABL( xyzColor );
    double A = calcLABA( xyzColor );
    double B = calcLABB( xyzColor );
    cv::Vec3d labColor( L, A, B );
    return labColor;
}

double ImgProcUtil::calcLABL( cv::Vec3d color )
{
    return  116 * calcFunc( color.val[1] ) - 16;
}

double ImgProcUtil::calcLABA( cv::Vec3d color )
{
    double x = calcFunc( color.val[0] / NTSC_XN );
    double y = calcFunc( color.val[1] );
    return 500 * ( x - y );
}

double ImgProcUtil::calcLABB( cv::Vec3d color )
{
    double y = calcFunc( color.val[1] );
    double z = calcFunc( color.val[2] / NTSC_ZN );
    return 200 * ( y - z );
}

double ImgProcUtil::calcFunc( double input )
{
    if( LABFUNC_THRESHOLD > input ){
        return ( pow(29/3, 3 ) * input + 16 ) / 116;
    }
    return pow( input, 0.333333 );
}

cv::Vec3d ImgProcUtil::calculateLAB10( cv::Vec3d rgb, DefColorAndWhite eMode )
{

}

double ImgProcUtil::calcLAB10L( cv::Vec3d color )
{

}

double ImgProcUtil::calcLAB10A( cv::Vec3d color )
{

}

double ImgProcUtil::calcLAB10B( cv::Vec3d color )
{

}

cv::Vec3d ImgProcUtil::calculateReverseLAB( cv::Vec3d lab, DefColorAndWhite eMode )
{
    double fx;
    double fy;
    double fz;
    double X;
    double Y;
    double Z;
    double R;
    double G;
    double B;
    fy = ( lab.val[0] + 16 ) / 116;
    fx = fy + ( lab.val[1] / 500 );
    fz = fy - ( lab.val[2] / 200 );
    if( fx > 6 / 29 ){
        X = pow( fx, 3.0 ) * NTSC_XN;
    }else{
        X = pow( 3.0 / 29.0, 3.0 ) * ( 116 * fx - 16 ) * NTSC_XN;
    }
    if( fy > 6 / 29 ){
        Y = pow( fy, 3.0 );
    }else{
        Y = pow( 3.0 / 29.0, 3.0 ) * ( 116 * fy - 16 );
    }
    if( fz > 6 / 29 ){
        Z = pow( fz, 3.0 ) * NTSC_ZN;
    }else{
        Z = pow( 3.0 / 29.0, 3.0 ) * ( 116 * fz - 16 ) * NTSC_ZN;
    }
    R = 1.908 * X - 0.535 * Y - 0.274 * Z;
    G = -0.986 * X + 2.005 * Y - 0.0823 * Z;
    B = 0.0585 * X - 0.119 * Y + 0.904 * Z;
    return cv::Vec3d( R, G, B );
}

double ImgProcUtil::calculateDiff( cv::Vec3d color0, cv::Vec3d color1, ImgProcColorMode eMode )
{
    double diff = 0;
    switch (eMode) {
    case COLORMODE_RGB:
        diff = calcDiffRGB( color0, color1 );
        break;
    case COLORMODE_LAB:
    case COLORMODE_LAB10:
        diff = calcDiffLAB( color0, color1 );
        break;
    case COLORMODE_HVC:
    case COLORMODE_HVC10:
        diff = calcDiffHVC( color0, color1 );
        break;
    default:
        break;
    }
    return diff;
}

double ImgProcUtil::calcDiffRGB( cv::Vec3d rgb0, cv::Vec3d rgb1 )
{
    double diffR = fabs(rgb0.val[0] - rgb1.val[0]);
    double diffG = fabs(rgb0.val[1] - rgb1.val[1]);
    double diffB = fabs(rgb0.val[2] - rgb1.val[2]);
    return sqrt( pow( diffR, 2.0 ) + pow( diffG, 2.0 ) + pow( diffB, 2.0 ) );
}

double ImgProcUtil::calcDiffLAB( cv::Vec3d lab0, cv::Vec3d lab1 )
{
    double diffL = fabs(lab0.val[0] - lab1.val[0]);
    double diffA = fabs(lab0.val[1] - lab1.val[1]);
    double diffB = fabs(lab0.val[2] - lab1.val[2]);
    return sqrt( pow( diffL, 2.0 ) + pow( diffA, 2.0 ) + pow( diffB, 2.0 ) );
}

double ImgProcUtil::calcDiffHVC( cv::Vec3d hvc0, cv::Vec3d hvc1 )
{
    double diffH = fabs( hvc0.val[0] - hvc1.val[0] );
    double diffV = fabs( hvc0.val[1] - hvc1.val[1] );
    double diffC = fabs( hvc0.val[2] - hvc1.val[2] );
    /* calculate Godlove difference */
    /* 2*C1*C2(1-cos(2PIdH/100)+(dC)^2+(4dV)^2)^{1/2} */
    //double deltaE = 2 * hvc0.val[2] * hvc1.val[2] * ( 1 - cos( 2 * M_PI * diffH / 100 ) ) + pow( diffC, 2.0 ) + pow( 4 * diffV, 2.0 );
    //diffH = diffH * 100 / 360;
    diffH = diffH * M_PI / 180;
    double deltaE = 2 * hvc0.val[2] * hvc1.val[2] * ( 1 - cos( diffH ) ) + pow( diffC, 2.0 ) + pow( 4.0 * diffV, 2.0 );
    return sqrt( deltaE );
}

void ImgProcUtil::writeMask( cv::Mat *srcChoshiImg, cv::Mat *dstMaskImg, int labelNum )
{
    if( !srcChoshiImg->data || !dstMaskImg->data ){
        // TODO DebugMessage
        return;
    }

    for( int y = 0; y < srcChoshiImg->size().height; y++ ){
        for( int x = 0; x < srcChoshiImg->size().width; x++ ){
            cv::Vec6d tempVal = srcChoshiImg->at<CVVEC_CHOSHI>( y, x );
            // TODO make common CHANNEL MACRO
            if( labelNum == tempVal.val[srcChoshiImg->channels() - 1] || dstMaskImg->at<uchar>( y, x ) == 255 ){
                dstMaskImg->at<uchar>( y, x ) = 255;
            }else{
                dstMaskImg->at<uchar>( y, x ) = 0;
            }
        }
    }

}

void ImgProcUtil::writeContour( cv::Mat *srcMaskImg, cv::Mat *dstContourImg )
{
    /* find countour of mass */
    for( int y = 0; y < srcMaskImg->size().height; y++ ){
        for( int x = 0; x < srcMaskImg->size().width; x++ ){
            /* ignore when pixel is not white */
            if( 255 != srcMaskImg->at<uchar>( y, x ) )
                continue;

            /* border of image is contour */
            if( 0 == y || ( srcMaskImg->size().height - 1 == y ) || 0 == x || srcMaskImg->size().width - 1 == x ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }

            /* search 8-neighbers */
            /* top-left */
            if( 0 == srcMaskImg->at<uchar>( y - 1, x - 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* top-middle */
            if( 0 == srcMaskImg->at<uchar>( y - 1, x) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* top-right */
            if( 0 == srcMaskImg->at<uchar>( y - 1, x + 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* middle-left */
            if( 0 == srcMaskImg->at<uchar>( y, x - 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* middle-right */
            if( 0 == srcMaskImg->at<uchar>( y, x + 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* bottom-left */
            if( 0 == srcMaskImg->at<uchar>( y + 1, x - 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* bottom-middle */
            if( 0 == srcMaskImg->at<uchar>( y + 1, x) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
            /* bottom-right */
            if( 0 == srcMaskImg->at<uchar>( y + 1, x + 1) ){
                dstContourImg->at<uchar>( y, x ) = 255;
                continue;
            }
        }
    }
}

} /* namespace choshiimgproc */
