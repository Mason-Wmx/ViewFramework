#pragma once

namespace SIM
{
    class ViewFactory;
    class View3D;

    class IViewFactoryRegistry
    {
    public:
        virtual ~IViewFactoryRegistry() {}

        virtual View3D & GetView3D() = 0;
        virtual void RegisterViewFactory(std::shared_ptr<ViewFactory> factory) = 0;
    };
}
