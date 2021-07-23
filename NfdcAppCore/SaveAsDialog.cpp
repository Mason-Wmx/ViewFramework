#include "stdafx.h"
#include "SaveAsDialog.h"

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>
#include <QDir>

#include "../NfdcCommonUI/SelectionCommandInput.h"
#include "../NfdcCommonUI/UnitValueCommandInput.h"
#include "../NfdcCommonUI/CommandInputEvent.h"
#include "Application.h"
#include "MainWindow.h"

using namespace SIM;



QString SaveAsDialog::CreateScalarisDataModelDirectory()
{
  const QLineEdit * pNameEdit = this->findChild<const QLineEdit *>(QString::fromUtf8("NameEdit"));
  if (!pNameEdit)
  {
    Q_ASSERT(false && "Internal error in 'SaveAs' dialog. Could not get find QLineEdit with user entered project name");
    return "";
  }

  QString projectName = pNameEdit->text();
  if (!DocModel::validateProjectName(projectName.toStdString()))
  {
    Q_ASSERT(false && "Internal error in 'SaveAs' dialog. Provided project name is wrong"); //here we should notify user by visible message
    return "";
  }
    
  QString scalarisDataModelDirectory = _appLocalDataUserPath + QDir::separator() + projectName;
  if (!SaveAsDialog::validateSelectedFolder(scalarisDataModelDirectory))
  {
    Q_ASSERT(false && "SaveAs dialog error: Selected directory is not empty - user is overwritting some other project"); //here we should notify user by visible message
    return "";
  }

  _createdScalarisDataModelDirectory = Application::CreateDirectoryIfNotExist(scalarisDataModelDirectory);

  return _createdScalarisDataModelDirectory;
}


//static
bool SaveAsDialog::validateSelectedFolder(const QString & dirNameWithAbsolutePath)
{
  QDir dir = QDir(QDir::toNativeSeparators(dirNameWithAbsolutePath));
  if (!dirNameWithAbsolutePath.isEmpty() && dir.exists() && dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() != 0)
  {
    Q_ASSERT(false && "Selected foler is not empty in 'SaveAs' dialog"); //This Q_ASSERT should be here. General idea is to check dialog availability when user is typing name and disable "Save" button, so code never should fall here.
    return false;
  }

  return true;
}


//static
bool SaveAsDialog::GetPathAndProposedProjectName(Application & app, const DocModel & model, QString & appLocalDataUserPath /*out*/, QString & proposedProjectName /*out*/)
{
  appLocalDataUserPath = QString::fromStdString(app.GetOrCreateAppLocalDataUserPath(/*bShowWarningIfUserNotLogged*/ true));
  Q_ASSERT(!appLocalDataUserPath.isEmpty() && "Could not take AppLocalDataUser path. Model will not be saved");
  if (appLocalDataUserPath.isEmpty())
    return false;

  QString proposedProjectNameTmp = QString::fromStdString(model.GetProjectName(/*ifNotSetReturnProjectTemporaryName*/ true));
  Q_ASSERT(!proposedProjectNameTmp.isEmpty() && "Project name is empty, could not provide suggestion in 'SaveAs' dialog");
  int idx = 0;
  do //create directory with unique name
  {
    QDir dir(appLocalDataUserPath + QDir::separator() + proposedProjectNameTmp + ((idx == 0) ? QString("") : QString::number(idx)));
    if (!dir.exists())
    {
      proposedProjectNameTmp = dir.dirName();
      break;
    }
    idx++;
  } while (idx <= 100); //this test if directory (not file) exist

  if (idx >= 100)
  {
    Q_ASSERT(false && "Can not find unique name for project (Scalaris Data Model). It looks that on HDD there are already directories with that name");
    return false;
  }

  if (!proposedProjectNameTmp.isEmpty())
    proposedProjectName = proposedProjectNameTmp;


  return (!appLocalDataUserPath.isEmpty() && !proposedProjectName.isEmpty());
}


//static
std::unique_ptr<SaveAsDialog> SaveAsDialog::CreateSaveAsDialog(Application & app, const DocModel & model, QWidget * parent /*= NULL*/)
{
  QString appLocalDataUserPath;
  QString proposedProjectName;
  if (!GetPathAndProposedProjectName(app, model, appLocalDataUserPath /*out*/, proposedProjectName /*out*/))
    return nullptr;

  return std::make_unique<SaveAsDialog>(proposedProjectName, appLocalDataUserPath, app, parent);
}


SaveAsDialog::SaveAsDialog(const QString & proposedProjectName, const QString & appLocalDataUserPath, Application & app, QWidget * parent)
  : BaseDialog(parent) 
  , _Application(app)
  , _appLocalDataUserPath(appLocalDataUserPath)
{
  Q_ASSERT((!proposedProjectName.isEmpty() && !appLocalDataUserPath.isEmpty()) && "Missing proposedProjectName or scalarisDataModelPath where project (Scalaris Data Model) could be saved. 'SaveAs' dialog was created but it fail. Fix method that called this method");

  //set BaseDialog
  BaseDialog::SetHeaderTitle(QApplication::translate("SaveAsDialog", "Save", Q_NULLPTR));

  QVBoxLayout* pMainLayout = GetMainLayout();

  //set up layouts
  QGridLayout * pGridLayout = new QGridLayout();
  pGridLayout->setObjectName(QStringLiteral("gridLayout"));

  pMainLayout->addLayout(pGridLayout);

  { // Name (Project Name):
    QLabel * pNameLabel = new QLabel(this);
    pNameLabel->setText(QApplication::translate("SaveAsDialog", "Name:", Q_NULLPTR));
    pGridLayout->addWidget(pNameLabel, 1, 0, 1, 2);

    SelectAllOnClickLineEdit * pNameEdit = new SelectAllOnClickLineEdit(this);
    pNameEdit->setObjectName(QString::fromUtf8("NameEdit"));
    pNameEdit->setText(proposedProjectName);

    // Define the minimum SaveAsDialog size using QLineEdit width
    pNameEdit->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    pNameEdit->setMinimumWidth(/*twice the default dialog width*/ 288 * 2);
    pNameEdit->selectAll(); //After opening dialog we would like to have focus in Name control with all content selected, so user can start typing project name without additional mouse click and selection in name edit

    pGridLayout->addWidget(pNameEdit, 2, 0, 1, 2);
  }


  { // A360 Location:
    QLabel * pLocationLabel = new QLabel(this);
    pLocationLabel->setText(QApplication::translate("SaveAsDialog", "A360 Location:", Q_NULLPTR));
    pGridLayout->addWidget(pLocationLabel, 4, 0, 1, 2);

    QLineEdit * pLocationEdit = new QLineEdit(this);
    pLocationEdit->setReadOnly(true);
    pLocationEdit->setText(QString::fromStdString(_Application.GetCurrentHubName()));
    pGridLayout->addWidget(pLocationEdit, 5, 0, 1, 2);
  }

  SaveAsDialog::CreateBottomFrameAndAdjustStyle();

  QMetaObject::connectSlotsByName(this);
}




// This method modify default Lakota dialog style to be aligned with UX requirements in DC-837
//
// In the Lakota style dialogs have "OK" button is the second from the right. In the save dialog we want to "OK" ("Save") be first from the right.
// Additionally we need to rename "OK" to "Save"
// also remove bottom horizontal line
bool SaveAsDialog::CreateBottomFrameAndAdjustStyleInt()
{
  //call parent method which create default bottom of dialog
  BaseDialog::CreateBottomFrame(/*bOKCancel*/ true);

  //
  // below code modify default bottom dialog style to adopt to requirements
  //

  QPushButton * pOKButton = GetOKButton();
  QHBoxLayout * pBottomLayout = GetBottomLayout();
  if (!pOKButton || !pBottomLayout)
    return false;

  pOKButton->setText(QApplication::translate("SaveAsDialogButton", "Save", Q_NULLPTR)); //rename "OK" to "Save"

  { //remove bottom horizontal line
    QFrame * pBotFrameDivider = BotFrameDivider();
    if (!pBotFrameDivider)
      return false;

    pBotFrameDivider->hide();
  }

  return true;
}


bool SaveAsDialog::CreateBottomFrameAndAdjustStyle()
{
  bool bRes = CreateBottomFrameAndAdjustStyleInt();
  Q_ASSERT(bRes && "Failure style modyfication in SaveAs dialog");

  return bRes;
}


SaveAsDialog::~SaveAsDialog()
{
}

void SaveAsDialog::OkButtonClicked()
{
  QString scalarisDataModelPathWithProjectDir = CreateScalarisDataModelDirectory();
  if (scalarisDataModelPathWithProjectDir.isEmpty())
  {
    QMessageBox::critical(this, _Application.GetConfig()->message(), QObject::tr("Invalid project name or project name already exists. \nProvide new project name."));
    return;
  }

  BaseDialog::OkButtonClicked();
}
