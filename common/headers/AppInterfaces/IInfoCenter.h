#pragma once

#include "stdafx.h"

namespace SIM
{
	class InfoCenterItem;
	class IInfoCenter
	{
	public:
		virtual ~IInfoCenter() {}

		virtual bool AddItem(std::shared_ptr<InfoCenterItem> item) = 0;
	};
}