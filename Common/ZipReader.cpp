/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ZipReader.h"

/////////////////////////////////////////////////////////////////////////////
CZipReader::CZipReader()
{
	memset(&m_zip, 0, sizeof(m_zip));
	IsOpen = false;
} // CZipReader

/////////////////////////////////////////////////////////////////////////////
CZipReader::~CZipReader()
{
	if (IsOpen)
		Close();
} // ~CZipReader

/////////////////////////////////////////////////////////////////////////////
bool CZipReader::Open(const CString& path)
{
	memset(&m_zip, 0, sizeof(m_zip));

	CT2A utf8(path, CP_UTF8);

	if (!mz_zip_reader_init_file(&m_zip, utf8, 0))
		return false;

	IsOpen = true;
	return true;
} // Open

/////////////////////////////////////////////////////////////////////////////
bool CZipReader::ExtractFile(const CString& internalPath, std::vector<uint8_t>& outData)
{
	if (!IsOpen)
		return false;

	CT2A utf8(internalPath, CP_UTF8);

	int index = mz_zip_reader_locate_file(&m_zip, utf8, nullptr, 0);
	if (index < 0)
		return false;

	size_t size = 0;
	void* p = mz_zip_reader_extract_to_heap(&m_zip, index, &size, 0);
	if (!p)
		return false;

	outData.resize(size);
	memcpy(outData.data(), p, size);

	mz_free(p);
	return true;
} // ExtractFile

/////////////////////////////////////////////////////////////////////////////
bool CZipReader::Close()
{
	if (!IsOpen)
		return false;

	mz_zip_reader_end(&m_zip);
	IsOpen = false;
	return true;
} // Close

/////////////////////////////////////////////////////////////////////////////
