#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include "choglwindowbase.h"
#include "choresource.h"
#include "QtGui/QOpenGLShaderProgram"
#include <opencv2/opencv.hpp>

namespace choshigui {

typedef enum ChoshiImgWndTexId_TAG {
  ID_IMAGE = 0,
  ID_OVERLAY,
  ID_NUM
} ChoshiImgWndTexId;

class ImageWindow : public ChoGlWindowBase {
  Q_OBJECT
 public:
  ImageWindow();
  ImageWindow( QWindow *parent );
  ImageWindow( QWindow *parent, cv::Mat *image );
  ~ImageWindow();

  void initialize(); /* Virtual */
  void render(); /* Virtual */

  void loadTexture( cv::Mat* image, ChoshiImgWndTexId eTexureId );
  void update();
  void capture();

 private:
  void setInitValue( void );
  GLuint loadShader( GLenum type, const char *source );
  void mousePressEvent( QMouseEvent *event );

 signals:
  void requestDrawCrossSection( QPoint pushedPoint );
  void requestSelectAreaImage(QPoint pushedPoint);

 private:
  GLuint m_posAttr;
  GLuint m_colAttr;
  GLuint m_matrixUniform;

  GLuint m_dispTextures[2]; /* 0: image, 1: overlay */
  cv::Mat m_dispImage;
  cv::Mat m_overlayImage;

  QOpenGLShaderProgram *m_program;

  int m_frameCount;
  bool m_textureLoaded;

  Qt::MouseButton m_pushedButton;
  std::vector<QPoint> m_pushedPoints;

  bool m_updateRequest;
  int m_captureCount;

  QRect m_imageArea;

}; /* class ImageWindow */

} /* namespace choshigui */

#endif // IMAGEWINDOW_H
