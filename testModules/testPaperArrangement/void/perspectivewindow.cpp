#include "perspectivewindow.h"
#include <QMouseEvent>
#include "choguibasedef.h"
#include <GL/glu.h>

namespace choshigui {

PerspectiveWindow::PerspectiveWindow() {
  setInitValue();
}

PerspectiveWindow::~PerspectiveWindow() {
  if( !m_texImage.data )
    m_texImage.release();
  int num = m_textures.size();
  for( int i = 0; i < num; i++ )
    glDeleteTextures( 1, &m_textures[num] );
  m_textures.clear();
  delete m_glLogger;
}

PerspectiveWindow::PerspectiveWindow( QWindow *parent ) {
  setInitValue();
  this->setParent( parent );
  QSurfaceFormat format;
  format.setSamples( INITSAMPLESIZE );
  this->setFormat( format );
  this->resize( INITWINDOWWIDTH, INITWINDOWHEIGHT );
}

PerspectiveWindow::PerspectiveWindow( QWindow *parent, choshiresource::ChoResource *resource ) {
  setInitValue();
  this->setParent( parent );
  QSurfaceFormat format;
  format.setSamples( INITSAMPLESIZE );
  this->setFormat( format );
  this->resize( INITWINDOWWIDTH, INITWINDOWHEIGHT );
  m_resource = resource;
}

void PerspectiveWindow::setInitValue() {
  m_posAttr = 0;
  m_colAttr = 0;
  m_matrixUniform = 0;
  m_textures.clear();
  m_texImage.release();
  m_maskImage.release();
  m_shaderProgram = NULL;
  m_frameCount = 0;
  m_viewXDistance = 0.0;
  m_viewYDistance = 0.0;
  m_viewZDistance = 0.0;
  m_viewTwist = 0.0;
  m_viewElevation = 0.0;
  m_viewAzimuth = 0.0;
  m_lastClickPos = QPoint( 0, 0 );
  m_pushedButton = Qt::NoButton;
  m_resource = NULL;
  m_updateRequest = false;
  m_captureCount = 0;
  m_glLogger = NULL;
}

GLuint PerspectiveWindow::loadShader( GLenum type, const char *source ) {
  GLuint shader = glCreateShader( type );
  glShaderSource( shader, 1, &source, 0 );
  glCompileShader( shader );
  return shader;
}

void PerspectiveWindow::initialize() {
  m_glLogger = new QOpenGLDebugLogger( this );
  connect( m_glLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );
  if( m_glLogger->initialize() ) {
    m_glLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
    m_glLogger->enableMessages();
  }

  /* clear textures */
  if( !m_resource )
    m_resource->clearMaskImage();
  if( !m_texImage.empty() ) {
    while( !m_texImage.empty() ) {
      GLuint texture = m_textures.back();
      glDeleteTextures( 1, &texture );
      m_textures.pop_back();
    }
  }

  /* cleate and mapping textures */
  glEnable( GL_TEXTURE_2D );
  int height = m_resource->getImageHeight();
  int width = m_resource->getImageWidth();

  for( int i = 0; i < m_resource->getClusterSize(); i++ ) {
    /* cleate textures */
    cv::Mat layerImage = cv::Mat( height, width, CV_8UC4 );
    layerImage.zeros( height, width, CV_8UC4 );
    extractImageAtLabel( i, &layerImage );
    cv::flip( layerImage, layerImage, CHOCVFLIPHORIZONTAL );
    //cv::flip(layerImage, layerImage, CHOCVFLIPBOTH);

    // debug
    QString fileName = tr("layer%1.png").arg( i );
    cv::imwrite( fileName.toStdString(), layerImage );

    GLuint texture;
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    m_textures.push_back( texture );

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, layerImage.size().width, layerImage.size().height, 0, GL_BGRA, GL_UNSIGNED_BYTE, layerImage.data );
    layerImage.release();
  }
  glClearColor( 1.0, 0.0, 0.0, 1.0 );
}

void PerspectiveWindow::extractImageAtLabel( int labelNum, cv::Mat *dstLayerImage ) {
  if( !m_resource ) {
    // TODO DebugMessage
    return;
  }

  int height = m_resource->getImageHeight();
  int width = m_resource->getImageWidth();
  cv::Mat* choshiImage = m_resource->getChoshiImage();

  if( !m_maskImage.data )
    m_maskImage = cv::Mat::zeros( height, width, CV_8UC1 );
  for( int y = 0; y < height; y++ ) {
    for( int x = 0; x < width; x++ ) {
      int pixelLabel = (int)choshiImage->at<cv::Vec6d>( y, x ).val[5];
      if( labelNum == pixelLabel || 255 == m_maskImage.at<uchar>( y, x ) ) {
        choshiresource::ChoCluster *cluster = m_resource->getClusterAt( labelNum );
        cv::Vec3b rgb = cluster->m_meanRGB;
        dstLayerImage->at<cv::Vec4b>( y, x ) = cv::Vec4b( rgb.val[0], rgb.val[1], rgb.val[2], 255 );
        m_maskImage.at<uchar>( y, x ) = 255;
        continue;
      }
      dstLayerImage->at<cv::Vec4b>( y, x ) = cv::Vec4b( 0, 0, 0, 0 );
    }
  }
}

void PerspectiveWindow::clearMask() {
  m_maskImage.zeros( m_resource->getImageHeight(), m_resource->getImageWidth(), CV_8UC1 );
}

void PerspectiveWindow::renderStart() {
  glViewport( 0, 0, width(), height() );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(60.0, INITWINDOWWIDTH/INITWINDOWHEIGHT, 0.1, 100000.0 );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  //gluLookAt( 0.0, 0.0, 5.0,
  //                   0.0, 0.0, 0.0,
  //                   0.0, 1.0, 0.0);

  polarView();
  //glPushMatrix();


  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_ALPHA_TEST );

  glDepthFunc( GL_LESS );
  glAlphaFunc( GL_GREATER, 0.5 );

  // glTexEnvi is most important function for alpha mask
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable( GL_BLEND );
  glShadeModel( GL_FLAT );
}

void PerspectiveWindow::render() {
  renderStart();
  //glColor4d( 0.0, 1.0, 0.0, 1.0 );
  float norm = (float)this->width() / (float)this->height();
  float normX, normY;
  if( norm >= 1.0 ) {
    normX = 1.0 / norm;
    normY = 1.0;
  } else {
    normX = 1.0;
    normY = 1.0 * norm;
  }

  //glPushMatrix();
  //polarView();

  //glPushMatrix();
  //polarView();
  /*
      glColor4d( 1.0, 1.0, 1.0, 1.0 );
      glBegin( GL_POLYGON );
      glVertex2d(-normX,normY);
      glVertex2d(-normX,-normY);
      glVertex2d(normX,-normY);
      glVertex2d(normX,normY);
      glEnd();
  */
  // TODO 座標系の回転修正・整理
  glEnable( GL_TEXTURE_2D );
  if( m_textures.size() > 0 ) {
    for( int i = 0; i < (int)m_textures.size(); i++ ) {
      glPushMatrix();
      {
        glTranslatef( 0.0, 0.0, 0.005 * i );
        glBindTexture( GL_TEXTURE_2D, m_textures[i] );
        glColor4d( 1.0, 1.0, 1.0, 1.0 );

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
      glPopMatrix();
    }
  }
  glDisable( GL_TEXTURE_2D );
  //glPopMatrix();
  //glPopMatrix();

  renderEnd();
}

void PerspectiveWindow::renderEnd() {
  //glPopMatrix();
  glEnd();
  glFlush();

  glDisable( GL_ALPHA_TEST );
  glDisable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );
  if( m_updateRequest == true ) {
    initialize();
    m_updateRequest = false;
  }
}

void PerspectiveWindow::polarView() {
  glTranslatef( 0.0, 0.0, -m_viewZDistance );
  glRotatef( -m_viewTwist, 0.0, 0.0, 1.0 );
  glRotatef( -m_viewElevation, 1.0, 0.0, 0.0 );
  glRotatef( -m_viewAzimuth, 0.0, 1.0, 0.0 );
}

void PerspectiveWindow::update() {
  m_updateRequest = true;
}

void PerspectiveWindow::capture() {
  int height = this->frameGeometry().height();
  int width = this->frameGeometry().width();

  /* save front buffer */
  glReadBuffer( GL_FRONT );
  cv::Mat saveImage = cv::Mat::zeros( height, width, CV_8UC3 );
  glReadPixels( 0, 0, height, width, GL_BGR, GL_UNSIGNED_BYTE, saveImage.data );
  cv::flip( saveImage, saveImage, CHOCVFLIPVERTICAL );
  QString saveFile = tr( DEBUGCAPTURENAME_FRONT ).arg( m_captureCount );
  cv::imwrite( saveFile.toStdString(), saveImage );

  /* save back buffer */
  glReadBuffer( GL_BACK );
  glReadPixels( 0, 0, height, width, GL_BGR, GL_UNSIGNED_BYTE, saveImage.data );
  cv::flip( saveImage, saveImage, CHOCVFLIPVERTICAL );
  saveFile = tr( DEBUGCAPTURENAME_FRONT ).arg( m_captureCount );
  cv::imwrite( saveFile.toStdString(), saveImage );

  m_captureCount++;
}

void PerspectiveWindow::mousePressEvent( QMouseEvent *event ) {
  m_lastClickPos = event->pos();
  m_pushedButton = event->button();
}

void PerspectiveWindow::mouseMoveEvent( QMouseEvent *event ) {
  int xMove, yMove;
  QPoint currentPos = event->pos();
  xMove = currentPos.x() - m_lastClickPos.x();
  yMove = currentPos.y() - m_lastClickPos.y();

  switch( m_pushedButton ) {
  case Qt::LeftButton: {
    m_viewAzimuth += (float)xMove / 2.0;
    m_viewElevation += (float)yMove / 2.0;
    break;
  }
  case Qt::RightButton: {
    //if( xMove > yMove )
    //m_viewZDistance += (float)xMove / 40.0;
    //else
    m_viewZDistance += (float)yMove / 40.0;
    break;
  }
  case Qt::MiddleButton: {
    m_viewXDistance += (float)xMove / 40.0;
    m_viewYDistance += (float)yMove / 40.0;
    break;
  }
  default:
    break;
  }
  m_lastClickPos = currentPos;
}

void PerspectiveWindow::mouseReleaseEvent( QMouseEvent *event ) {
  m_pushedButton = Qt::NoButton;
}

void PerspectiveWindow::onMessageLogged( QOpenGLDebugMessage message ) {
  qDebug() << message;
}


} /* namespace choshigui */
