#pragma once

namespace SIM
{
    class CommandBase;
	class DeleteStrategy;

    class ICommandRegistry
    {
    public:
        virtual ~ICommandRegistry() {}

        virtual void RegisterCommand(std::shared_ptr<CommandBase> cmd) = 0;
		virtual void RegisterEditCommand(const std::string & typeName, std::shared_ptr<CommandBase> cmd) = 0;
		virtual void RegisterDeleteStrategy(const std::string& type, const std::shared_ptr<DeleteStrategy>& stragety) = 0;
    };
}
