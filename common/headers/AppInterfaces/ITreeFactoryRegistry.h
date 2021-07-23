#pragma once

namespace SIM
{
	class BrowserTree;
	class TreeFactory;

    class ITreeFactoryRegistry
    {
    public:
        virtual ~ITreeFactoryRegistry() {}

		virtual BrowserTree & GetBrowserTree(void) = 0;
		virtual void RegisterTreeFactory(std::shared_ptr<TreeFactory> factory) = 0;
	};
}
