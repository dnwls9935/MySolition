#pragma once

namespace Engine{

#define BEGIN(NAMESPACE) namespace NAMESPACE {
#define END }

#define USING(CLASSNAME) using namespace CLASSNAME;

#ifdef ENGINE_EXPORTS
#define DLL_MODE _declspec(dllexport)
#else
#define DLL_MODE _declspec(dllimport)
#endif


#define MSGBOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("Error"), MB_OK);


#define		NO_COPY(CLASSNAME)										\
			private:												\
			CLASSNAME(const CLASSNAME&);							\
			CLASSNAME& operator = (const CLASSNAME&);				\


#define		DECLARE_SINGLETON(CLASSNAME)							\
			NO_COPY(CLASSNAME)										\
			private:												\
			static	CLASSNAME*		instance;						\
			public:													\
			static	CLASSNAME*		GetInstance();					\
			static	void			DestroyInstance();

#define		IMPLEMET_SINGLETON(CLASSNAME)							\
			CLASSNAME*		CLASSNAME::instance = NULL;				\
			CLASSNAME*		CLASSNAME::GetInstance()	{			\
				if(NULL == instance){								\
					instance = new CLASSNAME;						\
				}													\
				return instance;									\
			}														\
			void	CLASSNAME::DestroyInstance()		{			\
				if(NULL != instance) {								\
					delete instance;								\
					instance = NULL;								\
				}													\
			}	
}													