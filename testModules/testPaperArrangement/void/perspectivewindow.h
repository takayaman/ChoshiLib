#ifndef PERSPECTIVEWINDOW_H
#define PERSPECTIVEWINDOW_H

#include "choglwindowbase.h"
#include "choresource.h"
#include "QtGui/QOpenGLShaderProgram"
#include "QtGui/QOpenGLDebugLogger"
#include <opencv2/opencv.hpp>

namespace choshigui {

class PerspectiveWindow : public ChoGlWindowBase {
 public:
  PerspectiveWindow();
  PerspectiveWindow( QWindow *parent );
  PerspectiveWindow( QWindow *parent, choshiresource::ChoResource *resource );
  ~PerspectiveWindow();

  void initialize(); /* Virtual */
  void render(); /* Virtual */

  void update();
  void capture();

 private:
  void setInitValue( void );
  GLuint loadShader( GLenum type, const char *source );
  void renderStart();
  void renderEnd();
  void polarView();

  void mousePressEvent( QMouseEvent *event );
  void mouseMoveEvent( QMouseEvent *event );
  void mouseReleaseEvent( QMouseEvent *event );

  void clearMask( void );
  void extractImageAtLabel( int labelNum, cv::Mat *dstLayerImage );

 private slots:
  void onMessageLogged( QOpenGLDebugMessage message );

 private:
  GLuint m_posAttr;
  GLuint m_colAttr;
  GLuint m_matrixUniform;

  std::vector<GLuint> m_textures;
  cv::Mat m_texImage;
  cv::Mat m_maskImage;

  QOpenGLShaderProgram *m_shaderProgram;

  int m_frameCount;

  float m_viewXDistance;
  float m_viewYDistance;
  float m_viewZDistance;
  float m_viewTwist;
  float m_viewElevation;
  float m_viewAzimuth;

  QPoint m_lastClickPos;
  Qt::MouseButton m_pushedButton;

  choshiresource::ChoResource *m_resource;

  bool m_updateRequest;

  int m_captureCount;

  QOpenGLDebugLogger *m_glLogger;

}; /* class PerspactiveWindow */

} /* choshigui */

#endif // PERSPECTIVEWINDOW_H
