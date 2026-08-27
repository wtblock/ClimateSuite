/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026, by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "PageContent.h"

void CPageContent::WriteXml(IXmlWriter* pWriter, int /*nPage*/, int /*nItem*/)
{
	// <Content Type="Graph"> ... </Content>
	// Placeholder — no-op
}

void CPageContent::ReadXml(IXmlReader* pReader)
{
	// Placeholder — no-op
}
