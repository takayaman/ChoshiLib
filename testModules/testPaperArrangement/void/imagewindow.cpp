#include "imagewindow.h"
#include "choguibasedef.h"
#include <QMouseEvent>

namespace choshigui {

ImageWindow::ImageWindow() {
  setInitValue();
}

ImageWindow::~ImageWindow() {
  m_dispImage.release();
  m_overlayImage.release();
  glDeleteTextures( 2, m_dispTextures );
}

ImageWindow::ImageWindow( QWindow *parent ) {
  setInitValue();
  this->setParent( parent );
  QSurfaceFormat format;
  format.setSamples( INITSAMPLESIZE );
  this->setFormat( format );
  this->resize( INITWINDOWWIDTH, INITWINDOWHEIGHT );
}

ImageWindow::ImageWindow( QWindow *parent, cv::Mat *image ) {
  setInitValue();
  this->setParent( parent );
  QSurfaceFormat format;
  format.setSamples( INITSAMPLESIZE );
  this->setFormat( format );
  this->resize( INITWINDOWWIDTH, INITWINDOWHEIGHT );
  m_dispImage = image->clone();
}

void ImageWindow::setInitValue() {
  m_posAttr = 0;
  m_colAttr = 0;
  m_matrixUniform = 0;
  m_dispTextures[0] = 0;
  m_dispTextures[1] = 0;
  m_dispImage.release();
  m_program = NULL;
  m_frameCount = 0;
  m_textureLoaded = false;
  m_pushedButton = Qt::NoButton;
  m_pushedPoints.clear();
  m_updateRequest = false;
  m_captureCount = 0;
}

void ImageWindow::loadTexture( cv::Mat *image, ChoshiImgWndTexId eTextureId ) {
  if( !m_dispImage.empty() )
    m_dispImage.release();
  m_dispImage = image->clone();
  // debug
  cv::imwrite("./test.png", m_dispImage);

  m_textureLoaded = true;
  m_updateRequest = true;
  /* mapping texture */
  event(new QEvent(QEvent::UpdateRequest));

  /* eventでm_glContext->makeCurrentを呼び出してからでないと
   * 違うウィンドウに書き込まれる
   */
  cv::flip(m_dispImage, m_dispImage, CHOCVFLIPHORIZONTAL);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  if(ID_IMAGE == eTextureId)
    glBindTexture(GL_TEXTURE_2D, m_dispTextures[0]);
  else if(ID_OVERLAY == eTextureId)
    glBindTexture(GL_TEXTURE_2D, m_dispTextures[1]);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_dispImage.size().width, m_dispImage.size().height, GL_BGRA, GL_UNSIGNED_BYTE, m_dispImage.data);

  event(new QEvent(QEvent::UpdateRequest));
}

GLuint ImageWindow::loadShader( GLenum type, const char *source ) {
  GLuint shader = glCreateShader( type );
  glShaderSource( shader, 1, &source, 0 );
  glCompileShader( shader );
  return shader;
}

void ImageWindow::initialize() {
  if( m_dispImage.empty() && !m_textureLoaded ) {
    // TODO DebugMessage
    return;
  }

  // debug
  cv::imwrite("./test.png", m_dispImage);

  // maybe not use
  if( true == m_textureLoaded && true == m_updateRequest )
    glDeleteTextures( 2, m_dispTextures );

  cv::flip( m_dispImage, m_dispImage, CHOCVFLIPHORIZONTAL );
  m_overlayImage = cv::Mat::zeros(m_dispImage.dims, m_dispImage.size, CV_8UC4);

  /* mapping texture */
  glEnable( GL_TEXTURE_2D );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glGenTextures( 2, m_dispTextures );

  // mapping image texture
  glBindTexture( GL_TEXTURE_2D, m_dispTextures[0] );
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_dispImage.size().width, m_dispImage.size().height, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_dispImage.data );
  // mapping overlay texture
  glBindTexture(GL_TEXTURE_2D, m_dispTextures[1]);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_dispImage.size().width, m_dispImage.size().height, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_overlayImage.data );

  resize( m_dispImage.size().width, m_dispImage.size().height );
  m_textureLoaded = true;
  glDisable(GL_TEXTURE_2D);
  //event(new QEvent(QEvent::None));
  event( new QEvent( QEvent::UpdateRequest ) );
}

void ImageWindow::render() {
  /* set viewpoint and aspect */
  glViewport( 0, 0, this->width(), this->height() );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable(GL_ALPHA_TEST);

  glAlphaFunc(GL_GREATER, 0.5);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
  glEnable(GL_BLEND);

  glColor3d( 1.0, 1.0, 1.0 );

  // 不要
  float norm = (float)this->width() / (float)this->height();
  float normX, normY;
  if( norm >= 1.0 ) {
    normX = 1.0 / norm;
    normY = 1.0;
  } else {
    normX = 1.0;
    normY = 1.0 * norm;
  }

  normX = normY = 1.0;

  // 不要
  int width = this->width();
  int height = this->height();
  int top = this->height() * (1.0 - normY) / 2.0;
  int left = this->width() * (1.0 - normX) / 2.0;
  int bottom = this->height() * (1.0 + normY) / 2.0;
  int right = this->width() * (1.0 + normX) / 2.0;
  m_imageArea = QRect(QPoint(left, top), QPoint(right, bottom));



  glEnable( GL_TEXTURE_2D );
  if( m_textureLoaded ) {
    // mapping image texture
    glBindTexture( GL_TEXTURE_2D, m_dispTextures[0] );
    glBegin( GL_QUADS );
    {
      glTexCoord2d(0.0, 1.0);
      glVertex2d(-normX,normY);
      glTexCoord2d(0.0, 0.0);
      glVertex2d(-normX,-normY);
      glTexCoord2d(1.0, 0.0);
      glVertex2d(normX,-normY);
      glTexCoord2d(1.0, 1.0);
      glVertex2d(normX,normY);
    }
    glEnd();
    // mappint overlay texture
    glBindTexture(GL_TEXTURE_2D, m_dispTextures[1]);
    glBegin( GL_QUADS );
    {
      glTexCoord2d(0.0, 1.0);
      glVertex2d(-normX,normY);
      glTexCoord2d(0.0, 0.0);
      glVertex2d(-normX,-normY);
      glTexCoord2d(1.0, 0.0);
      glVertex2d(normX,-normY);
      glTexCoord2d(1.0, 1.0);
      glVertex2d(normX,normY);
    }
    glEnd();

  }
  glDisable( GL_TEXTURE_2D );
  glDisable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  glFlush();
}

void ImageWindow::update() {
  m_updateRequest = true;
}

void ImageWindow::capture() {
  QString fileName = tr( DEBUGCAPTURENAME_2D ).arg( m_captureCount );
  cv::Mat saveImage = cv::Mat::zeros( m_dispImage.size().height, m_dispImage.size().width, CV_8UC4 );
  cv::flip( m_dispImage, saveImage, CHOCVFLIPHORIZONTAL );
  cv::imwrite( fileName.toStdString(), saveImage );
  m_captureCount++;
}

void ImageWindow::mousePressEvent( QMouseEvent *event ) {
  Qt::MouseButton button = event->button();
  QPoint pushedPoint = event->pos();
  bool isInImageArea = m_imageArea.contains(event->pos());
  if(!isInImageArea) {
    pushedPoint = QPoint(-1, -1);
  } else {
    pushedPoint = event->pos() - m_imageArea.topLeft();
  }

  if(Qt::RightButton == button)
    emit requestDrawCrossSection( pushedPoint );
  if(Qt::LeftButton == button)
    emit requestSelectAreaImage(pushedPoint);

}

} /* namespace choshigui */
