#include "stdafx.h"
#include "..\header\MainApp.h"

Client::MainApp::MainApp()
{
}

Client::MainApp::~MainApp()
{

}

HRESULT Client::MainApp::NativeConstruct()
{
	return S_OK;
}

Engine::_int Client::MainApp::Tick(const _double _deltaTime)
{
	return 0;
}

Engine::_int Client::MainApp::Process(const _double _deltaTime)
{
	return 0;
}

HRESULT Client::MainApp::Render()
{
	return S_OK;
}

Client::MainApp* Client::MainApp::Create()
{
	MainApp* pInstance = new MainApp;
	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("MainApp Create Error");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void Client::MainApp::Free()
{

}
