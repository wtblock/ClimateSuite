/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "miniz.h"
#include <vector>

class CZipReader
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
	CZipReader();
	~CZipReader();

	bool Open(const CString& path);
	bool ExtractFile(const CString& internalPath, std::vector<uint8_t>& outData);
	bool Close();

private:
	mz_zip_archive m_zip;
	bool m_bIsOpen;
}; // CZipReader

/////////////////////////////////////////////////////////////////////////////
