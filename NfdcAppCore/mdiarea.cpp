#include "Stdafx.h"
#include "mdiarea.h"
#include "FileIOCommands.h"
#include "ViewEvents.h"
#include "QtExtHelpers.h"

using namespace SIM;


QMdiTabBarEx::QMdiTabBarEx(QWidget* parent, QMdiArea* mdiArea)
	: QTabBar(parent)
{
	setMovable(true);
	setTabsClosable(true);
	setExpanding(false);

	connect(this, &QTabBar::currentChanged, [this, mdiArea](int tab)
	{
		if (auto w = getWidget(tab))
		{
			if (auto subWindow = dynamic_cast<QMdiSubWindow*> (w))
			{
				if (mdiArea->activeSubWindow() != subWindow)
				{
					mdiArea->setActiveSubWindow(subWindow);
				}
			}
		}
	});

	connect(this, &QTabBar::tabCloseRequested, [this](int tab)
	{
		if (auto w = getWidget(tab))
		{
			w->close();
		}
	});

	connect(mdiArea, &QMdiArea::subWindowActivated, [this](QMdiSubWindow* window)
	{
		auto t = getTab(dynamic_cast<QWidget*> (window));
		if (t >= 0)
		{
			if (QTabBar::currentIndex() != t)
			{
				QTabBar::setCurrentIndex(t);
			}
		}
	});
}

//-----------------------------------------------------------------------------

int QMdiTabBarEx::getTab(QWidget* widget)
{
	for (int i = 0; i < QTabBar::count(); i++)
	{
		auto data = QTabBar::tabData(i);
		if (auto w = data.value<QWidget*>())
		{
			if (w == widget)
				return i;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------

QWidget* QMdiTabBarEx::getWidget(int tab)
{
	auto data = tabData(tab);
	return data.value<QWidget*>();
}

//-----------------------------------------------------------------------------

void QMdiTabBarEx::addTab(QWidget* widget)
{
	if (widget)
	{
		// Add tab with initial title
		auto title = widget->windowTitle();
		auto tab = QTabBar::addTab(title);

		// Store widget in tab
		QTabBar::setTabData(tab, qVariantFromValue(widget));

		// Watch for window title change 
		connect(widget, &QWidget::windowTitleChanged, [this, widget](const QString& title)
		{
			// Update tab text
			auto tab = getTab(widget);
			if (tab >= 0)
			{
				QTabBar::setTabText(tab, title);
			}
		});

		// Watch for window destroyed
		connect(widget, &QWidget::destroyed, [this](QObject* o)
		{
			// Remove tab
			auto tab = getTab(dynamic_cast<QWidget*> (o));
			if (tab >= 0)
			{
				QTabBar::removeTab(tab);
			}
		});

		QTabBar::setCurrentIndex(tab);
	}
}

//-----------------------------------------------------------------------------

QMdiAreaEx::QMdiAreaEx(QWidget* parent)
	: QMdiArea(parent)
	, mTabBar(nullptr)
{
}

//-----------------------------------------------------------------------------

QTabBar* QMdiAreaEx::tabBar()
{
	// Qt tabbar implementation
	auto windowList = findChildren<QTabBar*>();
	if (windowList.count())
	{
		auto tabBar = windowList.at(0);
		return tabBar;
	}

	// Our implementation
	if (!mTabBar)
	{
		mTabBar = new QMdiTabBarEx(dynamic_cast<QWidget*> (parent()), this);
	}
	return mTabBar;
}

//-----------------------------------------------------------------------------

void QMdiAreaEx::addWindow(QMdiSubWindow* window)
{
	if (mTabBar)
	{
		addSubWindow(window, Qt::FramelessWindowHint);

		mTabBar->addTab(window);
	}
	else
	{
		addSubWindow(window);
	}

}

//-----------------------------------------------------------------------------

void QMdiAreaEx::update(QMdiSubWindow* window)
{
	if (mTabBar && viewMode() == QMdiArea::ViewMode::SubWindowView)
	{
		if (window)
		{
			auto flags = window->windowFlags();
			if (window->isMaximized())
				flags |= Qt::FramelessWindowHint;
			else
				flags &= (~Qt::FramelessWindowHint);
			window->setWindowFlags(flags);
		}
	}
}

//-----------------------------------------------------------------------------

void QMdiAreaEx::paintEvent(QPaintEvent *e)
{

	QMdiArea::paintEvent(e);

	// Draw the standard Autodesk gradient background (the one you see when you close AIRLook based applications)
	QPainter painter(viewport());
	painter.setRenderHint(QPainter::Antialiasing);

	//painter.fillRect( rect(), QColor( 205, 215, 225 ) );
	painter.fillRect(rect(), QColor(222, 222, 223));

	return;
	//// http://stackoverflow.com/questions/19852594/image-on-qt-qmdiarea-background
	//QMdiArea::paintEvent(e);

	//// Draw the standard Autodesk gradient background (the one you see when you close AIRLook based applications)
	//QPainter painter(viewport());
	//painter.setRenderHint(QPainter::Antialiasing);

	//static const auto topLeft = QPointF(0, 0);
	//const auto rc = rect();

	//// Start with the dark background
	//QLinearGradient grad(topLeft, QPointF(0, rc.bottom()));
	//static const QColor bgTopColour(102, 104, 106);
	//static const QColor bgBottomColour(34, 34, 34);
	//grad.setColorAt(0.0, bgTopColour);
	//grad.setColorAt(1.0, bgBottomColour);
	//painter.fillRect(rc, QBrush(grad));

	//// Now the company branding
	//static const QPixmap logo = QtExtHelpers::dpiPixmap(QPixmap(":/NfdcAppCore/images/autodesk-white.png"));
	//static const int logoWidth = logo.width();
	//static const int logoHeight = logo.height();

	//int x = width() - logoWidth;
	//int y = height() - logoHeight;

	//// Adjust for tabbar!
	//auto tabBar = this->tabBar();
	//if (tabBar && tabBar->isVisible())
	//{
	//	y -= tabBar->height();
	//}

	//painter.drawPixmap(x, y, logo);
}

//-----------------------------------------------------------------------------

QMdiSubWindowEx::QMdiSubWindowEx(QWidget* parent, QWidget* w, SIM::Document& document)
	: QMdiSubWindow(parent),_document(document)
{
#if defined(__APPLE__) || defined(LINUX)
	setMouseTracking(true);
#endif // defined(__APPLE__) || defined(LINUX)

	setWidget(w);

	connect(this, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), this,
		SLOT(handleWindowStateChanged(Qt::WindowStates, Qt::WindowStates)));
}

//-----------------------------------------------------------------------------

void QMdiSubWindowEx::mouseMoveEvent(QMouseEvent* e)
{
	QMdiSubWindow::mouseMoveEvent(e);

#if defined(__APPLE__) || defined(LINUX)
	// Ref: https://bugreports.qt-project.org/browse/QTBUG-19924
	// Revert this bug, update contents while dragging frame window
	if (e->buttons() && Qt::LeftButton)
	{
		// Get wigdte position
		auto p = widget()->pos();

		// Turn off display updates, shift window slightly
		widget()->setUpdatesEnabled(false);
		widget()->move(p.x() - 1, p.y() - 1);
		widget()->setUpdatesEnabled(true);

		// Restore window position
		widget()->move(p.x(), p.y());

		// Repaint
		widget()->update();
	}
#endif // defined(__APPLE__) || defined(LINUX)
}

//-----------------------------------------------------------------------------
void QMdiSubWindowEx::handleWindowStateChanged(Qt::WindowStates oldState, Qt::WindowStates newState)
{
	if (newState & Qt::WindowActive)
		NotifyObservers(SIM::MdiSubWindowActivatedEvent(*this, _document));
}

void QMdiSubWindowEx::closeEvent(QCloseEvent* ev)
{
    auto resBtn = IOCommands::PromptForSave(GetDocument());

    if (resBtn == QMessageBox::Cancel)
    {
        ev->ignore();
        return;
    }
    if (resBtn == QMessageBox::Yes)
    {
        if (!GetDocument().GetApplication().GetController().RunDocumentCommand(SaveFileCommand::Name, &GetDocument()))
        {
            ev->ignore();
            return;
        }
    }

    ev->accept();
    NotifyObservers(SIM::MdiSubWindowClosedEvent(*this, _document));
    QMdiSubWindow::closeEvent(ev);
}

