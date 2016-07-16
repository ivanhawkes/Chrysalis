#pragma once


class CConsoleCommands
{
public:
	void Register();
	void Unregister();

private:

	/**
	Attaches the currently player to an entity.

	\param [in,out]	pConsoleCommandArgs	If non-null, the console command arguments.
	*/
	static void OnAttach(IConsoleCmdArgs* pConsoleCommandArgs);


	/**
	Requests a new ObjectId to be created, and output to the log.
	
	\param [in,out]	pConsoleCommandArgs	If non-null, the console command arguments.
	*/
	static void OnCreateObjectId(IConsoleCmdArgs* pConsoleCommandArgs);	
};