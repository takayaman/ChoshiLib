/*=============================================================================
 * Project : ChoResource
 * Code : choresource.h
 * Written : N.Takayama, UEC
 * Date : 2014/11/29
 * Copyright (c) 2014 N.Takayama <takayaman@uec.ac.jp>
 * This class holds clusters and image data for create Choshi.
 *===========================================================================*/

#ifndef CHORESOURCE_H
#define CHORESOURCE_H


#include <opencv2/opencv.hpp>
#include <list>
#include <string>
#include <vector>

#include "./choresource_global.h"
#include "./chocluster.h"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/list.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

namespace choshiresource {

#define CHANNELS_COLOR 3
#define CHANNELS_XYLABEL 3
#define CHANNELS_COLORALPHA 4

/** Definition of each channel for multi-dimentional image.
 */
typedef enum ChoshiImageChannel_TAG {
  CHANNEL_R = 0,                        /**< Red */
  CHANNEL_G,                            /**< Green */
  CHANNEL_B,                            /**< Blue */
  CHANNEL_X,                            /**< Center of cluster X */
  CHANNEL_Y,                            /**< Center of cluster Y */
  CHANNEL_LABEL,                        /**< Index of region */
  CHANNEL_NUM
} ChoshiImageChannel;

/** Definition of color mode which used in image processing.
 */
typedef enum ChoshiColorMode_TAG {
  COLORMODE_RGB = 0,                    /**< RGM */
  COLORMODE_LAB,                        /**< LAB */
  COLORMODE_LAB10,                      /**< LAB under XYZ10 */
  COLORMODE_HVC,                        /**< HVC(MTM) */
  COLORMODE_HVC10,                      /**< HVC(MTM) under XYZ10 */
  COLORMODE_NUM
} ChoshiColorMode;

/** Definition of direction to generate cross section image of Choshi.
 */
typedef enum ChoshiCrossSectionDirection_TAG {
  SECTIONASSIDE_X = 0,                  /**< Generate asside X */
  SECTIONASSIDE_Y,                      /**< Generate asside Y */
  NUM
} CrossSectionDirection;

/** This class holds clusters and image data for Choshi.
 */
class CHORESOURCESHARED_EXPORT ChoResource {
 public:
  /** Defoult Constructor
   * @return None
   */
  ChoResource(void);

  /** Default Destructor
   * @return None
   */
  ~ChoResource(void);

  /** Clear Resource
   * @return None
   */
  void clear(void);

  /*--- Accessor -------------------------------------------------------------*/
  /** 
   * @return Image width.
   */
  int getImageWidth(void);

  /** 
   * @return Image height.
   */
  int getImageHeight(void);

  /** 
   * @return The number of color channels.
   */
  int getColorChannelNum(void);

  /** 
   * @param x Coordinate x of point.
   * @param y Coordinate y of point.
   * @return Index of label(Region number) at (x, y).
   */
  int getLabelAt(int x, int y);

  /** 
   * @param path Path to save resource datas.
   */
  void setResourcePath(std::string path);

  /** 
   * @return The pointer of cluster lists.
   */
  std::list<ChoCluster *> *getClusterList(void);

  /** 
   * @param labelNum Index of label(Region number).
   * @return Pointer of cluster which is numbered as labelNum.
   */
  ChoCluster *getClusterAt(int labelNum);

  /** 
   * @return Pointer of Choshi image.
   */
  cv::Mat *getChoshiImage(void);

  /** 
   * @return Color mode which is used in image processing.
   */
  ChoshiColorMode getColorMode(void);

  /** 
   * @param eMode Color mode.
   */
  void setColorMode(ChoshiColorMode eMode);

  /** 
   * @return Cluster size
   */
  int getClusterSize(void);

  /*--- Methods --------------------------------------------------------------*/
  /* handling image */

  /** 
   * @param srcImg Source image.
   * @return 
   */
  cv::Mat *createChoshiImage(const cv::Mat *srcImg);

  /** 
   * @return true : success, false : fail.
   */
  bool reflectClusterValToChoshiImage(void);

  /** 
   * @param dstImage Destination to copy.
   * @return true : success, false : fail.
   */
  bool copyColorChannelImage(cv::Mat *dstImage);

  /** 
   * @param dstImage Destination to copy.
   * @return true : success, false : fail.
   */
  bool copyRGBAImage(cv::Mat *dstImage);

  /** 
   * @param dstImg Destination to copy.
   * @param labelNum Index of a mask region.
   * @return true : success, false : fail.
   */
  bool copyColorChannelImageMaskedByLabel(cv::Mat *dstImg, int labelNum);

  /** 
   * @param dstImage Destination to copy.
   * @param position Position to generate cross section image.
   * @param eDir Direction to generate cross section image.
   * This method generate cross section image of position aside eDir.
   * For example, position is (50, 50), and eDir is X,
   * Generate cross section of from (0, 50) to (imagewidth, 50).
   */
  void extractSectionImage(cv::Mat *dstImage,
                           int position, CrossSectionDirection eDir);

  /** Clear mask image which is used in copyColorChannelImageMaskedByLabel()
   * TODO adjust or move this function.
   */
  void clearMaskImage(void);

  /** 
   * @param dstImage Destination to copy.
   * @param point Position of extract region.
   * @param Vec4b Color for displaying select area.
   */
  void extractSelectArea(cv::Mat *dstImage,
                         cv::Point point, cv::Vec4b color = cv::Vec4b(255, 0, 0, 255));

  /* handling cluster */

  /** 
   * @param cluster Pointer of cluster to add.
   */
  void addCluster(ChoCluster *cluster);

  /** 
   * @param labelNum The number of label to delete.
   * @return true : success, false : fail.
   */
  bool deleteClusterAt(int labelNum);

  /** 
   * @param label0 Index of cluster.
   * @param label1 Index of cluster.
   */
  void exchangeCluster(int label0, int label1);

  /** 
   * @param label0 
   * @param label1 
   */
  void insertCluster(int label0, int label1);

  /** Sort cluster based on ascending()
   */
  void sortClusters(void);

  /** Reassign labels to clusters.
   */
  void reassignLabelNum(void);
  /* sort algorithm */
  /** 
   * @param val1 Left hand side.
   * @param val2 Right hand side.
   * @return 
   */
  /*
  static bool ascending(const ChoCluster& val1, const ChoCluster& val2) {
    if (val1.m_labelNum < val2.m_labelNum) {
      return true;
    } else {
      return false;
    }
  }
  */

  /*--- save/load ------------------------------------------------------------*/
  /** Save resource data to XML.
   * @return true : success, false : fail.
   */
  bool save(void);

  /** Load resource data from XML
   * @return true : success, false : fail.
   */
  bool load(void);

  /* quote form
   * https://github.com/takmin/Binary/CvMat/blob/master/BinaryCvMat.cpp
   */
  /** Write Choshi image as binary.
   * @return true : success, false : fail.
   */
  bool writeMatBin(void);

  /** Read Choshi image from binary.
   * @return true : success, false : fail.
   */
  bool readMatBin(void);

  /* check data */
  /** 
   * @param isolatedLabels Indices of isolated regions.
   */
  void checkConvex(std::vector<int> *isolatedLabels);

 private:

  /* serialize */
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive &archive, unsigned int version) {
    static_cast<void>(version);
    archive &boost::serialization::make_nvp("Path", m_resourcePath);
    archive &boost::serialization::make_nvp("ImgName", m_imageName);
    archive &boost::serialization::make_nvp("XmlName", m_xmlName);
    archive &boost::serialization::make_nvp("ColorMode", m_colorMode);
    archive &boost::serialization::make_nvp("ClusterList", m_clusterList);
  }

 private:
  std::string m_resourcePath;           /**< Resource path to save/load */
  std::string m_imageName;              /**< Image name to serialize. */
  std::string m_xmlName;                /**< Resource file name to save/load */

  std::list<ChoCluster *> m_clusterList; /**< List of clusters. */

  ChoshiColorMode m_colorMode;          /**< Color mode in used. */

  cv::Mat m_choshiImage;                /**< Choshi image data. */
  cv::Mat m_maskImage;                  /**< Mask image for generate partial image. */
}; /* class ChoResource */

} /* namespace choshiresource */

#endif // CHORESOURCE_H
