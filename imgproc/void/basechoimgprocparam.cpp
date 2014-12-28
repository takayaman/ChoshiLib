#include "basechoimgprocparam.h"

namespace choshiimgproc {

BaseChoImgProcParam::BaseChoImgProcParam( const cv::Mat *srcImg, cv::Mat *dstImg ) {
  m_srcImage = const_cast<cv::Mat *>( srcImg );
  m_dstImage = dstImg;
}

BaseChoImgProcParam::~BaseChoImgProcParam() {
  m_srcImage = NULL;
  m_dstImage = NULL;
}

int BaseChoImgProcParam::getImageWidth() {
  return m_srcImage->cols;
}

int BaseChoImgProcParam::getImageHeight() {
  return m_srcImage->rows;
}

int BaseChoImgProcParam::getImageChannels() {
  return m_srcImage->channels();
}

int BaseChoImgProcParam::getImageStep() {
  return m_srcImage->step;
}

int BaseChoImgProcParam::getImageElementSize() {
  return m_srcImage->elemSize();
}

void BaseChoImgProcParam::setColorMode( ImgProcColorMode eMode ) {
  m_colorMode = eMode;
  m_resource->setColorMode( (choshiresource::ChoshiColorMode)eMode );
}

ImgProcColorMode BaseChoImgProcParam::getColorMode() {
  return m_colorMode;
}

cv::Mat* BaseChoImgProcParam::getSrcImage() {
  return m_srcImage;
}

cv::Mat* BaseChoImgProcParam::getDstImage() {
  return m_dstImage;
}

cv::Mat* BaseChoImgProcParam::getChoshiImage() {
  return m_resource->getChoshiImage();
}

void BaseChoImgProcParam::setResource( choshiresource::ChoResource *resource ) {
  m_resource = resource;
}

choshiresource::ChoResource* BaseChoImgProcParam::getResource() {
  return m_resource;
}

bool BaseChoImgProcParam::reflectClusterValToChoshiImage() {
  return m_resource->reflectClusterValToChoshiImage();
}

} /* namespace choshiimgproc */
