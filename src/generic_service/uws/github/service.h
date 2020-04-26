// libuvdemo.h : Include file for standard system include files,
// or project specific include files.

#pragma once


// TODO: Reference additional headers your program requires here.
namespace Github {

	class CGithubService{
	public:
		SINGLETON_INSTANCE(CGithubService)
	};
}