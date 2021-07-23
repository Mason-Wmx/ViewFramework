#pragma once
#include "stdafx.h"
#include "../NfdcAppCore/BaseDialog.h"
#include "../NfdcAppCore/Application.h"
#include "../NfdcAppCore/ProjectManager.h"

namespace SIM
{
    struct ProjectItem;
    struct ProjectItemWithThumbnail;

	/*
		Used in ProjectsItemModel for custom data roles
	*/
	enum ProjectsListUserDataRoles {
		IDRole = Qt::UserRole,
        CreateTimeRole = Qt::UserRole + 1,
        ModifiedTimeRole = Qt::UserRole + 2 
	};

	// Styling of ProjectsListViewDelegate
	const int thumbnailWidth = 160;
	const int thumbnailHeight = 112;
	const QColor thumbnailBackgroundColor = QColor("#FFFFFF");
	const int itemWidth = 160;
	const int itemHeight = 160;
    const int itemHorizontalSpacing = 0;//36;
    const int itemVerticalSpacing = 0;//36;

	/*
		Displays notification messages in list view when
		- project list is being fetched
		- there are no projects available
	*/
	class ProjectsListView : public QListView 
	{
	public:
		ProjectsListView(QWidget *parent = 0) : QListView(parent) {}

		void setDownloadingProjectsLists(bool result) 
		{ 
			QMutexLocker l(&_mutex);
			_downloadingProjectsList = result;
		}

    protected:
        virtual void updateGeometries() 
        {
            QListView::updateGeometries();

            verticalScrollBar()->setSingleStep(15);
        }

	private:
		QMutex _mutex;
		bool _downloadingProjectsList{ false };

		void paintEvent(QPaintEvent *e) 
		{
			QListView::paintEvent(e);

			if (model() && model()->rowCount(rootIndex()) > 0) 
				return;

			QPainter p(this->viewport());
			
			QFont font = p.font();
			font.setPixelSize(16);
			p.setFont(font);

			QMutexLocker l(&_mutex);

			if (_downloadingProjectsList)
				p.drawText(rect(), Qt::AlignCenter, tr("Downloading projects list..."));
			else
				p.drawText(rect(), Qt::AlignCenter, tr("There are no projects available"));
		}
	};

	/*
		Implements custom draw function of QListView items
	*/
	class ProjectsListViewDelegate : public QStyledItemDelegate
	{
		Q_OBJECT
	public:
		ProjectsListViewDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

		QSize ProjectsListViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
		{
			return QSize(itemWidth + itemHorizontalSpacing, itemHeight + itemVerticalSpacing);
		}

		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
		{
			painter->save();

			// Exclude spacing between items from drawable region
			QRect itemRect = option.rect;
            itemRect.setX(itemRect.x() + itemHorizontalSpacing);
			itemRect.setSize(QSize(itemWidth, itemHeight));

            // Border rect has to take into account pen width
            QRectF borderRect(itemRect);
            borderRect.setX(borderRect.x() + 0.5);
            borderRect.setY(borderRect.y() + 0.5);
            borderRect.setWidth(borderRect.width() - 1);
            borderRect.setHeight(borderRect.height() - 1);

			// Background
			painter->fillRect(itemRect, Qt::white);

			// Icon
			QRect iconRect = itemRect;
			iconRect.setSize(QSize(thumbnailWidth, thumbnailHeight));

			painter->drawPixmap(iconRect, index.data(Qt::DecorationRole).value<QPixmap>());
			
            // If item is not selected we need to draw border before text
            if (!(option.state & QStyle::State_Selected) && !(option.state & QStyle::State_MouseOver)) {
                auto originalPen = painter->pen();
                painter->setPen(QPen(Qt::white));
                painter->drawRect(borderRect);
                painter->setPen(originalPen);
            }

			// Text
			QRect textRect = itemRect;
            textRect.setY(textRect.y() + thumbnailHeight);
            
            painter->fillRect(textRect, QColor("#F0F0F0"));

            textRect.setY(textRect.y() + 5);
			textRect.setX(textRect.x() + 14);
            textRect.setWidth(textRect.width()-14);

			QFont projectNameFont = painter->font();
			projectNameFont.setPixelSize(13);
            projectNameFont.setBold(true);

            QFontMetrics fm(projectNameFont);

			painter->setFont(projectNameFont);
			painter->drawText(textRect, fm.elidedText(index.data(Qt::DisplayRole).toString(),Qt::ElideRight, textRect.width()));
			
			textRect.setY(textRect.y() + fm.height() + 1);

			QFont dateTimeFont = painter->font();
			dateTimeFont.setPixelSize(11);
            dateTimeFont.setBold(false);
			painter->setFont(dateTimeFont);
			painter->drawText(textRect, index.data(ModifiedTimeRole).toDateTime().toString("MM/dd/yyyy h:mm ap"));

            // Border
            if ((option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver)) {
                QPen borderPen;

                if (option.state & QStyle::State_Selected)
                    borderPen.setColor(QColor("#05A7DF"));
                else if (option.state & QStyle::State_MouseOver)
                    borderPen.setColor(QColor("#DCDCDC"));

                painter->setPen(borderPen);
                painter->drawRect(borderRect);
            }

			painter->restore();
		}
	};

	/*
		Stores elements displayed in QListView
	*/
	class ProjectsItemModel : public QAbstractItemModel
	{
		Q_OBJECT

	public:
		explicit ProjectsItemModel(QObject *parent = 0);
		~ProjectsItemModel();

		QModelIndex index(int row, int column, const QModelIndex &parent) const override;
		QModelIndex parent(const QModelIndex &index) const override;
		int rowCount(const QModelIndex &parent = QModelIndex()) const override;
		int columnCount(const QModelIndex &parent = QModelIndex()) const override;
		QVariant data(const QModelIndex &index, int role) const override;

		void setProjectItems(ProjectManager * pProjectManager, const QList<ProjectItem> &projectItems); //pProjectManager might be nullptr - then thumbnails will not be generated.

        ProjectItemWithThumbnail projectItem(int row) const;

	private:
		QList<ProjectItemWithThumbnail> _projectItems;
		QPixmap _defaultThumbnail;
	};

    const int plusLength = 50;
    const int labelHeight = 48;

    class NewButton : public QPushButton
    {
        Q_OBJECT
    public:
        NewButton(const QString &text) : QPushButton(text) {}

        QSize sizeHint() const override
        {
            return QSize(160, 160);
        }

    protected:
        void mousePressEvent(QMouseEvent * e) 
        {
            _pressed = true;

            QPushButton::mousePressEvent(e);
        }

        void mouseReleaseEvent(QMouseEvent * e) 
        {
            _pressed = false;

            QPushButton::mouseReleaseEvent(e);
        }

        void paintEvent(QPaintEvent *event) override
        {
            const QRect itemRect = QRect(QPoint(0,0), sizeHint());

            QPainter painter(this);
            QStyleOptionButton option;
            option.initFrom(this);

            // Background
            if (_pressed)
                painter.fillRect(itemRect, QColor("#007FBE"));
            else if (option.state & QStyle::State_MouseOver)
                painter.fillRect(itemRect, QColor("#42B6DF"));
            else
                painter.fillRect(itemRect, QColor("#05A7DF"));

            // Plus
            QPen pen(Qt::white, 2);
            painter.setPen(pen);

            QPoint startVertical(itemRect.width()/2, (itemRect.height()-labelHeight-plusLength)/2);
            QPoint endVertical(startVertical.x(), startVertical.y()+plusLength);
            QPoint startHorizontal((itemRect.width() - plusLength) / 2, (itemRect.height() - labelHeight) / 2);
            QPoint endHorizontal(startHorizontal.x() + plusLength, startHorizontal.y());

            painter.drawLine(startVertical, endVertical);
            painter.drawLine(startHorizontal, endHorizontal);

            // Text
            QRect textRect = itemRect;
            textRect.setY(itemRect.height() - labelHeight);
            textRect.setHeight(labelHeight);

            painter.fillRect(textRect, QColor("#007FBE"));
            painter.drawText(textRect, text(), QTextOption(Qt::AlignCenter));
        }

    private:
        bool _pressed{ false };
    };

	/*
		Dialog for selecting project from Autodesk 360
	*/
	class OpenProjectDialog : public BaseDialog, public Observer
	{
		Q_OBJECT

	public:
		OpenProjectDialog(SIM::Application &rApp, QWidget *parent = nullptr);
		~OpenProjectDialog();

	protected slots:
        void loadProject(const QModelIndex &index);
        void customContextMenuRequested(const QPoint &pos);

    protected:
        bool eventFilter(QObject *obj, QEvent *event) override;

	private:
        void Notify(Event & ev);
        void FillProjectList();
        void retranslateUi(QDialog *Dialog);
        //void fillProjectsItemModel();

		SIM::Application & _application;

		ProjectsItemModel * _projectsItemModel;
		ProjectsListView * _projectsListView;
        QSortFilterProxyModel * _sortModel;
		QPushButton * _newProjectButton;
	};
}
