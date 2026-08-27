/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ZipWriter.h"

/////////////////////////////////////////////////////////////////////////////
CZipWriter::CZipWriter()
{
	memset(&m_zip, 0, sizeof(m_zip));
	IsOpen = false;
} // CZipWriter

/////////////////////////////////////////////////////////////////////////////
CZipWriter::~CZipWriter()
{
	if (IsOpen)
		Close();
} // ~CZipWriter

/////////////////////////////////////////////////////////////////////////////
bool CZipWriter::Create(const CString& path)
{
	memset(&m_zip, 0, sizeof(m_zip));

	// Convert CString → UTF‑8
	CT2A utf8(path, CP_UTF8);

	if (!mz_zip_writer_init_file(&m_zip, utf8, 0))
		return false;

	IsOpen = true;
	return true;
} // Create

/////////////////////////////////////////////////////////////////////////////
bool CZipWriter::AddFile(const CString& internalPath, const void* data, size_t size)
{
	if (!IsOpen)
		return false;

	CT2A utf8(internalPath, CP_UTF8);

	if (!mz_zip_writer_add_mem(&m_zip, utf8, data, size, MZ_BEST_COMPRESSION))
		return false;

	return true;
} // AddFile

/////////////////////////////////////////////////////////////////////////////
bool CZipWriter::Close()
{
	if (!IsOpen)
		return false;

	mz_zip_writer_finalize_archive(&m_zip);
	mz_zip_writer_end(&m_zip);

	IsOpen = false;
	return true;
} // Close

/////////////////////////////////////////////////////////////////////////////
