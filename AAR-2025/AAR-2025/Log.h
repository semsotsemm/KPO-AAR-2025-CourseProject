#pragma once
#include "In.h"
#include "Parm.h"
#include "Error.h"

#define MAX_LEN_MESSAGE 300

namespace Log
{
	struct LOG
	{
		wchar_t logfile[PARM_MAX_SIZE];
		std::ofstream* stream;

		LOG();
	};

	LOG getlog(wchar_t logfile[]);									

	void WriteLine(std::ostream* stream, const char* c, ...);		
	void WriteLine(std::ostream* stream, const wchar_t* c, ...);	
	void WriteLog(std::ostream* stream);							
	void WriteParm(std::ostream* stream, Parm::PARM parm);			
	void WriteIn(std::ostream* stream, In::IN in);					
	void WriteError(std::ostream* stream, Error::ERROR error);		

	void Close(LOG log);											
}
