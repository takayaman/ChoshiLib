#ifndef CHOGLWINDOWBASE_H
#define CHOGLWINDOWBASE_H

#include <QMdiSubWindow>
#include <QtGui/QOpenGLFunctions>
#include <QOpenGLPaintDevice>

namespace choshigui {

class ChoGlWindowBase : public QMdiSubWindow, protected QOpenGLFunctions {
  Q_OBJECT
 public:
  explicit ChoGlWindowBase( QWindow *parent = 0 );
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
  void resizeEvent( QResizeEvent *resizeEvent );

 private:
  bool m_updatePending;
  bool m_animating;

  QOpenGLContext *m_glContext;
  QOpenGLPaintDevice *m_glPaintDevice;

}; /* class ChoGlWindowBase */

} /* choshigui */

#endif // CHOGLWINDOWBASE_H
