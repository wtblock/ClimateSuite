/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#pragma once
#include "miniz.h"

/////////////////////////////////////////////////////////////////////////////
class CZipWriter
{
// public properties
public:
	// true if a file is open
	bool GetIsOpen()
	{
		return m_bIsOpen;
	}
	// true if a file is open
	void SetIsOpen(bool value)
	{
		m_bIsOpen = value;
	}
	// true if a file is open
	__declspec(property(get = GetIsOpen, put = SetIsOpen))
		bool IsOpen;

public:
	CZipWriter();
	~CZipWriter();

	bool Create(const CString& path);
	bool AddFile(const CString& internalPath, const void* data, size_t size);
	bool Close();

private:
	mz_zip_archive m_zip;
	bool m_bIsOpen;

}; // CZipWriter

/////////////////////////////////////////////////////////////////////////////
