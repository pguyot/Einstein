// ==============================
// File:			TFLScriptPanel.cpp
// Project:			Einstein
//
// Copyright 2003-2020 by Paul Guyot and Matthias Melcher.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

#include "TFLScriptPanel.h"

#include "TTkScript.h"

#include <cstdlib>
#include <ctype.h>

// MARK: - TFLScriptPanel -

/**
 * This class provides and FLTK UI to edit NewtonScript source code.
 */
TFLScriptPanel::TFLScriptPanel(int x, int y, int w, int h, const char* label) :
		Fl_Group(x, y, w, h, label), mEditor(new TFLScriptEditor(x, y, w, h))
{
	mEditor->SetPanel(this);
	resizable(mEditor);
}

TFLScriptPanel::~TFLScriptPanel()
{
}

/**
 * Load a script form a file into the text buffer.
 */
void
TFLScriptPanel::LoadFile(const char* filename)
{
	// TODO: there is no error handling yet
	mEditor->buffer()->loadfile(filename);
}

/**
 * Return a duplicate of the current source code in the editor.
 *
 * Caller is responsible to \c free() the text that was returned.
 *
 * \return a copy of the current source code
 * \note the code is not saved to a file before it is returned!
 */
char*
TFLScriptPanel::DupSourceCode()
{
	return mEditor->buffer()->text();
}

void
TFLScriptPanel::SetSourceCode(const char* sourcecode)
{
	mEditor->buffer()->text(sourcecode);
}

void
TFLScriptPanel::SetDirty()
{
	mScript->SetDirty();
}

void
TFLScriptPanel::ClearDirty()
{
	mScript->ClearDirty();
}

// MARK: - TFLScriptEditor -

// FIXME: the syntax highlighting was taken from the FLTK "C" editor demo and needs to be updated to NewtonScript
// FIXME: actually, lex and yacc could be used for syntax highlighting... .

// Syntax highlighting stuff...

#define TS 14 // default editor textsize

static char sSymbolFirstChar = 0;

Fl_Text_Display::Style_Table_Entry TFLScriptEditor::styletable[] = {
	{ FL_BLACK, FL_COURIER, TS, 0 }, // A - Plain
	{ FL_DARK_GREEN, FL_COURIER, TS, 0 }, // B - Line comments
	{ FL_DARK_GREEN, FL_COURIER, TS, 0 }, // C - Block comments
	{ 0x99000000, FL_COURIER, TS, 0 }, // D - Strings
	{ FL_DARK_MAGENTA, FL_COURIER, TS, 0 }, // E - Symbol
	{ FL_BLUE, FL_COURIER_BOLD, TS, 0 }, // F - Types
	{ FL_DARK_MAGENTA, FL_COURIER_BOLD, TS, 0 }, // G - Keywords
};

const char* TFLScriptEditor::code_keywords[] = {
	// List of known NewtonScript reserved words...
	"and",
	"begin",
	"break",
	"by",
	"call",
	"constant",
	"div",
	"do",
	"else",
	"end",
	"exists",
	"for",
	"foreach",
	"func",
	"global",
	"if",
	"in",
	"inherited",
	"local",
	"loop",
	"mod",
	"native",
	"not",
	"onexception",
	"or",
	"repeat",
	"return",
	"self",
	"then",
	"to",
	"try",
	"until",
	"while",
	"with",
};

const char* TFLScriptEditor::code_types[] = {
	// List of known C/C++ types...
	"nil",
	"true",
};

/**
 * This class displays and provides editing capabilities for NewtonScript source code.
 */
TFLScriptEditor::TFLScriptEditor(int x, int y, int w, int h, const char* label) :
		Fl_Text_Editor(x, y, w, h, label)
{
	box(FL_FLAT_BOX);
	color(fl_color_average(FL_BACKGROUND2_COLOR, FL_WHITE, 0.9));
	selection_color(0xCCCCFF00);
	textfont(FL_COURIER);
	textsize(TS);
	linenumber_width(40); // enable
	linenumber_size(textsize() - 2);
	wrap_mode(Fl_Text_Display::WRAP_NONE, 0);

	mTextBuffer = new Fl_Text_Buffer();
	buffer(mTextBuffer);

	mStyleBuffer = new Fl_Text_Buffer();
	highlight_data(mStyleBuffer, styletable,
		sizeof(styletable) / sizeof(styletable[0]),
		'A', style_unfinished_cb, 0);

	mTextBuffer->add_modify_callback(style_update_cb, this);
	mTextBuffer->call_modify_callbacks();
	mTextBuffer->tab_distance(4);
}

TFLScriptEditor::~TFLScriptEditor()
{
	delete mStyleBuffer;
}

#if TARGET_OS_WIN32
static int
strcasecmp(const char* a, const char* b)
{
	return stricmp(a, b);
}
#endif

extern "C" int
compare_keywords(const void* a, const void* b)
{
	//    static const char *lut[] = {"<", "=", ">"};
	const char* aa = *(const char**) a;
	const char* bb = *(const char**) b;
	int ret = (strcasecmp(aa, bb));
	//    printf("%s %s %s\n", aa, lut[((rewt > 0) - (ret < 0))+1], bb);
	return ret;
}

//
// 'style_parse()' - Parse text and produce style data.
//

void
TFLScriptEditor::style_parse(const char* text,
	char* style,
	int length)
{
	char current;
	int col;
	int last;
	char buf[255],
		*bufptr;
	const char* temp;

	// Style letters:
	//
	// A - Plain
	// B - Line comments
	// C - Block comments
	// D - Strings
	// E - Symbol
	// F - Types
	// G - Keywords

	for (current = *style, col = 0, last = 0; length > 0; length--, text++)
	{
		if (current == 'B' || current == 'F' || current == 'G')
			current = 'A';
		if (current == 'A')
		{
			// Check for directives, comments, strings, and keywords...
			if (*text == '\'')
			{
				// Set style to directive
				current = 'E';
				sSymbolFirstChar = 0;
			} else if (strncmp(text, "//", 2) == 0)
			{
				current = 'B';
				for (; length > 0 && *text != '\n'; length--, text++)
					*style++ = 'B';

				if (length == 0)
					break;
			} else if (strncmp(text, "/*", 2) == 0)
			{
				current = 'C';
			} else if (strncmp(text, "\\\"", 2) == 0)
			{
				// Quoted quote...
				*style++ = current;
				*style++ = current;
				text++;
				length--;
				col += 2;
				continue;
			} else if (*text == '\"')
			{
				current = 'D';
			} else if (!last && (isalpha((*text) & 255) || *text == '_'))
			{
				// Might be a keyword...
				for (temp = text, bufptr = buf;
					 (isalnum((*temp) & 255) || *temp == '_') && bufptr < (buf + sizeof(buf) - 1);
					 *bufptr++ = *temp++)
				{
					// nothing
				}

				if (!isalnum((*temp) & 255) && *temp != '_')
				{
					*bufptr = '\0';

					bufptr = buf;

					if (bsearch(&bufptr, code_types,
							sizeof(code_types) / sizeof(code_types[0]),
							sizeof(code_types[0]), compare_keywords))
					{
						while (text < temp)
						{
							*style++ = 'F';
							text++;
							length--;
							col++;
						}

						text--;
						length++;
						last = 1;
						continue;
					} else if (bsearch(&bufptr, code_keywords,
								   sizeof(code_keywords) / sizeof(code_keywords[0]),
								   sizeof(code_keywords[0]), compare_keywords))
					{
						while (text < temp)
						{
							*style++ = 'G';
							text++;
							length--;
							col++;
						}

						text--;
						length++;
						last = 1;
						continue;
					}
				}
			}
		} else if (current == 'C' && strncmp(text, "*/", 2) == 0)
		{
			// Close a C comment...
			*style++ = current;
			*style++ = current;
			text++;
			length--;
			current = 'A';
			col += 2;
			continue;
		} else if (current == 'D')
		{
			// Continuing in string...
			if (strncmp(text, "\\\"", 2) == 0)
			{
				// Quoted end quote...
				*style++ = current;
				*style++ = current;
				text++;
				length--;
				col += 2;
				continue;
			} else if (*text == '\"')
			{
				// End quote...
				*style++ = current;
				col++;
				current = 'A';
				continue;
			}
		} else if (current == 'E')
		{
			if (sSymbolFirstChar == 0)
			{
				// we don't know yet if this is a text-only symbol, or a symbol separted by vertical bars
				if (*text == '|')
					// complex symbol name bracketed by '|', for example '|clock:MATT|
					sSymbolFirstChar = '|';
				else if (isalpha(*text) || *text == '_')
					// alphanumeric symbol name
					sSymbolFirstChar = 'A';
				else
					// not a symbol, my be a literal frame: ``x := '{ text: "a" );``
					current = 'A';
			} else if (sSymbolFirstChar == '|')
			{
				if (*text == '\\')
					sSymbolFirstChar = '\\';
				else if (*text == '|')
					sSymbolFirstChar = '.';
			} else if (sSymbolFirstChar == '\\')
			{
				sSymbolFirstChar = '|';
			} else if (sSymbolFirstChar == '.')
			{
				current = 'A';
			} else
			{
				if (!isalnum(*text) && *text != '_')
					current = 'A';
			}
		}

		// Copy style info...
		if (current == 'A' && (*text == '{' || *text == '}'))
			*style++ = 'G';
		else
			*style++ = current;
		col++;

		last = isalnum((*text) & 255) || *text == '_' || *text == '.';

		if (*text == '\n')
		{
			// Reset column and possibly reset the style
			col = 0;
			if (current == 'B')
				current = 'A';
		}
	}
}

//
// 'style_init()' - Initialize the style buffer...
//

void
TFLScriptEditor::style_init()
{
	char* style = new char[mTextBuffer->length() + 1];
	char* text = mTextBuffer->text();

	memset(style, 'A', mTextBuffer->length());
	style[mTextBuffer->length()] = '\0';

	style_parse(text, style, mTextBuffer->length());

	mStyleBuffer->text(style);
	delete[] style;
	free(text);
}

//
// 'style_unfinished_cb()' - Update unfinished styles.
//

void
TFLScriptEditor::style_unfinished_cb(int, void*)
{
}

//
// 'style_update()' - Update the style buffer...
//

void
TFLScriptEditor::style_update(int pos, // I - Position of update
	int nInserted, // I - Number of inserted chars
	int nDeleted, // I - Number of deleted chars
	int /*nRestyled*/, // I - Number of restyled chars
	const char* /*deletedText*/) // I - Text that was deleted
{
	int start, // Start of text
		end; // End of text
	char last, // Last style on line
		*style, // Style data
		*text; // Text data

	// If this is just a selection change, just unselect the style buffer...
	if (nInserted == 0 && nDeleted == 0)
	{
		mStyleBuffer->unselect();
		return;
	}

	mPanel->SetDirty();

	// Track changes in the text buffer...
	if (nInserted > 0)
	{
		// Insert characters into the style buffer...
		style = new char[nInserted + 1];
		memset(style, 'A', nInserted);
		style[nInserted] = '\0';

		mStyleBuffer->replace(pos, pos + nDeleted, style);
		delete[] style;
	} else
	{
		// Just delete characters in the style buffer...
		mStyleBuffer->remove(pos, pos + nDeleted);
	}

	// Select the area that was just updated to avoid unnecessary
	// callbacks...
	mStyleBuffer->select(pos, pos + nInserted - nDeleted);

	// Re-parse the changed region; we do this by parsing from the
	// beginning of the previous line of the changed region to the end of
	// the line of the changed region...  Then we check the last
	// style character and keep updating if we have a multi-line
	// comment character...
	start = mTextBuffer->line_start(pos);
	//  if (start > 0) start = textbuf->line_start(start - 1);
	end = mTextBuffer->line_end(pos + nInserted);
	text = mTextBuffer->text_range(start, end);
	style = mStyleBuffer->text_range(start, end);
	if (start == end)
		last = 0;
	else
		last = style[end - start - 1];

	//  printf("start = %d, end = %d, text = \"%s\", style = \"%s\", last='%c'...\n",
	//         start, end, text, style, last);

	style_parse(text, style, end - start);

	//  printf("new style = \"%s\", new last='%c'...\n",
	//         style, style[end - start - 1]);

	mStyleBuffer->replace(start, end, style);
	redisplay_range(start, end);

	if (start == end || last != style[end - start - 1])
	{
		//    printf("Recalculate the rest of the buffer style\n");
		// Either the user deleted some text, or the last character
		// on the line changed styles, so reparse the
		// remainder of the buffer...
		free(text);
		free(style);

		end = mTextBuffer->length();
		text = mTextBuffer->text_range(start, end);
		style = mStyleBuffer->text_range(start, end);

		style_parse(text, style, end - start);

		mStyleBuffer->replace(start, end, style);
		redisplay_range(start, end);
	}

	free(text);
	free(style);
}

// ============================================================================ //
// NewtonScript
//
// A dynamic, object-oriented programming language developed for the Newton
// platform. Lovingly nicknamed WallyScript after its designer Walter Smith.
//
//   -- Newton Glossary
// ============================================================================ //
