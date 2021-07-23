#pragma once
#include "stdafx.h"

namespace SIM
{
	class SetRibbonItemResult
	{
	public:
		static const int OK = 0;
		static const int InvalidPosition = 1;
		static const int PositionNotAvailable = 2;
		static const int InvalidName = 3;
		static const int NameExists = 4;
	};

	//-------------------------

	template <class T> class RibbonItems
	{
	public:
		int SetItem(int position, const std::string & name, const T & item);

		std::string GetName(int position);
		int GetPosition(const std::string & name);
		T* GetItem(const std::string & name);
		
		const std::map<int, std::string> & Names(void) const;

	private:
		std::map<int, std::string> _mapPositionToName;
		std::map<std::string, T> _mapNameToItem;
	};

	//-------------------------

	template<class T> int SIM::RibbonItems<T>::SetItem(int position, const std::string & name, const T & item)
	{
		if (position < 0)
			return SetRibbonItemResult::InvalidPosition;

		if (GetName(position).length() > 0)
			return SetRibbonItemResult::PositionNotAvailable;

		if (name.length() == 0)
			return SetRibbonItemResult::InvalidName;

		if (GetItem(name))
			return SetRibbonItemResult::NameExists;

		_mapNameToItem.insert(std::pair<std::string, T>(name, item));
		_mapPositionToName.insert(std::pair<int, std::string>(position, name));

		return SetRibbonItemResult::OK;
	}

	//-------------------------

	template<class T> std::string SIM::RibbonItems<T>::GetName(int position) 
	{
		auto it = _mapPositionToName.find(position);
		if (it != _mapPositionToName.end())
		{
			return it->second;
		}
		else
		{
			return std::string();
		}
	}

	//-------------------------

	template<class T> int SIM::RibbonItems<T>::GetPosition(const std::string & name)
	{
		for (auto it : _mapPositionToName)
		{
			if (it.second == name)
			{
				return it.first;
			}
		}

		return -1;
	}

	//-------------------------

	template<class T> T* SIM::RibbonItems<T>::GetItem(const std::string & name) 
	{
		auto it = _mapNameToItem.find(name);
		if (it != _mapNameToItem.end())
		{
			return &(it->second);
		}
		else
		{
			return NULL;
		}
	}

	//-------------------------

	template<class T> const std::map<int, std::string> & SIM::RibbonItems<T>::Names(void) const
	{
		return _mapPositionToName;
	}

	//-------------------------
}