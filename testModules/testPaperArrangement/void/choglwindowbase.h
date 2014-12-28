#ifndef CHOGLWINDOWBASE_H
#define CHOGLWINDOWBASE_H

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>

class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;

namespace choshigui {

class ChoGlWindowBase : public QWindow, protected QOpenGLFunctions {
  Q_OBJECT
 public:
  ChoGlWindowBase( QWindow *parent = 0 );
  ~ChoGlWindowBase();

  virtual void render( QPainter *painter );
  virtual void render();
  virtual void initialize();

  void setAnimating( bool animating );

 public slots:
  void renderLater();
  void renderNow();

 signals:
  void clicked();

 protected:
  bool event( QEvent *event );
  void exposeEvent( QExposeEvent *event );
  void resizeEvent( QResizeEvent *event );

 private:
  bool m_updatePending;
  bool m_animating;
  QOpenGLContext *m_glContext;
  QOpenGLPaintDevice *m_glPaintDevice;

}; /* class ChoGlWindowBase */

} /* namespace choshigui */

#endif // CHOGLWINDOWBASE_H
