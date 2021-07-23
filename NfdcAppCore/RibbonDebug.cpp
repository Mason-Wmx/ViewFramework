#include "stdafx.h"
#include "RibbonDebug.h"
#include "DocCommand.h"
#include "ApplyQssCommand.h"
#include "FileIOCommands.h"
#include "InternalTools.h"

using namespace SIM;

void SIM::RibbonDebug::AddRibbonContent(IRibbon* pRibbon)
{
	std::string bar = "DEBUG";
	pRibbon->SetBar(30, bar, "Debug");

	int pos = 0;

	std::string grp = "ACTION";
	pos += 10;
	if (pRibbon->SetGroup(bar, pos, grp, "Action") == 0)
	{
        pRibbon->AddCommand(bar, grp, OpenFileCommand::Name);
		pRibbon->AddCommand(bar, grp, UndoCommand::Name);
		pRibbon->AddCommand(bar, grp, RedoCommand::Name);
		pRibbon->AddCommand(bar, grp, EditCommand::Name);
        pRibbon->AddCommand(bar, grp, ApplyQssCommand::Name);
        pRibbon->AddCommand(bar, grp, TestCERCommand::Name);
        pRibbon->AddCommand(bar, grp, ShowBearerTokenCommand::Name);
        pRibbon->AddCommand(bar, grp, UploadProjectCommand::Name);
        pRibbon->AddCommand(bar, grp, SetViewColorCommand::Name);
        pRibbon->AddCommand(bar, grp, ResetVisibilityCommand::Name);
        pRibbon->AddCommand(bar, grp, SyncCommand::Name);
  }
}
