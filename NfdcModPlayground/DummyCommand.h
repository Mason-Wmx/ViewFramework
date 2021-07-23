#pragma once
#include "stdafx.h"
#include "../NfdcAppCore/Application.h"
#include "../NfdcAppCore/ContinuousCommand.h"
#include <QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include "../jsoncpp/include/json/json.h" // for DummyCommandReadJSONMaterial

#include <QNetworkAccessManager> // for DummyCommandReadJSONMaterial
#include <QNetworkReply>         // for DummyCommandReadJSONMaterial

namespace SIM
{
	class DummyCommand :
		 public QObject, public ContinuousCommand
	{
		Q_OBJECT

	public:
		DummyCommand(Application& app);
		virtual ~DummyCommand(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		static const std::string Name;
	};

	class DummyCommandEdit :
		public QObject, public ContinuousCommand
	{
		Q_OBJECT

	public:
		DummyCommandEdit(Application& app);
		virtual ~DummyCommandEdit(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		static const std::string Name;
	};

	class DummyCommandSurfaceMode :
		public QObject, public ContinuousCommand
	{
		Q_OBJECT

	public:
		DummyCommandSurfaceMode(Application& app);
		virtual ~DummyCommandSurfaceMode(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Select Surface"); }
		std::string GetLargeIcon() { return ":/NfdcModDefine/images/Calculate_32x32.png"; }
		std::string GetSmallIcon() { return ":/NfdcModDefine/images/Calculate_16x16.png"; }

		static const std::string Name;
	};

	class DummyCommandSelectionFilter :
		public QObject, public ContinuousCommand
	{
		Q_OBJECT

	public:
		DummyCommandSelectionFilter(Application& app);
		virtual ~DummyCommandSelectionFilter(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Filter selection"); }
		std::string GetLargeIcon() { return ":/NfdcModDefine/images/Fixed_32x32.png"; }
		std::string GetSmallIcon() { return ":/NfdcModDefine/images/Fixed_16x16.png"; }

		static const std::string Name;
	};

    class DummyCommandBlockEmptySelection :
        public QObject, public ContinuousCommand
    {
        Q_OBJECT

    public:
        DummyCommandBlockEmptySelection(Application& app);
        virtual ~DummyCommandBlockEmptySelection(void);

        virtual bool Execute();

        virtual std::string GetUniqueName() { return DummyCommandBlockEmptySelection::Name; }

        virtual QString GetLabel() { return QString("Block empty selection"); }

        static constexpr char* Name = "DummyCommandBlockEmptySelection";
    };

    class DummyCommandFreezeSelection :
        public QObject, public ContinuousCommand
    {
        Q_OBJECT

    public:
        DummyCommandFreezeSelection(Application& app);
        virtual ~DummyCommandFreezeSelection(void);

        virtual bool Execute();

        virtual std::string GetUniqueName() { return DummyCommandFreezeSelection::Name; }

        virtual QString GetLabel() { return QString("Freeze selection"); }

        static constexpr char* Name = "DummyCommandFreezeSelection";
    };

	class DummyCommandVisibilityFilter :
		public QObject, public ContinuousCommand
	{
		Q_OBJECT

	public:
		DummyCommandVisibilityFilter(Application& app);
		virtual ~DummyCommandVisibilityFilter(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Filter visibility"); }
		std::string GetLargeIcon() { return ":/NfdcModDefine/images/generate_32x32.png"; }
		std::string GetSmallIcon() { return ":/NfdcModDefine/images/generate_16x16.png"; }

		static const std::string Name;
	};

	class DummyCommandControlsUnits :
		public QObject, public ContinuousCommand
	{
		Q_OBJECT

	public:
		DummyCommandControlsUnits(Application& app);
		virtual ~DummyCommandControlsUnits(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Controls with Units"); }

		static const std::string Name;
	};

	class UnitControls : public QObject
	{
		Q_OBJECT

	public:
			UnitControls(Application& app, QDialog* parent, QGridLayout* layout, QString name, double value, EUnitCategory category, int idx);
	};

  class DummyCommandMDSMaterial :
    public QObject, public DocCommand
  {
    Q_OBJECT

  public:

      DummyCommandMDSMaterial(Application& app);
    virtual ~DummyCommandMDSMaterial(void);

    virtual bool Execute();

    virtual std::string GetUniqueName();

    virtual QString GetLabel() { return QString("  MDS Materials  "); }

    static const std::string Name;
  };

	class ControlsUnitsDialog : public QDialog
	{
		Q_OBJECT
	public:
		ControlsUnitsDialog(Application& app, QWidget *parent = 0);
		~ControlsUnitsDialog();

		QDialogButtonBox* buttonBox;

	};

	class DummyCommandCreateOnSurface :
		public QObject, public Observer,  public ContinuousCommand
	{
		Q_OBJECT

	public:
		DummyCommandCreateOnSurface(Application& app);
		virtual ~DummyCommandCreateOnSurface(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Create on surface"); }
		std::string GetLargeIcon() { return ":/NfdcModDefine/images/materials_32x32.png"; }
		std::string GetSmallIcon() { return ":/NfdcModDefine/images/materials_16x16.png"; }

		static const std::string Name;

		// Inherited via Observer
		virtual void Notify(Event & ev) override;

	private:

		vtkSmartPointer<vtkActor> _actor;
	};

	class DummyCommandCreateOnSurfaceTop :
		public QObject, public Observer, public ContinuousCommand
	{
		Q_OBJECT

	public:
		DummyCommandCreateOnSurfaceTop(Application& app);
		virtual ~DummyCommandCreateOnSurfaceTop(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Create on surface top"); }
		std::string GetLargeIcon() { return ":/NfdcModDefine/images/materials_32x32.png"; }
		std::string GetSmallIcon() { return ":/NfdcModDefine/images/materials_16x16.png"; }

		static const std::string Name;

		// Inherited via Observer
		virtual void Notify(Event & ev) override;

	private:

		vtkSmartPointer<vtkActor> _actor;
	};

	class DummyCommandCreateGroupOnSurface :
		public DummyCommandCreateOnSurface
	{
	public:
		DummyCommandCreateGroupOnSurface(Application& app);
		virtual ~DummyCommandCreateGroupOnSurface(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Create group on surface"); }
		std::string GetLargeIcon() { return ":/NfdcModDefine/images/materials_32x32.png"; }
		std::string GetSmallIcon() { return ":/NfdcModDefine/images/materials_16x16.png"; }

		static const std::string Name;

		virtual void Notify(Event & ev) override;

	private:
		vtkSmartPointer<vtkActor> _actor;
	};

	class DummyCommandBaseDialgStyle :
		public QObject, public ContinuousCommand
	{
		Q_OBJECT

	public:
		DummyCommandBaseDialgStyle(Application& app);
		virtual ~DummyCommandBaseDialgStyle(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		static const std::string Name;
	};

	class DummyCommandGenerateRandomEdges : public DocCommand
	{
	public:
		DummyCommandGenerateRandomEdges(Application& app);
		virtual ~DummyCommandGenerateRandomEdges(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Generate edges"); }

		static const std::string Name;
	};

	class DummyCommandEdgeMode :
		public QObject, public ContinuousCommand
	{
		Q_OBJECT

	public:
		DummyCommandEdgeMode(Application& app);
		virtual ~DummyCommandEdgeMode(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Select Edges"); }

		static const std::string Name;
	};



    class DummyCommandGenerateRandomNodes : public DocCommand
    {
    public:
        DummyCommandGenerateRandomNodes(Application& app);
        virtual ~DummyCommandGenerateRandomNodes(void);

        virtual bool Execute();

        virtual std::string GetUniqueName();

        virtual QString GetLabel() { return QString("Generate nodes"); }

        static const std::string Name;
    };

    class DummyCommandNodeMode :
        public QObject, public ContinuousCommand
    {
        Q_OBJECT

    public:
        DummyCommandNodeMode(Application& app);
        virtual ~DummyCommandNodeMode(void);

        virtual bool Execute();

        virtual std::string GetUniqueName();

        virtual QString GetLabel() { return QString("Select Nodes"); }

        static const std::string Name;
    };

	class DummyCommandInfoCenter :
		public AppCommand
	{
	public:
		DummyCommandInfoCenter(Application& app);
		virtual ~DummyCommandInfoCenter(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Dummy InfoCenter"); }

		static const std::string Name;
	};

	class DummyKeepInCommand : public DocCommand
	{
	public:
		DummyKeepInCommand(Application& app);
		virtual ~DummyKeepInCommand(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Keep IN"); }

		static const std::string Name;
	};

	class DummyCommandGlyph :
		public QObject, public Observer, public ContinuousCommand
	{
		Q_OBJECT

	public:
		DummyCommandGlyph(Application& app);
		virtual ~DummyCommandGlyph(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("CreateGlyph"); }
		std::string GetLargeIcon() { return ":/NfdcModDefine/images/Calculate_32x32.png"; }
		std::string GetSmallIcon() { return ":/NfdcModDefine/images/Calculate_16x16.png"; }
		void Notify(Event & ev);

		static const std::string Name;

	private:

		vtkSmartPointer<vtkActor> _actor;
	};

	class GenerateFieldNormalsCommand : public DocCommand
	{
	public:
		GenerateFieldNormalsCommand(Application& app);
		virtual ~GenerateFieldNormalsCommand(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Fields normals"); }

		static const std::string Name;
	};

	class GeneratePointNormalsCommand : public DocCommand
	{
	public:
		GeneratePointNormalsCommand(Application& app);
		virtual ~GeneratePointNormalsCommand(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Points normals"); }

		static const std::string Name;
	};

	class GenerateEdgeNormalsCommand : public DocCommand
	{
	public:
		GenerateEdgeNormalsCommand(Application& app);
		virtual ~GenerateEdgeNormalsCommand(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Edges normals"); }

		static const std::string Name;
	};

	class GenerateNodeNormalsCommand : public DocCommand
	{
	public:
		GenerateNodeNormalsCommand(Application& app);
		virtual ~GenerateNodeNormalsCommand(void);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Nodes normals"); }

		static const std::string Name;
	};

	class NormalsCommand : public DocCommand
	{
	public:
		NormalsCommand(Application& app);
		virtual ~NormalsCommand(void);

		virtual bool Execute() { return false; };

		virtual std::string GetUniqueName();

		virtual QString GetLabel() { return QString("Normals"); }

		static const std::string Name;
	};


    class DebugMode6SocketsCommand : public AppCommand
    {
    public:
        DebugMode6SocketsCommand(Application& application) :AppCommand(application) {}
        ~DebugMode6SocketsCommand() {}

        virtual bool Execute();

        virtual std::string GetUniqueName() override;
        virtual std::string GetTooltipKey() override;
        virtual QString GetLabel() override;
        virtual QString GetHint() override;

        static const std::string Name;
    };


    class DummyProgressCommand : public ContinuousCommand
    {
    public:
        DummyProgressCommand(Application& app) :ContinuousCommand(app) {}
        virtual ~DummyProgressCommand(void) {};

        virtual bool Execute();

        virtual bool IsPermanent() { return true; }
        virtual std::string GetUniqueName() { return DummyProgressCommand::Name; }
        virtual bool SupportsMultiExecution() { return true; }

        virtual QString GetLabel() { return QString("Progress"); }

        virtual void OnDialogDestroy() { _dlg = nullptr; };

        static constexpr char* Name = "DummyProgressCommand";

    private:
        QWidget* _dlg = nullptr;
    };


    class DummyProgressDialog : public QWidget
    {
        Q_OBJECT

    public:
        DummyProgressDialog(Application& app, QWidget *btn, QWidget *parent = 0);
    protected:
        bool eventFilter(QObject *obj, QEvent *event);
        void showEvent(QShowEvent* event);
        void RefreshDialogPosition();

        private slots:
        void CloseDialog();


    private:
        Application& _App;
        QWidget* _btn;

        QPushButton* closeButton;

    };
}
