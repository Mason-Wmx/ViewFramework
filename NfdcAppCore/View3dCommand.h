#pragma once

#include "stdafx.h"
#include "export.h"
#include "DocCommand.h"

namespace SIM
{
	class NFDCAPPCORE_EXPORT View3dCommand : public DocCommand
	{
	public:
		View3dCommand(Application& application, vtkExtAutoCAM::CommandID command);
		~View3dCommand(void);

		virtual bool Execute(void) override;
		virtual std::string GetUniqueName() override;
		static const std::string Name;

	private:
		vtkExtAutoCAM::CommandID _command; 
	};


	class NFDCAPPCORE_EXPORT View3dHomeCommand : public View3dCommand
	{
	public:
		View3dHomeCommand(Application& application) :
			View3dCommand(application, vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_HOME)
		{}

		virtual std::string GetUniqueName() override;
		static const std::string Name;
	};


	class NFDCAPPCORE_EXPORT View3dTopCommand : public View3dCommand
	{
	public:
		View3dTopCommand(Application& application) :
			View3dCommand(application, vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ROTATE_TO_TOP)
		{}

		virtual std::string GetUniqueName() override;
		static const std::string Name;
	};


	class NFDCAPPCORE_EXPORT View3dFrontCommand : public View3dCommand
	{
	public:
		View3dFrontCommand(Application& application) :
			View3dCommand(application, vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ROTATE_TO_FRONT)
		{}

		virtual std::string GetUniqueName() override;
		static const std::string Name;
	};


	class NFDCAPPCORE_EXPORT View3dRightCommand : public View3dCommand
	{
	public:
		View3dRightCommand(Application& application) :
			View3dCommand(application, vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ROTATE_TO_RIGHT)
		{}

		virtual std::string GetUniqueName() override;
		static const std::string Name;
	};

    class NFDCAPPCORE_EXPORT View3dZoomFitCommand : public View3dCommand
    {
    public:
        View3dZoomFitCommand(Application& application) :
            View3dCommand(application, vtkExtAutoCAM::CommandID::VTKAUTOCAM_CMD_ZOOM_FIT)
        {}

        virtual std::string GetUniqueName() override;
        static const std::string Name;
    };
}