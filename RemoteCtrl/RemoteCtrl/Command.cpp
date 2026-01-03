#include "pch.h"
#include "Command.h"

CCommand::CCommand()
{
	 struct {
		int nCmd;
		CMDFUNC pCmdFunc;
	}data[] = {
		{1,&CCommand::MakeDriverInfo},
		{2,&CCommand::MakeDirectoryInfo},
		{3,&CCommand::RunFile},
		{4,&CCommand::DownloadFile},
		{5,&CCommand::SendScreen},
		{6,&CCommand::LockMachine},
		{7,&CCommand::UnlockMachine},
		{8,&CCommand::DeleteFileByCommand},
		{9,&CCommand::MouseEvent},
		{1981,&CCommand::TestConnect},
		{-1,NULL}
	};
	
	for (int i = 0; data[i].nCmd != -1; i++) {
		m_mapFunction.insert(std::make_pair(data[i].nCmd, data[i].pCmdFunc));
	}

}
