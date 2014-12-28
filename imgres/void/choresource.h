/******************************************************************************
 * Project      : ChoResource
 * File         : choresource.h
 * Copyright    : 2013 (c) Natsuki Takayama, UEC. Tokyo
 * The Header file of ChoResource class.
 * this class holds clusters and image data for create Choshi.
 *****************************************************************************/


#ifndef CHORESOURCE_H
#define CHORESOURCE_H

#include "choresource_global.h"

#include <list>
#include <opencv2/opencv.hpp>
#include "chocluster.h"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/list.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

namespace choshiresource {

#define CHANNELS_COLOR 3
#define CHANNELS_XYLABEL 3
#define CHANNELS_COLORALPHA 4

typedef enum ChoshiImageChannel_TAG {
  CHANNEL_R = 0,
  CHANNEL_G,
  CHANNEL_B,
  CHANNEL_X,
  CHANNEL_Y,
  CHANNEL_LABEL,
  CHANNEL_NUM
} ChoshiImageChannel;

typedef enum ChoshiColorMode_TAG {
  COLORMODE_RGB = 0,
  COLORMODE_LAB,
  COLORMODE_LAB10,
  COLORMODE_HVC,
  COLORMODE_HVC10,
  COLORMODE_NUM
} ChoshiColorMode;

typedef enum ChoshiCrossSectionDirection_TAG {
  SECTIONASSIDE_X = 0,
  SECTIONASSIDE_Y,
  NUM
} CrossSectionDirection;

class CHORESOURCESHARED_EXPORT ChoResource {

 public:
  /* Constructor */
  ChoResource();
  /* Destructor */
  ~ChoResource();
  void clear();

  /* Accessor */
  int getImageWidth();
  int getImageHeight();
  int getColorChannelNum();
  int getLabelAt(int x, int y);
  void setResourcePath( std::string path );
  std::list<ChoCluster *> *getClusterList();
  ChoCluster *getClusterAt( int labelNum );
  cv::Mat *getChoshiImage( void );
  ChoshiColorMode getColorMode( void );
  void setColorMode( ChoshiColorMode eMode );
  int getClusterSize( void );

  /* Methods */
  /* handling image */
  cv::Mat *createChoshiImage( const cv::Mat *srcImg );
  bool reflectClusterValToChoshiImage();
  bool copyColorChannelImage( cv::Mat *dstImage );
  bool copyRGBAImage(cv::Mat *dstImage);
  bool copyColorChannelImageMaskedByLabel( cv::Mat *dstImg, int labelNum );
  void extractSectionImage( cv::Mat *dstImage, int position, CrossSectionDirection eDir );
  void clearMaskImage( void ); // TODO adjust or move this function
  void extractSelectArea(cv::Mat *dstImage, cv::Point point, cv::Vec4b color = cv::Vec4b(255, 0, 0, 255));

  /* handling cluster */
  void addCluster( ChoCluster *cluster );
  bool deleteClusterAt( int labelNum );
  void exchangeCluster( int label0, int label1 );
  void insertCluster( int label0, int label1 );
  void sortClusters();
  void reassignLabelNum();
  /* sort algorithm */
  static bool ascending( const ChoCluster& val1, const ChoCluster& val2 ) {
    if( val1.m_labelNum < val2.m_labelNum ) {
      return true;
    } else return false;
  }

  /* save/load */
  bool save();
  bool load();

  /* quote form
   * https://github.com/takmin/Binary/CvMat/blob/master/BinaryCvMat.cpp
   */
  bool writeMatBin();
  bool readMatBin();

  /* check data */
  void checkConvex( std::vector<int> *isolatedLabels );

 private:

  /* serialize */
  friend class boost::serialization::access;
  template<class Archive>
  void serialize( Archive &archive, unsigned int version ) {
    static_cast<void>( version );
    archive &boost::serialization::make_nvp( "Path", m_resourcePath );
    archive &boost::serialization::make_nvp( "ImgName", m_imageName );
    archive &boost::serialization::make_nvp( "XmlName", m_xmlName );
    archive &boost::serialization::make_nvp( "ColorMode", m_colorMode );
    archive &boost::serialization::make_nvp( "ClusterList", m_clusterList );
  }

 private:
  std::string m_resourcePath;
  std::string m_imageName;
  std::string m_xmlName;

  std::list<ChoCluster *> m_clusterList;

  ChoshiColorMode m_colorMode;

  cv::Mat m_choshiImage;
  cv::Mat m_maskImage;

}; /* class ChoResource */

} /* namespace choshiresource */

#endif // CHORESOURCE_H
