#include "choselectpapersparam.h"

namespace choshiimgproc {

ChoSelectPapersParam::ChoSelectPapersParam( BaseChoImgProcParam *param )
  : BaseChoImgProcParam( param->getSrcImage(), param->getDstImage() ) {
  if( NULL != param->getResource() ) {
    m_resource = param->getResource();
    m_choshiImage = param->getChoshiImage();
    m_clusterList = m_resource->getClusterList();
  }
}

ChoSelectPapersParam::~ChoSelectPapersParam() {
  m_srcImage = NULL;
  m_dstImage = NULL;
  m_choshiImage = NULL;
}

choshiresource::ChoCluster* ChoSelectPapersParam::getClusterAt( int labelNum ) {
  return m_resource->getClusterAt( labelNum );
}

int ChoSelectPapersParam::getClusterSize() {
  return m_clusterList->size();
}

UsePapers ChoSelectPapersParam::getUsePapers() {
  return m_usePaper;
}

void ChoSelectPapersParam::setUsePapers( UsePapers ePapers ) {
  m_usePaper = ePapers;
}

cv::Vec3d ChoSelectPapersParam::getPaperColor( int index ) {
  cv::Vec3d paperColor;

  int paperSize = getPaperSize();
  if( paperSize < 0 ) {
    // TODO DebugMessage;
    goto ERROR_RETURN;
  }

  switch( m_colorMode ) {
  case COLORMODE_HVC: {
    paperColor = getPaperHVC( index );
    break;
  }
  case COLORMODE_LAB: {
    paperColor = getPaperLAB( index );
    break;
  }
  case COLORMODE_RGB: {
    paperColor = getPaperRGB( index );
    break;
  }
  default:
    // TODO DebugMessage;
    goto ERROR_RETURN;
  }
  return paperColor;

ERROR_RETURN:
  return cv::Vec3d( -1, -1, -1 );
}

cv::Vec3d ChoSelectPapersParam::getPaperHVC( int index ) {
  cv::Vec3d paperHVC;
  int paperSize = getPaperSize();
  if( paperSize < 0 || paperSize <= index ) {
    // TODO DebugMessage;
    goto ERROR_RETURN;
  }
  switch( m_usePaper ) {
  case JISZ8102: {
    paperHVC = cv::Vec3d( paperJISZ8102[index].h, paperJISZ8102[index].v, paperJISZ8102[index].c );
    break;
  }
  default:
    // TODO DebugMessage;
    goto ERROR_RETURN;
  }
  return paperHVC;
ERROR_RETURN:
  return cv::Vec3d( -1, -1, -1 );
}

cv::Vec3d ChoSelectPapersParam::getPaperLAB( int index ) {
  cv::Vec3d paperLAB;
  int paperSize = getPaperSize();
  if( paperSize < 0 || paperSize <= index ) {
    // TODO DebugMessage;
    goto ERROR_RETURN;
  }
  switch( m_usePaper ) {
  case JISZ8102: {
    paperLAB = cv::Vec3d( paperJISZ8102[index].l, paperJISZ8102[index].a, paperJISZ8102[index].bforLAB );
    break;
  }
  default:
    // TODO DebugMessage;
    goto ERROR_RETURN;
  }
  return paperLAB;
ERROR_RETURN:
  return cv::Vec3d( -1, -1, -1 );
}

cv::Vec3d ChoSelectPapersParam::getPaperRGB( int index ) {
  cv::Vec3d paperRGB;
  int paperSize = getPaperSize();
  if( paperSize < 0 || paperSize <= index ) {
    // TODO DebugMessage;
    goto ERROR_RETURN;
  }
  switch( m_usePaper ) {
  case JISZ8102: {
    paperRGB = cv::Vec3d( paperJISZ8102[index].r, paperJISZ8102[index].g, paperJISZ8102[index].b );
    break;
  }
  default:
    // TODO DebugMessage;
    goto ERROR_RETURN;
  }
  return paperRGB;

ERROR_RETURN:
  return cv::Vec3d( -1, -1, -1 );
}

int ChoSelectPapersParam::getPaperSize() {
  int paperSize = 0;
  switch( m_usePaper ) {
  case JISZ8102: {
    paperSize = arrayLength( paperJISZ8102 );
    break;
  }
  default:
    paperSize = -1;
    break;
  }
  return paperSize;
}

std::string ChoSelectPapersParam::getPaperName( int index ) {
  int paperSize = getPaperSize();
  std::string paperName;
  if( paperSize < 0 || paperSize <= index ) {
    // TODO DebugMessage
    goto ERROR_RETURN;
  }
  switch( m_usePaper ) {
  case JISZ8102: {
    paperName = paperJISZ8102[index].name;
    break;
  }
  default:
    // TODO DebugMessage
    goto ERROR_RETURN;
  }
  return paperName;

ERROR_RETURN:
  return "";
}

} /* namespace choshiimgproc */
