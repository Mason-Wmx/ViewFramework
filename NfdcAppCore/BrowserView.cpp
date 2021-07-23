#include "stdafx.h"
#include "Application.h"
#include "AppCommand.h"
#include "BrowserTreeCommand.h"
#include "BrowserView.h"
#include "mainwindow.h"
#include "ModelEvents.h"
#include "Document.h"

using namespace SIM;

SIM::BrowserView::BrowserView(MainWindow & mainWindow)
	: DockerView(tr("Browser"), mainWindow),
	_tree(NULL),
	_layout(NULL)
{
	Initialize();
}

SIM::BrowserView::~BrowserView(void)
{}

std::string SIM::BrowserView::GetCloseEventCommandName(void)
{
	return CloseBrowserTreeCommand::Name;
}

void SIM::BrowserView::Initialize(void)
{
	setTitleBarWidget(new QWidget());
	setFeatures(NoDockWidgetFeatures);

	QWidget* widget = new QWidget(this);
	_layout = new QVBoxLayout(widget);
	_layout->setContentsMargins(0, 0, 0, 0);
	_layout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);

	widget->setLayout(_layout);

	QScrollArea* scroll = new QScrollArea(this);
	scroll->setWidget(widget);
	scroll->setWidgetResizable(true);
	scroll->setBackgroundRole(QPalette::Light);
	scroll->setContentsMargins(0, 0, 0, 0);

	setWidget(scroll);
}

void SIM::BrowserView::Notify(Event& ev)
{
	if (ev.Is<BrowserVisibilityChangedEvent>())
	{
		if (this->isHidden() && GetMainWindow().GetApplication().GetModel().GetBrowserVisible())
			this->show();
		else if (!this->isHidden() && !GetMainWindow().GetApplication().GetModel().GetBrowserVisible())
			this->hide();

		return;
	}

	if (ev.Is<ActiveDocumentChangedEvent>())
	{
		SetDocument();
		return;
	}
}

void SIM::BrowserView::SetDocument(void)
{
    if (_tree)
    {
        _layout->removeWidget(_tree);
        _tree->setParent(NULL);
    }

    _tree = NULL;

    Document* doc = GetMainWindow().GetApplication().GetActiveDocument().get();
    if (doc)
    {
        _tree = &doc->GetView().GetBrowserTree();
    }

    if (_tree)
    {
        _layout->addWidget(_tree);
    }
}
