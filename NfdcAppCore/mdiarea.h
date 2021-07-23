#pragma once

#include "stdafx.h"

#include <QtWidgets>
#include "SubView.h"

class QMdiTabBarEx : public QTabBar
{
	Q_OBJECT;

public:
	QMdiTabBarEx(QWidget* parent, QMdiArea* mdiArea);
	virtual ~QMdiTabBarEx() {};

	void addTab(QWidget* widget);
	int getTab(QWidget* widget);
	QWidget* getWidget(int tab);
};

class QMdiAreaEx : public QMdiArea
{
	Q_OBJECT

public:
	QMdiAreaEx(QWidget* parent);
	virtual ~QMdiAreaEx() {};

	QTabBar* tabBar();

	void addWindow(QMdiSubWindow* window);
	void update(QMdiSubWindow* window);

protected:
	void paintEvent(QPaintEvent *e);

private:
	QMdiTabBarEx* mTabBar;
};

class QMdiSubWindowEx : public QMdiSubWindow, public SIM::SubView
{
	Q_OBJECT

public:
	QMdiSubWindowEx(QWidget* parent, QWidget* w, SIM::Document& document);
	virtual ~QMdiSubWindowEx() {};

	SIM::Document& GetDocument() const { return _document; }
protected:
	void mouseMoveEvent(QMouseEvent* e);
	virtual void closeEvent(QCloseEvent* ev);

private slots:
	void handleWindowStateChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

private:
	SIM::Document& _document;
};