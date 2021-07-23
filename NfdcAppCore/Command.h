#pragma once
#include "stdafx.h"
#include "export.h"
#include "qstring.h"
#include "qobject.h"

namespace SIM
{
	class NFDCAPPCORE_EXPORT Command : public CommandBase
	{
	public:
		Command();
		virtual ~Command(void);
		
		virtual QString GetLabel(){ return QString(GetUniqueName().c_str()); }
		virtual QString GetHint() { return QString(GetUniqueName().c_str()); }
		virtual std::string GetLargeIcon() { return ":/NfdcAppCore/images/new.png"; }
		virtual std::string GetSmallIcon() { return ":/NfdcAppCore/images/new-16x16.png"; }
		virtual bool IsEnabled() { return true; }
		virtual QKeySequence GetKeySequence() { return QKeySequence::UnknownKey; }
		virtual bool IsSensitiveToEvent(Event&){return false;}	
		virtual std::string GetTooltipKey() { return GetUniqueName(); }
	};

}