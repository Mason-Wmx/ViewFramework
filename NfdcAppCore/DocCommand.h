#pragma once

#include "stdafx.h"
#include "export.h"
#include "Command.h"
#include "AppCommand.h"
#include "Application.h"
#include "Document.h"
#include <vector>

namespace SIM
{
	class DeleteRequest;
	class DeleteStrategy;
	class NFDCAPPCORE_EXPORT DocCommand:public AppCommand
	{
	public:
		DocCommand(Application& application);
		DocCommand(Document& document);
		
		~DocCommand(void);		
		Document* GetDocument();
		virtual bool Execute()=0;
		virtual bool Execute(Document* document);
	protected: 
		Document* _document;
	};

	class NFDCAPPCORE_EXPORT UndoCommand:public DocCommand
	{
	public:
		UndoCommand(Application& application);
		
		virtual bool Execute();
		
		virtual std::string GetUniqueName();

		static const std::string Name;
	};

	class NFDCAPPCORE_EXPORT RedoCommand :public DocCommand
	{
	public:
		RedoCommand(Application& application);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		static const std::string Name;
	};

	class NFDCAPPCORE_EXPORT EditCommand : public DocCommand
	{
	public:
		EditCommand(Application& application);

		virtual bool Execute();

		virtual std::string GetUniqueName();

		void RegisterCommand(const std::string& typeName, std::shared_ptr<CommandBase> command);

		static const std::string Name;
	private:
		std::map<std::string, std::shared_ptr<CommandBase>> _commands;
	};

	class NFDCAPPCORE_EXPORT DeleteCommand : public DocCommand
	{
	public:
		DeleteCommand(Application& application);

        std::string GetLargeIcon() { return ""; }

        std::string GetSmallIcon() { return ""; }

		virtual QString GetLabel() override { return QObject::tr("Delete"); }

		virtual bool Execute();

		virtual std::string GetUniqueName();

		void RegisterStrategy(const std::string& type, const std::shared_ptr<DeleteStrategy>& strategy);

		static const std::string Name;

	private:
		typedef std::map<std::string, std::vector<std::shared_ptr<DeleteStrategy>>> mapTypeToStrategies;
		mapTypeToStrategies _strategies;
	};

    class NFDCAPPCORE_EXPORT SelectionModeCommand : public DocCommand
    {
    public:
        SelectionModeCommand(Application& application, const std::string& context, QString& label);

        virtual QString GetLabel() override { return _label; }

        virtual bool Execute();

		std::string GetSelectionContext() { return _context; }
    private:
        std::string _context;
        QString _label;
    };

    class NFDCAPPCORE_EXPORT GeneralSelectionModeCommand : public SelectionModeCommand
    {
    public:
        GeneralSelectionModeCommand(Application& application) :SelectionModeCommand(application, SelectionContext::Normal, QObject::tr("Select")) {}
        
		std::string GetLargeIcon() { return ":/NfdcAppCore/images/selection_32x32.png"; }

		std::string GetSmallIcon() { return ":/NfdcAppCore/images/selection_16x16.png"; }

        virtual std::string GetUniqueName() { return GeneralSelectionModeCommand::Name; }

        static const std::string Name;
    };

    class NFDCAPPCORE_EXPORT BodySelectionModeCommand : public SelectionModeCommand
    {
    public:
        BodySelectionModeCommand(Application& application) :SelectionModeCommand(application, SelectionContext::Body, QObject::tr("Select Body")) {}

        std::string GetLargeIcon() { return ":/NfdcAppCore/images/selection_body_32x32.png"; }

        std::string GetSmallIcon() { return ":/NfdcAppCore/images/selection_body_16x16.png"; }

        virtual std::string GetUniqueName() { return BodySelectionModeCommand::Name; }

        static const std::string Name;
    };

    class NFDCAPPCORE_EXPORT SurfaceSelectionModeCommand : public SelectionModeCommand
    {
    public:
        SurfaceSelectionModeCommand(Application& application) :SelectionModeCommand(application, SelectionContext::Surface, QObject::tr("Select Surface")) {}

        std::string GetLargeIcon() { return ":/NfdcAppCore/images/selection_surface_32x32.png"; }

        std::string GetSmallIcon() { return ":/NfdcAppCore/images/selection_surface_16x16.png"; }

        virtual std::string GetUniqueName() { return SurfaceSelectionModeCommand::Name; }

        static const std::string Name;
    };

    class NFDCAPPCORE_EXPORT EdgeSelectionModeCommand : public SelectionModeCommand
    {
    public:
        EdgeSelectionModeCommand(Application& application) :SelectionModeCommand(application, SelectionContext::Edge, QObject::tr("Select Edge")) {}

        std::string GetLargeIcon() { return ":/NfdcAppCore/images/selection_edge_32x32.png"; }

        std::string GetSmallIcon() { return ":/NfdcAppCore/images/selection_edge_16x16.png"; }

        virtual std::string GetUniqueName() { return EdgeSelectionModeCommand::Name; }

        static const std::string Name;
    };

	class NFDCAPPCORE_EXPORT GeometrySelectionModeCommand : public SelectionModeCommand
	{
	public:
		GeometrySelectionModeCommand(Application& application) :SelectionModeCommand(application, SelectionContext::Geometry, QObject::tr("Select Geometry")) {}

		std::string GetLargeIcon() { return ":/NfdcAppCore/images/selection_32x32.png"; }

		std::string GetSmallIcon() { return ":/NfdcAppCore/images/selection_16x16.png"; }

		virtual std::string GetUniqueName() { return GeometrySelectionModeCommand::Name; }

		static const std::string Name;
	};

    class NFDCAPPCORE_EXPORT NodeSelectionModeCommand : public SelectionModeCommand
    {
    public:
        NodeSelectionModeCommand(Application& application) :SelectionModeCommand(application, SelectionContext::Node, QObject::tr("Select Vertex")) {}

        std::string GetLargeIcon() { return ":/NfdcAppCore/images/selection_node_32x32.png"; }

        std::string GetSmallIcon() { return ":/NfdcAppCore/images/selection_node_16x16.png"; }

        virtual std::string GetUniqueName() { return NodeSelectionModeCommand::Name; }

        static const std::string Name;
    };

	class NFDCAPPCORE_EXPORT HideSelectionCommand : public DocCommand
	{
	public:
		HideSelectionCommand(Application& application) :DocCommand(application) {}

		virtual QString GetLabel() override { return QObject::tr("Hide"); }

		virtual bool Execute();

		virtual std::string GetUniqueName() { return HideSelectionCommand::Name; }
        virtual std::string GetLargeIcon() { return ""; }
        virtual std::string GetSmallIcon() { return ""; }

		static constexpr const char* Name = "HideSelectionCommand";
	};

    class NFDCAPPCORE_EXPORT ShowSelectionCommand : public DocCommand
    {
    public:
        ShowSelectionCommand(Application& application) :DocCommand(application) {}

        virtual QString GetLabel() override { return QObject::tr("Show"); }

        virtual bool Execute();

        virtual std::string GetUniqueName() { return ShowSelectionCommand::Name; }
        virtual std::string GetLargeIcon() { return ""; }
        virtual std::string GetSmallIcon() { return ""; }

        static constexpr const char* Name = "ShowSelectionCommand";
    };

    class NFDCAPPCORE_EXPORT ToggleSelectionVisibilityCommand: public DocCommand
    {
    public:
        ToggleSelectionVisibilityCommand(Application& application) :DocCommand(application) {}

        virtual QString GetLabel() override { return QObject::tr("Toggle visibility"); }

        virtual bool Execute();

        virtual std::string GetUniqueName() { return ToggleSelectionVisibilityCommand::Name; }
        virtual std::string GetLargeIcon() { return ""; }
        virtual std::string GetSmallIcon() { return ""; }

        static constexpr const char* Name = "ToggleSelectionVisibilityCommand";
    };

	class NFDCAPPCORE_EXPORT ResetVisibilityCommand : public DocCommand
	{
	public:
		ResetVisibilityCommand(Application& application) :DocCommand(application) {}

		virtual QString GetLabel() override { return QObject::tr("Reset selection"); }

		virtual bool Execute();

		virtual std::string GetUniqueName() { return ResetVisibilityCommand::Name; }

		static constexpr const char* Name = "ResetVisibilityCommand";
	};

    class NFDCAPPCORE_EXPORT SyncCommand : public DocCommand
    {
    public:
        SyncCommand(Application& application) :DocCommand(application) {}

        virtual QString GetLabel() override { return QObject::tr("Synchronize"); }

        virtual bool Execute();

        virtual std::string GetUniqueName() { return SyncCommand::Name; }

        static constexpr const char* Name = "SynchronizeScalarisCommand";
    };
}