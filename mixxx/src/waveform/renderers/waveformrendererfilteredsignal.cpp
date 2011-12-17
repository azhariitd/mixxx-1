#include "waveformrendererfilteredsignal.h"

#include "waveformwidgetrenderer.h"
#include "waveform/waveform.h"

#include "widget/wskincolor.h"
#include "trackinfoobject.h"
#include "widget/wwidget.h"

#include "defs.h"

WaveformRendererFilteredSignal::WaveformRendererFilteredSignal(
        WaveformWidgetRenderer* waveformWidgetRenderer)
    : WaveformRendererAbstract( waveformWidgetRenderer) {
}

WaveformRendererFilteredSignal::~WaveformRendererFilteredSignal() {
}

void WaveformRendererFilteredSignal::init() {
}

void WaveformRendererFilteredSignal::onResize() {
    qDebug() << "WaveformRendererFilteredSignal::onResize";
    m_lowLines.resize(m_waveformRenderer->getWidth());
    m_midLines.resize(m_waveformRenderer->getWidth());
    m_highLines.resize(m_waveformRenderer->getWidth());
}

void WaveformRendererFilteredSignal::setup(const QDomNode& node) {
    m_signalColor.setNamedColor(
                WWidget::selectNodeQString(node, "SignalColor"));
    m_signalColor = WSkinColor::getCorrectColor(m_signalColor);

    // TODO(vRince): fetch color from skin
    int h, s, l;
    m_signalColor.getHsl(&h, &s, &l);
    m_lowColor = QColor::fromHsl(h, s, 50, 128);
    m_midColor = QColor::fromHsl(h-2, s, 100, 128);
    m_highColor =  QColor::fromHsl(h+2, s, 200, 128);
}

void WaveformRendererFilteredSignal::draw(QPainter* painter,
                                          QPaintEvent* /*event*/) {
    const TrackInfoObject* trackInfo = m_waveformRenderer->getTrackInfo().data();

    if (!trackInfo) {
        return;
    }

    const Waveform* waveform = trackInfo->getWaveForm();

    int samplesPerPixel = m_waveformRenderer->getZoomFactor();
    //samplesPerPixel = math_min(2, samplesPerPixel);
    int numberOfSamples = m_waveformRenderer->getWidth() * samplesPerPixel;

    int currentPosition = 0;
    if( m_waveformRenderer->getPlayPos() >= 0)
    {
        //TODO (vRince) not really accurate since waveform size une visual reasampling and
        //have two mores samples to hold the complete visual data
        currentPosition = m_waveformRenderer->getPlayPos()*waveform->size();
        m_waveformRenderer->regulateVisualSample(currentPosition);
    }

    painter->save();

    painter->setWorldMatrixEnabled(false);

    const float halfHeight = m_waveformRenderer->getHeight()/2.0;
    const float heightFactor = halfHeight/255.0;

    for (int i = 0; i < numberOfSamples; i += 2*samplesPerPixel) {
        const int xPos = i/samplesPerPixel;
        const int visualIndex = currentPosition + 2*i - numberOfSamples;
        if (visualIndex >= 0 && (visualIndex+1) < waveform->size()) {
            unsigned char maxLow[2] = {0, 0};
            unsigned char maxMid[2] = {0, 0};
            unsigned char maxHigh[2] = {0, 0};

            for (int subIndex = 0; subIndex < 2*samplesPerPixel; ++subIndex) {
                maxLow[0] = math_max( maxLow[0], waveform->getLow(visualIndex+subIndex));
                maxLow[1] = math_max( maxLow[1], waveform->getLow(visualIndex+subIndex+1));
                maxMid[0] = math_max( maxMid[0], waveform->getMid(visualIndex+subIndex));
                maxMid[1] = math_max( maxMid[1], waveform->getMid(visualIndex+subIndex+1));
                maxHigh[0] = math_max( maxHigh[0], waveform->getHigh(visualIndex+subIndex));
                maxHigh[1] = math_max( maxHigh[1], waveform->getHigh(visualIndex+subIndex+1));
            }

            m_lowLines[xPos].setLine(xPos, (int)(halfHeight-heightFactor*(float)maxLow[0]),
                                     xPos, (int)(halfHeight+heightFactor*(float)maxLow[1]));
            m_midLines[xPos].setLine(xPos, (int)(halfHeight-heightFactor*(float)maxMid[0]),
                                     xPos, (int)(halfHeight+heightFactor*(float)maxMid[1]));
            m_highLines[xPos].setLine(xPos, (int)(halfHeight-heightFactor*(float)maxHigh[0]),
                                      xPos, (int)(halfHeight+heightFactor*(float)maxHigh[1]));
        } else {
            m_lowLines[xPos].setLine(xPos, 0, xPos, 0);
            m_midLines[xPos].setLine(xPos, 0, xPos, 0);
            m_highLines[xPos].setLine(xPos, 0, xPos, 0);
        }
    }

    painter->setPen(QPen(QBrush(m_lowColor), 2));
    painter->drawLines(&m_lowLines[0], m_lowLines.size());
    painter->setPen(QPen(QBrush(m_midColor), 2));
    painter->drawLines(&m_midLines[0], m_midLines.size());
    painter->setPen(QPen(QBrush(m_highColor), 2));
    painter->drawLines(&m_highLines[0], m_highLines.size());

    painter->restore();
}
