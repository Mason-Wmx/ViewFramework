#pragma once
#include "stdafx.h"
namespace SIM
{
    template <class T>
    struct MVCEncodeBaseType
    {
    };

	template <class T_Model,class T_View, class T_Controller>
	class MVCBase
	{
	public:
        template<class T_MVCBase>
		MVCBase(MVCEncodeBaseType<T_MVCBase>, bool uiLess)
            : _model(*static_cast<T_MVCBase*>(this)),
              _view(nullptr),
              _controller(*static_cast<T_MVCBase*>(this)),
			  _uiLess(uiLess)              
        {
			if (!uiLess)
			{
				_view = std::make_unique<T_View>(*static_cast<T_MVCBase*>(this));
			}
        }

		template<class T_MVCBase, class T_Param>
		MVCBase(MVCEncodeBaseType<T_MVCBase>, T_Param* object, bool uiLess)
            : _model(*static_cast<T_MVCBase*>(this)),
              _view(nullptr),
              _controller(*static_cast<T_MVCBase*>(this)),
			 _uiLess(uiLess)              
        {
			if (!uiLess && object)
			{
				_view = std::unique_ptr<T_View>(new T_View(*static_cast<T_MVCBase*>(this), *object));
			}
        }
		
		T_Model& GetModel()
        {
            return _model;
        }

		T_View& GetView()
        {
			if (_uiLess)
				throw std::exception("MVC Component run in UILess mode. View is not initialized.");

            return *_view;
        }

		T_Controller& GetController()
        { 
            return _controller;
        }

		bool IsUILess() const{
			return _uiLess;
		}

	protected:
		T_Model _model;
		std::unique_ptr<T_View> _view;
		T_Controller _controller;
		bool _uiLess;
	};

	template <class T_Parent>
	class MVCItem
	{
	public:
		MVCItem(T_Parent& parent):_parent(parent){}
		~MVCItem(void){}
		T_Parent& GetParent(){return _parent;}
	protected:
		T_Parent& _parent;
	};
}

