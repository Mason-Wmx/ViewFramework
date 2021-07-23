#pragma once

#include <QGraphicsScene>
#include <QEvent>
#include <QWidget>

namespace SIM
{
    class View3D;

    class GeometryScene : public QGraphicsScene
    {
        Q_OBJECT

    public:

        GeometryScene(QWidget* parent);
        virtual ~GeometryScene();

        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

        void wheelEvent(QGraphicsSceneWheelEvent* event) override;

        void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
        void focusInEvent(QFocusEvent* event) override;
        void focusOutEvent(QFocusEvent* event) override;

        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;

    private:
        View3D* view();
        void mouseEvent(QEvent::Type type, QGraphicsSceneMouseEvent* event);

    };
}

