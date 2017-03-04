/**********************************************************
** Copyright (c) 2015 QUIt Coding <info@quitcoding.com>
**
** This software is provided 'as-is', without any express or implied
** warranty.  In no event will the authors be held liable for any damages
** arising from the use of this software.
**
** Permission is granted to anyone to use this software for any purpose,
** including commercial applications, and to alter it and redistribute it
** freely, subject to the following restrictions:
**
** 1. The origin of this software must not be misrepresented; you must not
**    claim that you wrote the original software. If you use this software
**    in a product, an acknowledgment in the product documentation would be
**    appreciated but is not required.
** 2. Altered source versions must be plainly marked as such, and must not be
**    misrepresented as being the original software.
** 3. This notice may not be removed or altered from any source distribution.
**
**********************************************************/

#ifndef QNANOQUICKITEMPAINTER_H
#define QNANOQUICKITEMPAINTER_H

#include <QtGui/QOpenGLFunctions>
#include <QtQuick/QQuickFramebufferObject>
#include <QColor>
#include <QElapsedTimer>
#include <QSharedPointer>
#ifdef QNANO_USE_RENDERNODE
#include <QSGRenderNode>
#endif
#include "qnanopainter.h"
#include "qnanoquickitem.h"

class QQuickWindow;

#ifdef QNANO_USE_RENDERNODE
class QNanoQuickItemPainter : public QSGRenderNode, protected QOpenGLFunctions
#else
class QNanoQuickItemPainter : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions
#endif
{

    struct ItemData {
        double x, y, width, height;
        double devicePixelRatio;
        double rotation;
        double scale;
        double opacity;
        QQuickItem::TransformOrigin transformOrigin;
        bool clip;
        ItemData()
        {
            x = 0.0;
            y = 0.0;
            width = 0.0;
            height = 0.0;
            devicePixelRatio = 1.0;
            rotation = 0.0;
            scale = 1.0;
            opacity = 1.0;
            transformOrigin = QQuickItem::Center;
            clip = false;
        }
    };

public:
    explicit QNanoQuickItemPainter();
    virtual ~QNanoQuickItemPainter();

    QColor fillColor() const;
    inline QNanoPainter *painter() const
    {
        return m_painter.data();
    }
    inline float width() const
    {
        return m_itemData.width;
    }
    inline float height() const
    {
        return m_itemData.height;
    }

protected:

#ifdef QNANO_USE_RENDERNODE
    StateFlags changedStates() const Q_DECL_OVERRIDE
    {
         return BlendState;
    }
#endif

    virtual void paint(QNanoPainter *painter);
    virtual void synchronize(QNanoQuickItem *item);
    virtual void sizeChanged(float width, float height);

private:

    friend class QNanoQuickItem;

    // Re-implemented from QQuickFramebufferObject::Renderer

#ifndef QNANO_USE_RENDERNODE
    // Creates initial FBO.
    // Gets also called whenever item size changes if textureFollowsItemSize = true
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) Q_DECL_OVERRIDE;
#endif

    // Gets called when FBO should be rendered into.
#ifdef QNANO_USE_RENDERNODE
    void render(const RenderState *) Q_DECL_OVERRIDE;
#else
    void render() Q_DECL_OVERRIDE;
#endif

#ifdef QNANO_USE_RENDERNODE
    void synchronizePainter(QNanoQuickItem * item);
#else
    // Gets called as a result of QQuickFramebufferObject::update().
    void synchronize(QQuickFramebufferObject * item) Q_DECL_OVERRIDE;
#endif

    // These are internal
    void initialize();
    void prepaint();
    void postpaint();
    void setViewSize(int width, int height);
    QPointF itemTransformOrigin() const;

    QQuickWindow *m_window;

    QSharedPointer<QNanoPainter> m_painter;
    QColor m_fillColor;
    int m_viewWidth, m_viewHeight;
    ItemData m_itemData;
    bool m_antialiasing;
    bool m_highQualityRendering;
    QNanoQuickItem::PixelAlign m_pixelAlign;
    QNanoQuickItem::PixelAlign m_pixelAlignText;
    bool m_setupDone;
#ifdef QNANO_DEBUG
    void paintDrawDebug();
    NVGdrawDebug m_drawDebug;
    QElapsedTimer m_debugTimer;
    QElapsedTimer m_debugUpdateTimer;
    qint64 m_debugNsElapsed;
    qint64 m_debugCounter;
    QString m_debugMsElapsed;
#endif
};

#endif // QNANOQUICKITEMPAINTER_H
