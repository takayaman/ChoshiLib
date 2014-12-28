#include "choglwindowbase.h"
#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#include <QList>

namespace choshigui{

ChoGlWindowBase::ChoGlWindowBase( QWindow *parent )
    : QWindow( parent )
{
    m_updatePending = false;
    m_animating = false;
    m_glContext = NULL;
    m_glPaintDevice = NULL;
    setSurfaceType( QWindow::OpenGLSurface );
}

ChoGlWindowBase::~ChoGlWindowBase()
{
    delete m_glContext;
    delete m_glPaintDevice;
}

void ChoGlWindowBase::initialize()
{}

void ChoGlWindowBase::render( QPainter *painter )
{
    Q_UNUSED( painter );
}

void ChoGlWindowBase::render()
{
    if( !m_glPaintDevice )
        m_glPaintDevice = new QOpenGLPaintDevice();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    m_glPaintDevice->setSize( this->size() );
    QPainter painter( m_glPaintDevice );
    render( &painter );
}

void ChoGlWindowBase::renderLater()
{
    if( !m_updatePending ){
        m_updatePending = true;
        QCoreApplication::postEvent( this, new QEvent( QEvent::UpdateRequest ) );
    }
}

bool ChoGlWindowBase::event( QEvent *event )
{
    switch( event->type() ){
    case QEvent::UpdateRequest:
    {
        renderNow();
        return true;
    }
    default:
        return QWindow::event( event );
    }
}

void ChoGlWindowBase::exposeEvent( QExposeEvent *event )
{
    Q_UNUSED( event );
    if( isExposed() )
        renderNow();
}

void ChoGlWindowBase::resizeEvent( QResizeEvent *event )
{
    Q_UNUSED( event );
    if( isExposed() )
        renderNow();
}

void ChoGlWindowBase::renderNow()
{
    if( !isExposed() )
        return;
    m_updatePending = false;
    bool needsInitialize = false;

    if( !m_glContext ){
        m_glContext = new QOpenGLContext( this );
        QSurfaceFormat format;
        /* opengl device setting
         * if do not set depth, alpha, etc..
         * cant not use depth test, alpha test
         */
        format.setDepthBufferSize( 8 );
        format.setAlphaBufferSize( 8 );
        m_glContext->setFormat( format );
        m_glContext->create();

        /* Debug */
        format = m_glContext->format();
        int red = format.redBufferSize();
        int green = format.greenBufferSize();
        int blue = format.blueBufferSize();
        int alpha = format.alphaBufferSize();
        int depth = format.depthBufferSize();
        int stencil = format.stencilBufferSize();
        bool hasalpha = format.hasAlpha();
        int major = format.majorVersion();
        int minor = format.minorVersion();
        int sample = format.samples();
        bool stereo = format.stereo();
        // OpengGL3.0以上ではglGetString()を呼ぶとエラーになる
        //const GLubyte *strExtensions = glGetString( GL_EXTENSIONS );

        needsInitialize = true;
    }
    bool couldRender = m_glContext->makeCurrent(this);
    if(!couldRender)
        return;
    if( needsInitialize ){
        initializeOpenGLFunctions();
        initialize();
    }

    render();

    m_glContext->swapBuffers( this );
    if( m_animating )
        renderLater();
}

void ChoGlWindowBase::setAnimating( bool animating )
{
    m_animating = animating;
    if( animating )
        renderLater();
}

} /* namespace choshigui */
