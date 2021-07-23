#include "stdafx.h"
#include "OpenProjectDialog.h"

#include "AppEvents.h"
#include "ContextMenu.h"
#include "headers/dbg_TimeProfiler.h"
#include "MainThreadDispatcher.h"
#include "mainwindow.h"
#include "FileIOCommands.h"
#include "ProjectItem.h"
#include "ProjectManager.h"

#include <chrono>
#include <thread>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QListView>
#include <QtConcurrent/qtconcurrentrun.h>



namespace SIM
{
    struct ProjectItemWithThumbnail : public ProjectItem
    {
        QPixmap thumbnail;
    };
}


using namespace SIM;

OpenProjectDialog::OpenProjectDialog(SIM::Application& app, QWidget *parent) :
    BaseDialog(parent),
    _application(app),
    _newProjectButton(new NewButton(tr("New Project"))),
    _projectsListView(new ProjectsListView()),
    _projectsItemModel(new ProjectsItemModel()),
    _sortModel(new QSortFilterProxyModel(this))
{
    START_TIMED_BLOCK("Open Dialog, just opening 'Open Dialog'");

    SetHeaderVisible(false);

    setObjectName("openProjectDialog");

	_newProjectButton->setObjectName("openProjectDialogNewProject");
    _newProjectButton->setAutoDefault(false);
    _newProjectButton->setFocusPolicy(Qt::ClickFocus);
    _sortModel->setSortRole(ModifiedTimeRole);
    _sortModel->setSourceModel(_projectsItemModel);
    _projectsListView->setObjectName("openProjectDialogProjectsListView");
	_projectsListView->setModel(_sortModel);
	_projectsListView->setItemDelegate(new ProjectsListViewDelegate());
	_projectsListView->setMouseTracking(true);
	_projectsListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_projectsListView->setViewMode(QListWidget::IconMode);
	_projectsListView->setResizeMode(QListWidget::Adjust);
    _projectsListView->setVerticalScrollMode(QListView::ScrollPerPixel);
    _projectsListView->setHorizontalScrollMode(QListView::ScrollPerPixel);
    _projectsListView->setContextMenuPolicy(Qt::CustomContextMenu);
    _projectsListView->setGridSize(QSize(160+36,160+36));

	QLabel *projectsLabel = new QLabel(tr("Projects"));
	projectsLabel->setObjectName("openProjectDialogHeaderLabel");

    QPushButton *closeButton = new QPushButton();
    closeButton->setObjectName("closeButton");
    closeButton->setAutoDefault(false);
    closeButton->setFocusPolicy(Qt::ClickFocus);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setMargin(0);
    headerLayout->addItem(new QSpacerItem(36, 32, QSizePolicy::Fixed, QSizePolicy::Fixed));
    headerLayout->addWidget(projectsLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(closeButton);

	QGridLayout *layout = new QGridLayout(this);
	layout->setObjectName(QStringLiteral("openProjectDialogGridLayout"));
	layout->setContentsMargins(16, 0, 0, 16);
	layout->setVerticalSpacing(12);
	layout->setHorizontalSpacing(24);
	layout->addWidget(_newProjectButton, 2, 1, 1, 1);
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding), 3, 1, 1, 1);
    layout->addItem(new QSpacerItem(12, 1, QSizePolicy::Fixed, QSizePolicy::Fixed), 1, 2, 1, 1);
	layout->addLayout(headerLayout, 1, 3, 1, 1);
	layout->addWidget(_projectsListView, 2, 3, 2, 1);
	GetMainLayout()->addLayout(layout);

    _application.GetModel().RegisterObserver(*this); //to listen ProjectListRefreshedEvent(). Listening ProjectListRefreshedEvent() need to be done before FillProjectList()
    FillProjectList();

	connect(_newProjectButton, &QPushButton::clicked, [=]() {
		accept();

		_application.GetController().RunCommand(NewFileCommand::Name);
	});
    connect(closeButton, &QPushButton::clicked, [=]() {
        reject();
    });
    connect(_projectsListView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(loadProject(const QModelIndex &)));
    connect(_projectsListView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));

    QMetaObject::connectSlotsByName(this);

    installEventFilter(this);

    END_TIMED_BLOCK("Open Dialog, just opening 'Open Dialog'");
}

OpenProjectDialog::~OpenProjectDialog()
{
}

void OpenProjectDialog::Notify(Event & ev)
{
    EventSwitch es(ev);
    es.Case<ProjectListRefreshedEvent>(std::bind(&OpenProjectDialog::FillProjectList, this));
}


void OpenProjectDialog::FillProjectList()
{
    auto pProjectManager = std::make_unique<ProjectManager>(_application);

    std::string currentHubId = _application.GetCurrentHubId();
    std::string server = _application.GetISSO() ? _application.GetISSO()->GetServer() : "";
    std::string userId = _application.GetISSO() ? _application.GetISSO()->GetUserId() : "";
    
    bool found;
    const QList<ProjectItem> & refreshedProjectList = _application.GetModel().GetCachedProjects(userId, server, currentHubId, &found);
    _projectsListView->setDownloadingProjectsLists(!found); //if there is no cached project list for particular user, it means that downloading project list is in progress (if it isn't, this is bug)
    _projectsItemModel->setProjectItems(pProjectManager.get(), refreshedProjectList);

    MainThreadDispatcher::Post([=]() {
        _sortModel->sort(0, Qt::DescendingOrder);
    });

}



void OpenProjectDialog::retranslateUi(QDialog *Dialog)
{

}


void OpenProjectDialog::loadProject(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    const QModelIndex modelIndex = _sortModel->mapToSource(index);

    auto projectItem = _projectsItemModel->projectItem(modelIndex.row());

    if (_application.HasActiveDocument()) {
        if (QString::fromStdString(_application.GetActiveDocument()->GetProjectId()) == projectItem.id) {
            if (QMessageBox::question(&_application.GetMainWindow(), _application.GetConfig()->application(), tr("The project is already open. Do you want to reopen it?"), QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes) == QMessageBox::No)
                return;
        }
    }

    // Hide dialog and load selected project
    accept();

    auto projectManager = std::make_unique<ProjectManager>(_application);

    if (!projectManager->CanLoadProject())
        return;

    projectManager->LoadProject(projectItem.name, QString::fromStdString(_application.GetCurrentHubId()), projectItem.id);
}

void OpenProjectDialog::customContextMenuRequested(const QPoint &pos)
{
    auto index = _projectsListView->indexAt(pos);

    if (!index.isValid())
        return;

    auto menu = new ContextMenu(&_application.GetMainWindow());

    connect(menu->addContextMenuAction(tr("Open")), &QAction::triggered, [=]() {
        loadProject(index);
    }); 

    menu->exec(QCursor::pos());
}

bool OpenProjectDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);

        if ((key->key() == Qt::Key_Enter) || (key->key() == Qt::Key_Return)) {
            auto indexes = _projectsListView->selectionModel()->selectedIndexes();

            if (!indexes.empty()) {
                loadProject(indexes.first());
                return false;
            }
        }
    }

    return QObject::eventFilter(obj, event);
}

ProjectsItemModel::ProjectsItemModel(QObject *parent) :
	QAbstractItemModel(parent)
{
	_defaultThumbnail = QPixmap(thumbnailWidth, thumbnailHeight);
	_defaultThumbnail.fill(thumbnailBackgroundColor);

	QPainter painter(&_defaultThumbnail);
	painter.setPen(QPen(QColor("#F0F0F0")));
	painter.drawLine(0, 0, thumbnailWidth, thumbnailHeight);
	painter.drawLine(thumbnailWidth, 0, 0, thumbnailHeight);
}

ProjectsItemModel::~ProjectsItemModel()
{
}

QModelIndex ProjectsItemModel::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column);
}

QModelIndex ProjectsItemModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int ProjectsItemModel::rowCount(const QModelIndex &parent) const
{
	return _projectItems.size();
}

int ProjectsItemModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

QVariant ProjectsItemModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	switch (role) {
	case Qt::DisplayRole:
		return _projectItems.at(index.row()).name;
	case ModifiedTimeRole:
		return _projectItems.at(index.row()).modifiedTime;
	case IDRole:
		return _projectItems.at(index.row()).id;
	case Qt::DecorationRole: {
		if (_projectItems.at(index.row()).thumbnail.isNull())
			return _defaultThumbnail;
		else
			return _projectItems.at(index.row()).thumbnail;
	}
	}

	return QVariant();
}

void ProjectsItemModel::setProjectItems(ProjectManager * pProjectManager, const QList<ProjectItem> &projectItems)
{
	beginResetModel();

    _projectItems.clear();

    for (auto proj : projectItems)
    {
        ProjectItemWithThumbnail projWT;
        projWT.name = proj.name;
        projWT.id = proj.id;
        projWT.createTime = proj.createTime;
        projWT.modifiedTime = proj.modifiedTime;

        if (pProjectManager)
        { // thumbnail generation and adding to list
            QString thumbnailPath = pProjectManager->ThumbnailPath(pProjectManager->ProjectPath(proj.name));
            QImage thumbnail(thumbnailPath);
            if (!thumbnail.isNull())
                projWT.thumbnail = QPixmap::fromImage(thumbnail.scaled(QSize(thumbnailWidth, thumbnailHeight), Qt::KeepAspectRatioByExpanding));
        }

        _projectItems.append(projWT);
    }

	endResetModel();
}

ProjectItemWithThumbnail ProjectsItemModel::projectItem(int row) const
{
	if (_projectItems.size() <= row || row<0)
		return ProjectItemWithThumbnail();

	return _projectItems.at(row);
}