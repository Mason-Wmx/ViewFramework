#pragma once
#include "stdafx.h"
#include "BaseDialog.h"

#include <QLineEdit>

class QPushButton;


namespace SIM
{
  class SelectionCommandInput;
  class UnitValueCommandInput;
  class Application;
  class DocModel;

  class SelectAllOnClickLineEdit : public QLineEdit
  {
	  Q_OBJECT
  public:
	  SelectAllOnClickLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

  protected:
	  void SelectAllOnClickLineEdit::mousePressEvent(QMouseEvent *e)
	  {
		  m_wasSelected = hasSelectedText();

		  QLineEdit::mousePressEvent(e);
	  }

	  void SelectAllOnClickLineEdit::mouseReleaseEvent(QMouseEvent *e)
	  {
		  // The line edit will not be selected if:
		  // - the user clicks a control that already has selection
		  // - the user drags mouse over control and selects a range
		  bool selectAllText = !m_wasSelected && !hasSelectedText();

		  QLineEdit::mouseReleaseEvent(e);

		  if (selectAllText)
			selectAll();
	  }

  private:
	  bool m_wasSelected{ false };
};

  class SaveAsDialog : public BaseDialog
  {
    Q_OBJECT

  public: //Statics
    static std::unique_ptr<SaveAsDialog>  CreateSaveAsDialog(Application & app, const DocModel & model, QWidget * parent = NULL);
    static bool                           GetPathAndProposedProjectName(Application & app, const DocModel & model, QString & appLocalDataUserPath /*out*/, QString & proposedProjectName /*out*/); //Return true when both are returned correctly, false otherwise.   Probably this method should be moved to better place
    static bool                           validateSelectedFolder(const QString & dirNameWithAbsolutePath); //this method verify if provided dirName exist and is empty. Return true if provided folder is OK to save Scalaris Data Model, false otherwise.   Probably this method should be moved to better place


  public:
    SaveAsDialog(const QString & proposedProjectName, const QString & scalarisDataModelPath, Application & app, QWidget * parent = NULL);
  public:
    ~SaveAsDialog();

    // Call this method after exiting from dialog to get directory with path to which Scalaris Data Model should be serialized. 
    // Return empty string on failure or if user canceled save.
    // Return path to existing empty directory e.g. "C:/Users/<OS_USER>/AppData/Local/Autodesk/Autodesk Generative Design/<A360_USER>/<ProjectName>".
    // Returned path is an absolute path which have slashes '/' used as separator characters.
    const QString &       GetScalarisDataModelDirectory() { return _createdScalarisDataModelDirectory; }
    

  protected slots:
    virtual void          OkButtonClicked() override;

  protected:
    // This method create Scalaris Data Model directory.
    // This method is called when user click "Save" ("OK") button
    // Return empty string on failure or path to existing empty directory e.g. "C:/Users/<OS_USER>/AppData/Local/Autodesk/Autodesk Generative Design/<A360_USER>/<ProjectName>".
    QString               CreateScalarisDataModelDirectory();

    // This method modify default Lakota dialog style to be aligned with UX requirements DC-837
    // return true on success and false when modification of styles failed (probably dialog still will usable but it may wrong style and names etc.)
    bool                  CreateBottomFrameAndAdjustStyle();
    bool                  CreateBottomFrameAndAdjustStyleInt(); //see description for bool CreateBottomFrameAndAdjustStyle() method

  private:
    Application &         _Application; //this should be const but some access methods used in code have missing const, so we can't use const here
    const QString         _appLocalDataUserPath; //it is const because we want to protect against changes. It is not & because original object is deleted while dialog is running.
    QString               _createdScalarisDataModelDirectory;
  };


}