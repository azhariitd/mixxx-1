#include "glslwaveformwidget.h"

#include <QPainter>

#include "waveform/renderers/waveformwidgetrenderer.h"
#include "waveform/renderers/waveformrenderbackground.h"
#include "waveform/renderers/glslwaveformrenderersignal.h"
#include "waveform/renderers/waveformrendermark.h"
#include "waveform/renderers/waveformrendermarkrange.h"
#include "waveform/renderers/waveformrendererendoftrack.h"
#include "waveform/renderers/waveformrenderbeat.h"

GLSLWaveformWidget::GLSLWaveformWidget( const char* group, QWidget* parent) :
    WaveformWidgetAbstract(group),
    QGLWidget(parent) {

    addRenderer<WaveformRenderBackground>();
    addRenderer<WaveformRendererEndOfTrack>();
    addRenderer<WaveformRenderMarkRange>();
    addRenderer<WaveformRenderMark>();
    addRenderer<GLSLWaveformRendererSignal>();
    addRenderer<WaveformRenderBeat>();

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);

    setAutoBufferSwap(false);

    makeCurrent();
    init();
}

GLSLWaveformWidget::~GLSLWaveformWidget(){
    makeCurrent();
}

void GLSLWaveformWidget::castToQWidget() {
    m_widget = static_cast<QWidget*>(static_cast<QGLWidget*>(this));
}

void GLSLWaveformWidget::paintEvent( QPaintEvent* event) {
    makeCurrent();
    QPainter painter(this);
    draw(&painter,event);
    QGLWidget::swapBuffers();
}
