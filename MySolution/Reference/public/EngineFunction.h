#pragma once

namespace Engine {
	template<typename T>
	void	SafeDelete(T& _Pointer)
	{
		if (nullptr != _Pointer)
		{
			delete _Pointer;
			_Pointer = nullptr;
		}
	}

	template<typename T>
	void	SafeDeleteArray(T& _ArrayPointer)
	{
		if (nullptr != _ArrayPointer)
		{
			delete[] _ArrayPointer;
			_ArrayPointer = nullptr;
		}
	}

	template<typename T>
	unsigned long SafeRelease(T& _Instance)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != _Instance)
		{
			dwRefCnt = _Instance->Release();

			if (0 == dwRefCnt)
				_Instance = nullptr;
		}

		return dwRefCnt;
	}

	class TagFinder {
	public:
		TagFinder(const wchar_t* _tag) : targetTag(_tag) { };
		~TagFinder() {};

	public:
		template<typename T>
		bool	operator()(const T& pair)
		{
			if (0 == lstrcat(targetTag.pair.first))
				return TRUE;
			return FALSE;
		}

	private:
		const wchar_t*			targetTag = nullptr;
	};
}