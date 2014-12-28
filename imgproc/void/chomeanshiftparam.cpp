#include "chomeanshiftparam.h"

namespace choshiimgproc {

ChoMeanShiftParam::ChoMeanShiftParam( BaseChoImgProcParam *param )
  : BaseChoImgProcParam( param->getSrcImage(), param->getDstImage() ) {
  if( NULL != param->getResource() ) {
    m_resource = param->getResource();
    m_choshiImage = param->getChoshiImage();
  }
}

ChoMeanShiftParam::~ChoMeanShiftParam() {
  m_srcImage = NULL;
  m_dstImage = NULL;
  m_choshiImage = NULL;
}

void ChoMeanShiftParam::setSpatialRadius( float sRadius ) {
  m_sRadius = sRadius;
}

double ChoMeanShiftParam::getSpatialRadius() {
  return m_sRadius;
}

void ChoMeanShiftParam::setColorRadius( float cRadius ) {
  m_cRadius = cRadius;
}

double ChoMeanShiftParam::getColorRadius() {
  return m_cRadius;
}

void ChoMeanShiftParam::setCvTermCriteria( cv::TermCriteria termCriteria ) {
  m_termCriteria = termCriteria;
}

cv::TermCriteria ChoMeanShiftParam::getCvTermCriteria() {
  return m_termCriteria;
}

} /* namespace choshiimgproc */
