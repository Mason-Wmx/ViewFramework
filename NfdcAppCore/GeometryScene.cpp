#include "stdafx.h"

#include "GeometryScene.h"
#include "View3D.h"

using namespace SIM;

GeometryScene::GeometryScene ( QWidget* parent )
: QGraphicsScene ( parent )
{
}

//-----------------------------------------------------------------------------

GeometryScene::~GeometryScene ()
{
}

//-----------------------------------------------------------------------------

void GeometryScene::mousePressEvent ( QGraphicsSceneMouseEvent* event )
{
	QGraphicsScene::mousePressEvent ( event ) ;
	mouseEvent ( QEvent::MouseButtonPress, event ) ;
}

//-----------------------------------------------------------------------------

void GeometryScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event )
{
	QGraphicsScene::mouseReleaseEvent ( event ) ;
	mouseEvent ( QEvent::MouseButtonRelease, event ) ;
}

//-----------------------------------------------------------------------------

void GeometryScene::mouseMoveEvent ( QGraphicsSceneMouseEvent* event )
{
	QGraphicsScene::mouseMoveEvent ( event ) ;
	mouseEvent ( QEvent::MouseMove, event ) ;
}

//-----------------------------------------------------------------------------

void GeometryScene::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent* event ) 
{
	QGraphicsScene::mouseDoubleClickEvent ( event ) ;
	mouseEvent ( QEvent::MouseButtonDblClick, event ) ;
}

//-----------------------------------------------------------------------------

void GeometryScene::wheelEvent ( QGraphicsSceneWheelEvent* event )
{
	QGraphicsScene::wheelEvent ( event );
	if ( event -> isAccepted () )
		return ;

	auto parent = view () ;
	if ( !parent )
		return ;

	QWheelEvent wheelEvent ( parent -> mapFromGlobal ( event -> screenPos () ), 
							 event -> scenePos () . toPoint (), 
							 event -> delta (), 
							 event -> buttons (), 
							 event -> modifiers (), 
							 event -> orientation () ) ;
	parent -> interactorEvent ( &wheelEvent ) ;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void GeometryScene::contextMenuEvent ( QGraphicsSceneContextMenuEvent* event )
{
	QGraphicsScene::contextMenuEvent ( event );
	if ( event -> isAccepted () )
		return ;

	auto parent = view () ;
	if ( !parent )
		return ;

	QContextMenuEvent contextEvent ( ( QContextMenuEvent::Reason ) event -> reason (),
										parent -> mapFromGlobal ( event -> screenPos () ), 
										event -> screenPos () ) ;
	parent -> interactorEvent ( &contextEvent ) ;
}

//-----------------------------------------------------------------------------

void GeometryScene::focusInEvent ( QFocusEvent* event ) 
{
	QGraphicsScene::focusInEvent ( event );
	if ( event -> isAccepted () )
		return ;

	auto parent = view () ;
	if ( !parent )
		return ;

	parent -> interactorEvent ( event ) ;
}

//-----------------------------------------------------------------------------

void GeometryScene::focusOutEvent ( QFocusEvent* event )
{
	QGraphicsScene::focusOutEvent ( event );
	if ( event -> isAccepted () )
		return ;

	auto parent = view () ;
	if ( !parent )
		return ;

	parent -> interactorEvent ( event ) ;
}

//-----------------------------------------------------------------------------

void GeometryScene::keyPressEvent ( QKeyEvent* event )
{
	QGraphicsScene::keyPressEvent ( event );
	if ( event -> isAccepted () )
		return ;

	auto parent = view () ;
	if ( !parent )
		return ;

	parent -> interactorEvent ( event ) ;
}

//-----------------------------------------------------------------------------

void GeometryScene::keyReleaseEvent ( QKeyEvent* event ) 
{
	QGraphicsScene::keyReleaseEvent ( event );
	if ( event -> isAccepted () )
		return ;

	auto parent = view () ;
	if ( !parent )
		return ;

	parent -> interactorEvent ( event ) ;
}

//-----------------------------------------------------------------------------

View3D* GeometryScene::view () 
{
	return dynamic_cast<View3D*> ( parent () ) ;
}

//-----------------------------------------------------------------------------

void GeometryScene::mouseEvent ( QEvent::Type type, QGraphicsSceneMouseEvent* event )
{
	if ( event -> isAccepted () )
		return ;

	auto parent = view () ;
	if ( !parent )
		return ;

	QMouseEvent mouseEvent ( type, 
								parent -> mapFromGlobal ( event -> screenPos () ), 
								event -> button (), 
								event -> buttons (), 
								event -> modifiers () ) ;
	parent -> interactorEvent ( &mouseEvent ) ;
}

//-----------------------------------------------------------------------------