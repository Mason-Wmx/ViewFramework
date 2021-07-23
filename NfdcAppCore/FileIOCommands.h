#pragma once

#include "stdafx.h"
#include "Command.h"
#include "AppCommand.h"
#include "DocCommand.h"
#include "QString"
#include "vtkExtDataFile.h"
#include "FileSystem.h"
#include "BaseDialog.h"

namespace SIM
{

class IOCommands
{
public:
    static std::shared_ptr<Document> NewDocument(Application & app);
    static bool CanCreateDocument(Application & app, bool bCloseActiveDocumentInSDIMode = false);
    static QMessageBox::StandardButton PromptForSave(Document & doc);
    static bool SaveAs(Document & doc);
    static bool Save(Document & doc);
protected:
    static bool SaveInternal(Document & doc, const QString & scalarisDataModelDirectory); //scalarisDataModelDirectory is expected to be in format returned by DocModel::GetScalarisDataModelDirectory()
};

class NFDCAPPCORE_EXPORT SaveAsFileCommand : public DocCommand
{
public:
	SaveAsFileCommand(Application& application)
		: DocCommand(application)
	{
	}

	virtual bool Execute() override;

	std::string GetUniqueName() override;

	virtual QString GetLabel(){ return QObject::tr("Save As"); }
	virtual QString GetHint() { return QObject::tr("Save As"); }
	virtual std::string GetLargeIcon() { return ":/NfdcAppCore/images/save_as_32_32x32.png"; }
	virtual std::string GetSmallIcon() { return ":/NfdcAppCore/images/save_as_16_16x16.png"; }

	virtual bool IsEnabled();

	virtual QKeySequence GetKeySequence();

	virtual bool IsSensitiveToEvent(Event& ev);

	static const std::string Name;
};

class NFDCAPPCORE_EXPORT SaveFileCommand : public DocCommand
{
public:
    SaveFileCommand(Application& application)
        : DocCommand(application)
    {
    }

    bool Execute() override;

    std::string GetUniqueName() override;

	virtual QString GetLabel(){ return QObject::tr("Save"); }
	virtual QString GetHint() { return QObject::tr("Save"); }
	std::string GetLargeIcon() { return ":/NfdcAppCore/images/save_32_32x32.png"; }
	std::string GetSmallIcon() { return ":/NfdcAppCore/images/save_16_16x16.png"; }

    virtual bool IsEnabled() override;

	virtual QKeySequence GetKeySequence();

    virtual bool IsSensitiveToEvent(Event& ev) override;

    static const std::string Name;
};

class NFDCAPPCORE_EXPORT UploadProjectCommand : public DocCommand
{
public:
  UploadProjectCommand(Application& application)
    : DocCommand(application)
    , _sync(nullptr)
  {
  }
  virtual ~UploadProjectCommand();

  bool Execute() override;

  std::string GetUniqueName() override;

  virtual QString GetLabel() { return QObject::tr("Upload project"); }
  virtual QString GetHint() { return QObject::tr("Upload project"); }
  std::string GetLargeIcon() { return ":/NfdcAppCore/images/save_forge_32_32x32.png"; }
  std::string GetSmallIcon() { return ":/NfdcAppCore/images/save_forge_16_16x16.png"; }

  virtual QKeySequence GetKeySequence();

  static const std::string Name;

public:
	static constexpr const char * HubTypeKey = "hub-type";

private:
  std::thread _uploadWorker;
  void pushProject(QString environment, QString accessToken, QString sourcePath, QString projectName, QString hubId);
  std::atomic<QProcess*> _sync{ nullptr };
};

class NFDCAPPCORE_EXPORT NewFileCommand : public AppCommand
{
public:
    NewFileCommand(Application& application)
        : AppCommand(application)
    {
    }

    bool Execute() override;

    std::string GetUniqueName() override;

    virtual QString GetLabel(){ return QObject::tr("New"); }
    virtual QString GetHint() { return QObject::tr("New"); }
    std::string GetLargeIcon() { return ":/NfdcAppCore/images/new-32x32.png"; }
    std::string GetSmallIcon() { return ":/NfdcAppCore/images/new-16x16.png"; }

    virtual QKeySequence GetKeySequence();

    static const std::string Name;
};

class NFDCAPPCORE_EXPORT OpenFileCommand : public AppCommand
{
public:
    OpenFileCommand(Application& application)
        : AppCommand(application)
    {
    }

    bool Execute() override;

    std::string GetUniqueName() override;

    virtual QString GetLabel(){ return QObject::tr("Open"); }
    virtual QString GetHint() { return QObject::tr("Open"); }
    std::string GetLargeIcon() { return ":/NfdcAppCore/images/open_32_32x32.png"; }
    std::string GetSmallIcon() { return ":/NfdcAppCore/images/open_16_16x16.png"; }

    virtual QKeySequence GetKeySequence();

    static const std::string Name;
};

class NFDCAPPCORE_EXPORT OpenProjectDialogCommand : public AppCommand
{
public:
	OpenProjectDialogCommand(Application& application)
		: AppCommand(application)
	{
	}

	bool Execute() override;

	std::string GetUniqueName() override;

	virtual QString GetLabel() { return QObject::tr("Open project"); }
	virtual QString GetHint() { return QObject::tr("Open project"); }
	std::string GetLargeIcon() { return ":/NfdcAppCore/images/open_32_32x32.png"; }
	std::string GetSmallIcon() { return ":/NfdcAppCore/images/open_project_dialog_16_16x16.png"; }

	virtual QKeySequence GetKeySequence();

	static const std::string Name;
};

class NFDCAPPCORE_EXPORT ImportFileCommand : public AppCommand
{
public:
    ImportFileCommand(Application & application)
        : AppCommand(application)
    {
    }

    bool Execute() override;
    std::string GetUniqueName() override;
    virtual QString GetLabel() { return QObject::tr("Import"); } //shouldn't be override?
    virtual QString GetHint() { return QObject::tr("Import"); } //shouldn't be override?

    static const std::string Name;

protected:
    QString openFilter();
    QString formFileDialogFilter(const std::vector<std::pair<std::string, std::string>>& fileDescriptors, bool forOpen/* = true*/);
    std::pair<vtkSmartPointer<vtkPolyData>,int> readPolyDataFromFileFirstPass(const std::string& fileName);
    void readPolyDataFromFileSecondPass(vtkSmartPointer<vtkPolyData>& polyData);
    bool showImportedUnitsDialog(const std::vector<vtkSmartPointer<vtkPolyData>> polyData, std::vector<int>& units, std::vector<std::string> fullFileNames);
    vtkSmartPointer<vtkPolyData> readPolyDataFromFile(const std::string& fileName);
    unsigned int limitPointClouds(const std::string& fileName, unsigned int value = 10) const;
    void scaleModelAndSanitizeColorsNormalsAndTextures(vtkSmartPointer<vtkPolyData>& polyData, int units, bool& optimize);
    bool AddPolyDataToDocument(const std::vector<vtkSmartPointer<vtkPolyData>> & dataVector, const std::vector<std::string> & fileNames);

    std::string GetLargeIcon() { return ":/NfdcAppCore/images/import_geometry_32x32.png"; } //shouldn't be override?
    std::string GetSmallIcon() { return ":/NfdcAppCore/images/import_geometry_16x16.png"; } //shouldn't be override?
};

}

class ImportedUnitsDialog : public BaseDialog
{
  Q_OBJECT

public:
  ImportedUnitsDialog(int& unitType, const std::vector<QString> fileNames, const std::vector<std::tuple<double, double, double>> boundingBox, QWidget *parent);
  ~ImportedUnitsDialog();
private:
  const QString  _name = "importedUnitsDialog";
  QComboBox* _pUnitComboBox;
  int& _unitType;
private slots:
  void unitComboBoxIndexChanged(int index);
};

class NFDCAPPCORE_EXPORT DRYFile : public vtkExtDataFile
{
public:
	static void filterDRY(std::string& uiDescriptipon, std::string& fileExt)
	{
		const static std::string ui = "DRY";
		const static std::string ext = "dry";
		uiDescriptipon = ui;
		fileExt = ext;
	}

	static bool isSupported(const char* fileName)
	{
		if (!fileName)
			return false;

		std::string unused, dryExt;
		filterDRY(unused, dryExt);


		return FileSystem::isFileType(fileName, dryExt);
	}
};
