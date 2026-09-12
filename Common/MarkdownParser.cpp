/////////////////////////////////////////////////////////////////////////////
// Copyright © 2026 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "MarkdownParser.h"
#include "MarkdownRenderer.h"
#include "MarkdownBitmapRenderer.h"

/////////////////////////////////////////////////////////////////////////////
// ResolveConflict (span types)
//
// Determines whether a span should be downgraded based on the current block.
// Step 84: Fully populated span conflict rules.
/////////////////////////////////////////////////////////////////////////////
CMarkdownParser::ConflictRule CMarkdownParser::ResolveConflict
(
	MD_BLOCKTYPE eBlock,
	MD_SPANTYPE  eSpan
)
{
	//
	// Rule 1: Inside code blocks, ALL spans downgrade.
	//
	if (eBlock == MD_BLOCK_CODE)
		return ConflictRule_Downgrade;

	//
	// Rule 2: Inside table cells, links downgrade.
	//
	if (eBlock == MD_BLOCK_TD && eSpan == MD_SPAN_A)
		return ConflictRule_Downgrade;

	//
	// Rule 3: Inside blockquotes, inline code downgrades.
	//
	if (eBlock == MD_BLOCK_QUOTE && eSpan == MD_SPAN_CODE)
		return ConflictRule_Downgrade;

	//
	// Default: allow all other spans.
	//
	return ConflictRule_Allow;
} // ResolveConflict

/////////////////////////////////////////////////////////////////////////////
// ResolveTextConflict (text types)
//
// Determines whether a text fragment should be downgraded based on the block.
// Step 84: Fully populated text conflict rules.
/////////////////////////////////////////////////////////////////////////////
CMarkdownParser::ConflictRule CMarkdownParser::ResolveTextConflict
(
	MD_BLOCKTYPE eBlock,
	MD_TEXTTYPE  eText
)
{
	//
	// Rule 1: Inside headings, HTML text downgrades.
	//
	if (eBlock == MD_BLOCK_H && eText == MD_TEXT_HTML)
		return ConflictRule_Downgrade;

	//
	// Rule 2: Inside code blocks, HTML text downgrades.
	// (md4c already treats code blocks specially, but this keeps behavior consistent.)
	//
	if (eBlock == MD_BLOCK_CODE && eText == MD_TEXT_HTML)
		return ConflictRule_Downgrade;

	//
	// Rule 3: Inside blockquotes, HTML text downgrades.
	//
	if (eBlock == MD_BLOCK_QUOTE && eText == MD_TEXT_HTML)
		return ConflictRule_Downgrade;

	//
	// Default: allow all other text types.
	//
	return ConflictRule_Allow;
} // ResolveTextConflict

/////////////////////////////////////////////////////////////////////////////
// Parse
//
// Executes the md4c parser on the current Markdown text.
// Dispatches block/span/text events to the renderer stored in m_pRenderer.
//
// Returns:
//   true  = parse succeeded
//   false = parse failed
/////////////////////////////////////////////////////////////////////////////
bool CMarkdownParser::Parse()
{
	// renderer must be assigned
	if (Renderer == nullptr)
		return false;

	// markdown must not be empty
	if (Markdown.IsEmpty())
		return false;

	// convert CString → UTF‑8
	CT2A utf8Text(Markdown, CP_UTF8);
	const char* pszText = utf8Text;

	// length in bytes
	size_t nLength = strlen(pszText);

	// build parser structure
	MD_PARSER parser;
	parser.abi_version = 0;
	parser.flags = 0;

	parser.enter_block = &CMarkdownParser::EnterBlockCallback;
	parser.leave_block = &CMarkdownParser::LeaveBlockCallback;

	parser.enter_span = &CMarkdownParser::EnterSpanCallback;
	parser.leave_span = &CMarkdownParser::LeaveSpanCallback;

	parser.text = &CMarkdownParser::TextCallback;

	parser.debug_log = NULL;
	parser.syntax = NULL;

	// call md4c
	int nResult = md_parse
	(
		pszText,
		MD_SIZE(nLength),
		&parser,
		this
	);

	return (nResult == 0);
} // Parse

/////////////////////////////////////////////////////////////////////////////
// EnterBlockCallback
/////////////////////////////////////////////////////////////////////////////
int CMarkdownParser::EnterBlockCallback
(
	const MD_BLOCKTYPE type,
	void* detail,
	void* userdata
)
{
	CMarkdownParser* pParser =
		static_cast<CMarkdownParser*>(userdata);

	if (!pParser || !pParser->Renderer)
		return -1;

	CMarkdownRenderer* pRenderer =
		pParser->Renderer.get();

	// Track current block
	pParser->CurrentBlock = type;

	switch (type)
	{
	case MD_BLOCK_HR:
	{
		pRenderer->OnHorizontalRule();
		break;
	}

	case MD_BLOCK_P:
	{
		// paragraph start
		pRenderer->OnParagraphStart();
		break;
	}

	case MD_BLOCK_H:
	{
		MD_BLOCK_H_DETAIL* pDetail =
			static_cast<MD_BLOCK_H_DETAIL*>(detail);

		pRenderer->OnHeadingStart(pDetail->level);
		break;
	}

	case MD_BLOCK_UL:
	{
		pRenderer->OnUnorderedListStart();
		break;
	}

	case MD_BLOCK_OL:
	{
		pRenderer->OnOrderedListStart();
		break;
	}

	case MD_BLOCK_LI:
	{
		pRenderer->OnListItemStart();
		break;
	}

	case MD_BLOCK_QUOTE:
	{
		pRenderer->OnBlockQuoteStart();
		break;
	}

	case MD_BLOCK_CODE:
	{
		pRenderer->OnCodeBlockStart();
		break;
	}

	default:
	{
		// ignore unsupported block types
		break;
	}
	}

	return 0;
} // EnterBlockCallback

/////////////////////////////////////////////////////////////////////////////
// LeaveBlockCallback
/////////////////////////////////////////////////////////////////////////////
int CMarkdownParser::LeaveBlockCallback
(
	const MD_BLOCKTYPE type,
	void* detail,
	void* userdata
)
{
	CMarkdownParser* pParser =
		static_cast<CMarkdownParser*>(userdata);

	if (!pParser || !pParser->Renderer)
		return -1;

	CMarkdownRenderer* pRenderer =
		pParser->Renderer.get();

	// Track current block
	pParser->CurrentBlock = type;

	switch (type)
	{
	case MD_BLOCK_P:
	{
		pRenderer->OnParagraphEnd();
		break;
	}

	case MD_BLOCK_H:
	{
		pRenderer->OnHeadingEnd();
		break;
	}

	case MD_BLOCK_UL:
	{
		pRenderer->OnUnorderedListEnd();
		break;
	}

	case MD_BLOCK_OL:
	{
		pRenderer->OnOrderedListEnd();
		break;
	}

	case MD_BLOCK_LI:
	{
		pRenderer->OnListItemEnd();
		break;
	}

	case MD_BLOCK_QUOTE:
	{
		pRenderer->OnBlockQuoteEnd();
		break;
	}

	case MD_BLOCK_CODE:
	{
		pRenderer->OnCodeBlockEnd();
		break;
	}

	default:
	{
		break;
	}
	}

	return 0;
} // LeaveBlockCallback

/////////////////////////////////////////////////////////////////////////////
// EnterSpanCallback
int CMarkdownParser::EnterSpanCallback
(
	const MD_SPANTYPE type,
	void* detail,
	void* userdata
)
{
	CMarkdownParser* pParser =
		static_cast<CMarkdownParser*>(userdata);

	if (!pParser || !pParser->Renderer)
		return -1;

	// NEW: conflict check (no behavior change yet)
	ConflictRule eRule =
		pParser->ResolveConflict(pParser->CurrentBlock, type);

	if (eRule == ConflictRule_Downgrade)
	{
		// Do not emit span start callbacks.
		// Do not set InStrong / InEmphasis / InInlineCode.
		return 0;
	}

	CMarkdownRenderer* pRenderer = pParser->Renderer.get();

	switch (type)
	{
	case MD_SPAN_EM:
		pParser->InEmphasis = true;
		pRenderer->OnEmphasisStart();
		break;

	case MD_SPAN_STRONG:
		pParser->InStrong = true;
		pRenderer->OnStrongStart();
		break;

	case MD_SPAN_CODE:
		pParser->InInlineCode = true;
		pRenderer->OnInlineCodeStart();
		break;

	case MD_SPAN_IMG:
	{
		MD_SPAN_IMG_DETAIL* pImg =
			static_cast<MD_SPAN_IMG_DETAIL*>(detail);

		// image path (URL)
		CString csPath(pImg->src.text, (int)pImg->src.size);

		// alt text is delivered as nested MD_TEXT_NORMAL events
		// so for now we pass an empty string
		CString csAlt;

		pRenderer->OnImage(csPath, csAlt);
		break;
	}

	default:
		break;
	}

	return 0;
} // EnterSpanCallback

/////////////////////////////////////////////////////////////////////////////
// LeaveSpanCallback
/////////////////////////////////////////////////////////////////////////////
int CMarkdownParser::LeaveSpanCallback
(
	const MD_SPANTYPE type,
	void* detail,
	void* userdata
)
{
	CMarkdownParser* pParser =
		static_cast<CMarkdownParser*>(userdata);

	if (!pParser || !pParser->Renderer)
		return -1;

	// NEW: conflict check
	ConflictRule eRule =
		pParser->ResolveConflict(pParser->CurrentBlock, type);

	if (eRule == ConflictRule_Downgrade)
	{
		// Do not emit span end callbacks.
		return 0;
	}

	CMarkdownRenderer* pRenderer = pParser->Renderer.get();

	switch (type)
	{
	case MD_SPAN_EM:
		pRenderer->OnEmphasisEnd();
		break;

	case MD_SPAN_STRONG:
		pRenderer->OnStrongEnd();
		break;

	case MD_SPAN_CODE:
		pRenderer->OnInlineCodeEnd();
		break;

	case MD_SPAN_A:
		pRenderer->OnLinkEnd();
		break;

	default:
		break;
	}

	return 0;
} // LeaveSpanCallback

///////////////////////////////////////////////////////////////////////////
// TextCallback
///////////////////////////////////////////////////////////////////////////
int CMarkdownParser::TextCallback
(
	MD_TEXTTYPE       type,
	const MD_CHAR* text,
	MD_SIZE           size,
	void* userdata
)
{
	CMarkdownParser* pParser =
		static_cast<CMarkdownParser*>(userdata);

	if (!pParser || !pParser->Renderer)
		return -1;

	CMarkdownRenderer* pRenderer = pParser->Renderer.get();

	CString csText;

	if (text != NULL && size > 0)
	{
		CStringA csA(text, (int)size);
		csText = CString(csA);
	}

	//
	// Inline formatting spacing rule:
	//
	if ((pParser->InStrong || pParser->InEmphasis) &&
		!pParser->InInlineCode)
	{
		csText.TrimLeft();
		csText.TrimRight();
	}

	// Text conflict resolution
	ConflictRule eRule =
		pParser->ResolveTextConflict(pParser->CurrentBlock, type);

	if (eRule == ConflictRule_Downgrade)
	{
		// Only HTML text downgrades in our rules
		if (type == MD_TEXT_HTML)
			type = MD_TEXT_NORMAL;
	}

	if (eRule == ConflictRule_Downgrade && type == MD_TEXT_HTML)
	{
		// Treat HTML as normal text inside headings.
		type = MD_TEXT_NORMAL;
	}

	switch (type)
	{
	case MD_TEXT_NORMAL:
	{
		// Detect simple Markdown image: ![alt](path)
		// Example: "![Alt text](.\images\COP Meetings vs. CO2.jpg)"
		CString csTrimmed = csText;
		csTrimmed.Trim();

		if (csTrimmed.GetLength() > 0 &&
			csTrimmed[0] == L'!' &&
			csTrimmed.Find(L"![") == 0 &&
			csTrimmed.Find(L"](") > 0 &&
			csTrimmed.Right(1) == L")")
		{
			int nAltStart = csTrimmed.Find(L"[") + 1;
			int nAltEnd = csTrimmed.Find(L"](");
			int nPathStart = nAltEnd + 2; // skip "]("
			int nPathEnd = csTrimmed.ReverseFind(L')');

			CString csAlt = csTrimmed.Mid(nAltStart, nAltEnd - nAltStart);
			CString csPath = csTrimmed.Mid(nPathStart, nPathEnd - nPathStart);

			csAlt.Trim();
			csPath.Trim();

			// Dispatch to renderer as an image
			pRenderer->OnImage(csPath, csAlt);
		}
		else
		{
			// Normal text
			pRenderer->OnText(csText);
		}
		break;
	}

	case MD_TEXT_CODE:
		pRenderer->OnInlineCodeText(csText);
		break;

	case MD_TEXT_HTML:
		pRenderer->OnHtmlText(csText);
		break;

	case MD_TEXT_ENTITY:
		pRenderer->OnEntityText(csText);
		break;

	default:
		break;
	}

	return 0;
} // TextCallback

/////////////////////////////////////////////////////////////////////////////
