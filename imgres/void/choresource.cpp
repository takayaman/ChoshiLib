/******************************************************************************
 * Project      : ChoResource
 * File         : choresource.cpp
 * Copyright    : 2013 (c) Natsuki Takayama, UEC. Tokyo
 * The source file of ChoResource class.
 *****************************************************************************/

#include "choresource.h"

namespace choshiresource {

#define ROOT_TAG "ChoshiResource"

/********************************************************************
 * Constructor/Destructor
 *******************************************************************/

ChoResource::ChoResource()
  : m_resourcePath("")
  , m_imageName("")
  , m_xmlName("") {
  m_colorMode = COLORMODE_RGB;
}

ChoResource::~ChoResource() {
  this->clear();
}

void ChoResource::clear() {
  ChoCluster *cluster = NULL;
  std::list<ChoCluster *>::iterator it = m_clusterList.begin();
  while( it != m_clusterList.end() ) {
    cluster = *it;
    if( NULL != cluster ) {
      delete cluster;
    }
    it++;
  }
  m_clusterList.clear();
  m_choshiImage.release();
  m_maskImage.release();
}

/********************************************************************
 * Accessor
 *******************************************************************/

int ChoResource::getImageWidth() {
  return m_choshiImage.size().width;
}

int ChoResource::getImageHeight() {
  return m_choshiImage.size().height;
}

int ChoResource::getColorChannelNum() {
  return m_choshiImage.channels() - CHANNELS_XYLABEL;
}

int ChoResource::getLabelAt( int x, int y ) {
  cv::Vec6d pixel = m_choshiImage.at<cv::Vec6d>( y, x );
  return (int)pixel[CHANNEL_LABEL];
}

void ChoResource::setResourcePath( std::string path ) {
  m_resourcePath = path;
  m_imageName = path + "/choconv.bin";
  m_xmlName = path + "/chocluster.xml";
}

std::list<ChoCluster *>* ChoResource::getClusterList() {
  return &m_clusterList;
}

ChoCluster* ChoResource::getClusterAt( int labelNum ) {
  ChoCluster *cluster = NULL;
  std::list<ChoCluster *>::iterator it = m_clusterList.begin();
  while( it != m_clusterList.end() ) {
    cluster = *it;
    if( labelNum == cluster->m_labelNum )
      return cluster;
    it++;
  }
  return NULL;
}

cv::Mat* ChoResource::getChoshiImage( void ) {
  return &m_choshiImage;
}

ChoshiColorMode ChoResource::getColorMode() {
  return m_colorMode;
}

void ChoResource::setColorMode( ChoshiColorMode eMode ) {
  m_colorMode = eMode;
}

int ChoResource::getClusterSize() {
  return m_clusterList.size();
}

/********************************************************************
 * Handling image
 *******************************************************************/

cv::Mat* ChoResource::createChoshiImage( const cv::Mat *srcImg ) {
  if( NULL == srcImg ) {
    // TODO DebugMessage
    return NULL;
  } else if( CHANNELS_COLOR != srcImg->channels() ) {
    // TODO DebugMessage
    return NULL;
  }
  int height = srcImg->size().height;
  int width = srcImg->size().width;

  m_maskImage = cv::Mat::zeros( height, width, CV_8UC1 );
  m_choshiImage = cv::Mat::zeros( height, width, CV_64FC(6) );
  m_colorMode = COLORMODE_RGB;
  for( int y = 0; y < height; y++ ) {
    for( int x = 0; x < width; x++ ) {
      cv::Vec3b debug = srcImg->at<cv::Vec3b>( y, x );
      cv::Vec3b temp = debug;
      m_choshiImage.at<cv::Vec6d>( y, x ).val[CHANNEL_R] = (double)srcImg->at<cv::Vec3b>( y, x ).val[CHANNEL_B];
      m_choshiImage.at<cv::Vec6d>( y, x ).val[CHANNEL_G] = (double)srcImg->at<cv::Vec3b>( y, x ).val[CHANNEL_G];
      m_choshiImage.at<cv::Vec6d>( y, x ).val[CHANNEL_B] = (double)srcImg->at<cv::Vec3b>( y, x ).val[CHANNEL_R];
    }
  }

  return &m_choshiImage;
}

/*-------------------------------------------------------*/

bool ChoResource::reflectClusterValToChoshiImage() {
  int height = m_choshiImage.rows;
  int width = m_choshiImage.cols;

  for( int y = 0; y < height; y++ ) {
    for( int x = 0; x < width; x++ ) {
      int label = (int)m_choshiImage.at<cv::Vec6d>( y, x ).val[CHANNEL_LABEL];
      ChoCluster *cluster = getClusterAt( label );
      for( int nChannel = 0; nChannel < CHANNELS_COLOR; nChannel++ ) {
        switch( m_colorMode ) {
        case choshiresource::COLORMODE_RGB: {
          m_choshiImage.at<cv::Vec6d>( y, x ).val[nChannel] = cluster->m_meanRGB.val[nChannel];
          break;
        }
        case choshiresource::COLORMODE_LAB: {
          m_choshiImage.at<cv::Vec6d>( y, x ).val[nChannel] = cluster->m_meanLAB.val[nChannel];
          break;
        }
        case choshiresource::COLORMODE_HVC: {
          m_choshiImage.at<cv::Vec6d>( y, x ).val[nChannel] = cluster->m_meanHVC.val[nChannel];
          break;
        }
        default:
          break;
        }
      }
    }
  }
  return true;
}

/*-------------------------------------------------------*/

bool ChoResource::copyColorChannelImage( cv::Mat *dstImage ) {
  int height = getImageHeight();
  int width = getImageWidth();
  for( int y = 0; y < height; y++ ) {
    for( int x = 0; x < width; x++ ) {
      int label = m_choshiImage.at<cv::Vec6d>( y, x ).val[5];
      ChoCluster *cluster = getClusterAt( label );
      dstImage->at<cv::Vec3b>( y, x ).val[0] = cluster->m_meanRGB.val[0];
      dstImage->at<cv::Vec3b>( y, x ).val[1] = cluster->m_meanRGB.val[1];
      dstImage->at<cv::Vec3b>( y, x ).val[2] = cluster->m_meanRGB.val[2];
    }
  }
  return true;
}

bool ChoResource::copyRGBAImage(cv::Mat *dstImage) {
  if(CHANNELS_COLORALPHA != dstImage->channels()) {
    // TODO DebugMessage
    return false;
  }
  int height = getImageHeight();
  int width = getImageWidth();
  for( int y = 0; y < height; y++ ) {
    for( int x = 0; x < width; x++ ) {
      int label = m_choshiImage.at<cv::Vec6d>( y, x ).val[5];
      ChoCluster *cluster = getClusterAt( label );
      dstImage->at<cv::Vec4b>( y, x ).val[0] = cluster->m_meanRGB.val[0];
      dstImage->at<cv::Vec4b>( y, x ).val[1] = cluster->m_meanRGB.val[1];
      dstImage->at<cv::Vec4b>( y, x ).val[2] = cluster->m_meanRGB.val[2];
      dstImage->at<cv::Vec4b>(y, x).val[3] = 255;
    }
  }
  return true;
}

/* Copy RGBA Image. This image is masked by label.
 * This function is designed for creating opengl textures
 */
bool ChoResource::copyColorChannelImageMaskedByLabel( cv::Mat *dstImg, int labelNum ) {
  if( 0 == m_maskImage.rows )
    m_maskImage = cv::Mat::zeros( m_choshiImage.size().height, m_choshiImage.size().width, CV_8UC1 );

  if( CHANNELS_COLORALPHA != dstImg->channels() ) {
    // TODO DebugMessage
    return false;
  }
  ChoCluster *cluster = getClusterAt( labelNum );
  if( NULL == cluster ) {
    // TODO DebugMessage
    return false;
  }

  int height = m_choshiImage.rows;
  int width = m_choshiImage.cols;

  for( int y = 0; y < height; y++ ) {
    for( int x = 0; x < width; x++ ) {
      if( ( cluster->m_labelNum != (int)m_choshiImage.at<cv::Vec6d>( y, x ).val[CHANNEL_LABEL] ) &&
          ( 0 == m_maskImage.at<unsigned char>( y, x ) ) ) {
        dstImg->at<cv::Vec4b>( y, x ) = cv::Vec4b( 0, 0, 0, 0 );
      } else {
        cv::Vec3b rgb = cluster->m_meanRGB;
        dstImg->at<cv::Vec4b>( y, x ) = cv::Vec4b( rgb.val[0], rgb.val[1], rgb.val[2], 255 );
        m_maskImage.at<unsigned char>( y, x ) = 255;
      }
    }
  }
  return true;
}



void ChoResource::clearMaskImage() {
  m_maskImage = cv::Mat::zeros( m_choshiImage.size().height, m_choshiImage.size().width, CV_8UC1 );
}

void ChoResource::extractSectionImage( cv::Mat *dstImage, int position, CrossSectionDirection eDir ) {
  int unit;
  switch(eDir) {
  case SECTIONASSIDE_X: {
    unit = getImageHeight() / getClusterSize();
    break;
  }
  case SECTIONASSIDE_Y: {
    unit = getImageWidth() / getClusterSize();
    break;
  }
  default:
    break;
  }

  int height = getImageHeight();
  int width = getImageWidth();

  for( int y = 0; y < height; y++ ) {
    for( int x = 0; x < width; x++ ) {
      int label;
      switch( eDir ) {
      case SECTIONASSIDE_X: {
        label = getLabelAt(x, position);
        break;
      }
      case SECTIONASSIDE_Y: {
        label = getLabelAt(position, y);
        break;
      }
      default: {
        label = 0;
        break;
      }
      }

      int start = label * unit;
      //int end = (label + 1) * unit;
      switch( eDir ) {
      case SECTIONASSIDE_X: {
        if( y < start || 0 == ( y % unit ) )
          dstImage->at<cv::Vec4b>( y, x ) = cv::Vec4b( 0, 0, 0, 0 );
        else if( y / unit < getClusterSize() ) {
          int drawLabel = y / unit;
          ChoCluster *cluster = getClusterAt( drawLabel );
          cv::Vec3b temp = cluster->m_meanRGB;
          dstImage->at<cv::Vec4b>( y, x ) = cv::Vec4b(temp.val[0], temp.val[1], temp.val[2], 255);
        } else
          dstImage->at<cv::Vec4b>( y, x ) = 0;
        break;
      }
      case SECTIONASSIDE_Y: {
        if( x < start || 0 == ( x % unit ) )
          dstImage->at<cv::Vec4b>( y, x ) = cv::Vec4b(0, 0, 0, 0 );
        else if( x / unit < getClusterSize() ) {
          int drawLabel = x / unit;
          ChoCluster *cluster = getClusterAt( drawLabel );
          cv::Vec3b temp = cluster->m_meanRGB;
          dstImage->at<cv::Vec4b>( y, x ) = cv::Vec4b(temp.val[0], temp.val[1], temp.val[2], 255);
        } else
          dstImage->at<cv::Vec4b>( y, x ) = 0;
        break;
      }
      default:
        break;
      }
    }
  }
}

void ChoResource::extractSelectArea(cv::Mat *dstImage, cv::Point point, cv::Vec4b color) {
  if(CHANNELS_COLORALPHA != dstImage->channels()) {
    // TODO DebugMessage
    return;
  }
  int height = getImageHeight();
  int width = getImageWidth();
  int label = getLabelAt(point.x, point.y);

  for(int y = 0; y < height; y++) {
    for(int x = 0; x < width; x++) {
      cv::Vec6d tempVal = m_choshiImage.at<cv::Vec6d>(y, x);
      if(label == tempVal.val[CHANNEL_LABEL]) {
        dstImage->at<cv::Vec4b>(y, x) = color;
      } else {
        dstImage->at<cv::Vec4b>(y, x) = cv::Vec4b(0, 0, 0, 0);
      }
    }
  }
}

/********************************************************************
 * Handling cluster
 *******************************************************************/

void ChoResource::addCluster( ChoCluster *cluster ) {
  m_clusterList.push_back( cluster );
}

/*-------------------------------------------------------*/

bool ChoResource::deleteClusterAt( int labelNum ) {
  ChoCluster *cluster = NULL;
  std::list<ChoCluster *>::iterator it = m_clusterList.begin();
  while( it != m_clusterList.end() ) {
    cluster = *it;
    if( labelNum == cluster->m_labelNum ) {
      delete cluster;
      m_clusterList.erase( it );
      return true;
    }
    it++;
  }
  return false;
}

/*-------------------------------------------------------*/

void ChoResource::exchangeCluster( int label0, int label1 ) {
  ChoCluster *cluster0 = getClusterAt( label0 );
  ChoCluster *cluster1 = getClusterAt( label1 );
  int temp0 = cluster0->m_labelNum;
  int temp1 = cluster1->m_labelNum;
  cluster0->m_labelNum = temp1;
  cluster1->m_labelNum = temp0;
  for( int y = 0; y < m_choshiImage.size().height; y++ ) {
    for( int x = 0; x < m_choshiImage.size().width; x++ ) {
      cv::Vec6d pixelVal = m_choshiImage.at<cv::Vec6d>( y, x );
      if( pixelVal.val[5] == label0 ) {
        pixelVal.val[5] = temp1;
        m_choshiImage.at<cv::Vec6d>( y, x ) = pixelVal;
      } else if( pixelVal.val[5] == label1 ) {
        pixelVal.val[5] = temp0;
        m_choshiImage.at<cv::Vec6d>( y, x ) = pixelVal;
      }
    }
  }
}

/*-------------------------------------------------------*/

void ChoResource::insertCluster( int label0, int label1 ) {
  int dir = 0;
  if( label0 > label1 )
    dir = 1;
  if( 0 == dir ) {
    std::list<ChoCluster *>::iterator it = ChoResource::m_clusterList.begin();
    while( it != ChoResource::m_clusterList.end() ) {
      ChoCluster *cluster = *it;
      if( cluster->m_labelNum <= label1 && cluster->m_labelNum > label0 )
        cluster->m_labelNum--;
      else if( cluster->m_labelNum == label0 )
        cluster->m_labelNum = label1;
      it++;
    }
    for( int y = 0; y < m_choshiImage.size().height; y++ ) {
      for( int x = 0; x < m_choshiImage.size().width; x++ ) {
        cv::Vec6d pixelVal = m_choshiImage.at<cv::Vec6d>( y, x );
        if( pixelVal.val[5] == label0 ) {
          pixelVal.val[5] = label1;
          m_choshiImage.at<cv::Vec6d>( y, x ) = pixelVal;
        } else if( pixelVal.val[5] <= label1 && pixelVal.val[5] > label0 ) {
          pixelVal.val[5]--;
          m_choshiImage.at<cv::Vec6d>( y, x ) = pixelVal;
        }
      }
    }
  } else if( 1 == dir ) {
    std::list<ChoCluster *>::iterator it = ChoResource::m_clusterList.begin();
    while( it != ChoResource::m_clusterList.end() ) {
      ChoCluster *cluster = *it;
      if( cluster->m_labelNum >= label1 && cluster->m_labelNum < label0 )
        cluster->m_labelNum++;
      else if( cluster->m_labelNum == label0 )
        cluster->m_labelNum = label1;
      it++;
    }
    for( int y = 0; y < m_choshiImage.size().height; y++ ) {
      for( int x = 0; x < m_choshiImage.size().width; x++ ) {
        cv::Vec6d pixelVal = m_choshiImage.at<cv::Vec6d>( y, x );
        if( pixelVal.val[5] == label0 ) {
          pixelVal.val[5] = label1;
          m_choshiImage.at<cv::Vec6d>( y, x ) = pixelVal;
        } else if( pixelVal.val[5] >= label1 && pixelVal.val[5] < label0 ) {
          pixelVal.val[5]++;
          m_choshiImage.at<cv::Vec6d>( y, x ) = pixelVal;
        }
      }
    }
  }
}

/*-------------------------------------------------------*/

void ChoResource::sortClusters() {
  m_clusterList.sort( &ChoResource::ascending );
}

void ChoResource::reassignLabelNum() {
  std::list<ChoCluster *>::iterator it = m_clusterList.begin();
  int newLabel = 0;
  int oldLabel = 0;
  int height = getImageHeight();
  int width = getImageWidth();

  while( it != m_clusterList.end() ) {
    ChoCluster *cluster = *it;
    if( newLabel != cluster->m_labelNum ) {
      oldLabel = cluster->m_labelNum;
      cluster->m_labelNum = newLabel;
      cv::Vec6d dbg0; /* debug value */
      cv::Vec6d dbg1;
      cv::Point2i ittPoint;
      for( ittPoint.y = 0; ittPoint.y < height; ittPoint.y++ ) {
        for( ittPoint.x = 0; ittPoint.x < width; ittPoint.x++ ) {
          if( oldLabel == m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_LABEL] ) {
            dbg0 = m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x );
            m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_X] = cluster->m_meanXY[0];
            m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_Y] = cluster->m_meanXY[1];
            switch( m_colorMode ) {
            case COLORMODE_RGB: {
              m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_R] = cluster->m_meanRGB[0];
              m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_G] = cluster->m_meanRGB[1];
              m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_B] = cluster->m_meanRGB[2];
              break;
            }
            case COLORMODE_LAB: {
              m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_R] = cluster->m_meanLAB[0];
              m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_G] = cluster->m_meanLAB[1];
              m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_B] = cluster->m_meanLAB[2];
              break;
            }
            case COLORMODE_HVC: {
              m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_R] = cluster->m_meanHVC[0];
              m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_G] = cluster->m_meanHVC[1];
              m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_B] = cluster->m_meanHVC[2];
              break;
            }
            default:
              break;
            }
            m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x )[CHANNEL_LABEL] = cluster->m_labelNum;
            dbg1 = m_choshiImage.at<cv::Vec6d>( ittPoint.y, ittPoint.x );
          }
        }
      }
    }
    newLabel++;
    it++;
  }
}

/********************************************************************
 * Save/Load
 *******************************************************************/

bool ChoResource::save() {
  /* ofstream can not construct from std::string */
  std::ofstream ofs( m_xmlName.c_str() );
  if( !ofs.is_open() ) {
    // TODO DebugMessage
    return false;
  }

  /* Create cluster infomation */
  ChoCluster *cluster = NULL;
  std::list<ChoCluster *>::iterator it = m_clusterList.begin();
  while( it != m_clusterList.end() ) {
    cluster = *it;
    cluster->createAdjacencyInfo();
    it++;
  }

  /* serialize */
  boost::archive::xml_oarchive oArchive( ofs );
  oArchive << boost::serialization::make_nvp( ROOT_TAG, this );

  /* write choshi image binary data */
  writeMatBin();
  return true;
}

bool ChoResource::load() {
  /* ifstream can not construct from std::string */
  std::ifstream ifs( m_xmlName.c_str() );
  if( !ifs.is_open() ) {
    // TODO DebugMessage
    return false;
  }

  boost::archive::xml_iarchive iArchive( ifs );

  ChoResource tempResource;
  iArchive >> boost::serialization::make_nvp( ROOT_TAG, tempResource );

  /* copy values from temp */
  this->clear();

  std::list<ChoCluster *> *clusterList = tempResource.getClusterList();
  std::list<ChoCluster *>::iterator it = clusterList->begin();
  while( it != clusterList->end() ) {
    ChoCluster *cluster = new ChoCluster( *it );
    addCluster( cluster );
    it++;
  }

  /* make Adjacency */
  it = m_clusterList.begin();
  ChoCluster *srcCluster = NULL;
  ChoCluster *dstCluster = NULL;
  while( it != m_clusterList.end() ) {
    srcCluster = *it;
    std::vector<int> adjacencyNums;
    srcCluster->copyAdjacencyLabelNum( &adjacencyNums );
    for( unsigned int i = 0; i < adjacencyNums.size(); i++ ) {
      dstCluster = getClusterAt( adjacencyNums[i] );
      srcCluster->addAdjacencyCluster( dstCluster );
      dstCluster->addAdjacencyCluster( srcCluster );
    }
    it++;
  }

  /* read choshi image from binary data */
  readMatBin();
  return true;
}

bool ChoResource::writeMatBin() {
  std::ofstream ofs( m_imageName.c_str(), std::ios::binary );
  if( !ofs.is_open() ) {
    // TODO DebugMessage
    return false;
  }
  if( m_choshiImage.empty() ) {
    int data = 0;
    ofs.write( (const char *)( &data ), sizeof( int ) );
    // TODO DebugMessage
    return false;
  }
  int type = m_choshiImage.type();
  ofs.write( (const char *)( &m_choshiImage.rows ), sizeof( int ) );
  ofs.write( (const char *)( &m_choshiImage.cols ), sizeof( int ) );
  ofs.write( (const char *)( &type ), sizeof( int ) );
  ofs.write( (const char *)( m_choshiImage.data ), m_choshiImage.elemSize() * m_choshiImage.total() );
  return true;
}

bool ChoResource::readMatBin() {
  std::ifstream ifs( m_imageName.c_str(), std::ios::binary );
  if( !ifs.is_open() ) {
    // TODO DebugMessage
    return false;
  }
  int rows = 0;
  int cols = 0;
  int type = 0;
  ifs.read( (char *)( &rows ), sizeof( int ) );
  if( 0 == rows ) {
    // TODO DebugMessage
    return false;
  }
  ifs.read( (char *)( &cols ), sizeof( int ) );
  ifs.read( (char *)( &type ), sizeof( int ) );

  m_choshiImage.release();
  m_choshiImage.create( rows, cols, type );
  ifs.read( (char *)( m_choshiImage.data ), m_choshiImage.elemSize() * m_choshiImage.total() );
  return true;
}

/********************************************************************
 * Check data
 *******************************************************************/

void ChoResource::checkConvex( std::vector<int> *isolatedLabels ) {
  std::list<ChoCluster *>::iterator it = m_clusterList.begin();
  while( it != m_clusterList.end() ) {
    ChoCluster *cluster = *it;
    if( cluster->isIsolatedConvex( &this->m_choshiImage ) ) {
      isolatedLabels->push_back( cluster->m_labelNum );
    }
    it++;
  }
}

} /* namespace choshiresource */
