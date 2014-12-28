#include "choglwindowbase.h"
#include <QtCore/QCoreApplication>


namespace choshigui {

ChoGlWindowBase::ChoGlWindowBase( QWidget *parent )
  : QMdiSubWindow( parent ) {
  m_updatePending = false;
  m_animating = false;
  m_glContext = 0;
  m_glPaintDevice = 0;
}

ChoGlWindowBase::~ChoGlWindowBase() {
  delete m_glPaintDevice;
}

void ChoGlWindowBase::initialize() {

}

void ChoGlWindowBase::render( QPainter *painter ) {
  Q_UNUSED( painter );
}

void ChoGlWindowBase::render() {
  if( !m_glPaintDevice )
    m_glPaintDevice = new QOpenGLPaintDevice;
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
  m_glPaintDevice->setSize( this->size() );
  QPainter painter( m_glPaintDevice );
  render( &painter );
}

void ChoGlWindowBase::renderLater() {
  if( !m_updatePending ) {
    m_updatePending = true;
    QCoreApplication::postEvent( this, new QEvent( QEvent::UpdateRequest ) );
  }
}

bool ChoGlWindowBase::event( QEvent *event ) {
  switch( event->type() ) {
  case QEvent::UpdateRequest: {
    renderNow();
    return true;
  }
  default: {
    return QMdiSubWindow::event( event );
  }
  }
}

void ChoGlWindowBase::resizeEvent( QResizeEvent *resizeEvent ) {
  Q_UNUSED( event );
  if( is )
  }

} /* namespace choshigui */
