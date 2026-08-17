/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "miniz.h"
#include <vector>

class CZipReader
{
public:
	CZipReader();
	~CZipReader();

	bool Open(const CString& path);
	bool ExtractFile(const CString& internalPath, std::vector<uint8_t>& outData);
	bool Close();

private:
	mz_zip_archive m_zip;
	bool m_initialized;
}; // CZipReader

/////////////////////////////////////////////////////////////////////////////
