#include "choimgproc.h"

namespace choshiimgproc {

ChoImgProc::ChoImgProc() {
}

ChoImgProc::ChoImgProc( const std::string &filename ) {
  m_inputImage = cv::imread( filename, cv::IMREAD_COLOR );
  m_dispImage = m_inputImage;
}

ChoImgProc::~ChoImgProc() {
  m_inputImage.release();
  m_dispImage.release();
}

bool ChoImgProc::setImage( const std::string &filename ) {
  if( !m_inputImage.empty() )
    m_inputImage.release();
  m_inputImage = cv::imread( filename, cv::IMREAD_COLOR );
  if( m_inputImage.empty() ) {
    // TODO DebugMessage
    return false;
  }
  // TODO check dispimage has only reference or allocated new memory
  m_dispImage = m_inputImage;
  return true;
}

bool ChoImgProc::setImage( cv::Mat *image ) {
  if( !m_inputImage.empty() )
    m_inputImage.release();
  m_inputImage = image->clone();
  if( m_inputImage.empty() ) {
    // TODO DebugMessage
    return false;
  }
  // TODO check dispimage has only reference or allocated new memory
  m_dispImage = m_inputImage;
  return true;
}

void ChoImgProc::setImgProc( const IChoImgProc *proc ) {
  m_proc = const_cast<IChoImgProc *>( proc );
}

cv::Mat* ChoImgProc::getInputImage() {
  return &m_inputImage;
}

cv::Mat* ChoImgProc::getDispImage() {
  return &m_dispImage;
}

bool ChoImgProc::init( BaseChoImgProcParam *param ) {
  if( !m_proc ) {
    // TODO DebugMessage
    return false;
  }
  return m_proc->init( param );
}

bool ChoImgProc::clear() {
  return m_proc->clear();
}

bool ChoImgProc::execute() {
  return m_proc->execute();
}

ImgProcColorMode ChoImgProc::convertColor( ConvertColorMode eMode ) {
  return m_proc->convertColor( eMode );
}

void ChoImgProc::createDstImage() {
  return m_proc->createDstImage();
}


} /* namespace choshiimgproc */
