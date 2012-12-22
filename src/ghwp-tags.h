/*
 * ghwp-tags.c
 *
 * Copyright (C) 2012  Hodong Kim <cogniti@gmail.com>
 * 
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#ifndef _GHWP_TAG_H_
#define _GHWP_TAG_H_

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>

#define GHWP_TAG_BEGIN ((guint16) 16)
#define GHWP_TAG_DOCUMENT_PROPERTIES ((guint16) 16)
#define GHWP_TAG_ID_MAPPINGS ((guint16) 17)
#define GHWP_TAG_BIN_DATA ((guint16) 18)
#define GHWP_TAG_FACE_NAME ((guint16) 19)
#define GHWP_TAG_BORDER_FILL ((guint16) 20)
#define GHWP_TAG_CHAR_SHAPE ((guint16) 21)
#define GHWP_TAG_TAB_DEF ((guint16) 22)
#define GHWP_TAG_NUMBERING ((guint16) 23)
#define GHWP_TAG_BULLET ((guint16) 24)
#define GHWP_TAG_PARA_SHAPE ((guint16) 25)
#define GHWP_TAG_STYLE ((guint16) 26)
#define GHWP_TAG_DOC_DATA ((guint16) 27)
#define GHWP_TAG_DISTRIBUTE_DOC_DATA ((guint16) 28)
#define GHWP_TAG_RESERVED_29 ((guint16) 29)
#define GHWP_TAG_COMPATIBLE_DOCUMENT ((guint16) 30)
#define GHWP_TAG_LAYOUT_COMPATIBILITY ((guint16) 31)
#define GHWP_TAG_DOC_INFO_32 ((guint16) 32)
#define GHWP_TAG_FORBIDDEN_CHAR ((guint16) 94)
#define GHWP_TAG_PARA_HEADER ((guint16) 66)
#define GHWP_TAG_PARA_TEXT ((guint16) 67)
#define GHWP_TAG_PARA_CHAR_SHAPE ((guint16) 68)
#define GHWP_TAG_PARA_LINE_SEG ((guint16) 69)
#define GHWP_TAG_PARA_RANGE_TAG ((guint16) 70)
#define GHWP_TAG_CTRL_HEADER ((guint16) 71)
#define GHWP_TAG_LIST_HEADER ((guint16) 72)
#define GHWP_TAG_PAGE_DEF ((guint16) 73)
#define GHWP_TAG_FOOTNOTE_SHAPE ((guint16) 74)
#define GHWP_TAG_PAGE_BORDER_FILL ((guint16) 75)
#define GHWP_TAG_SHAPE_COMPONENT ((guint16) 76)
#define GHWP_TAG_TABLE ((guint16) 77)
#define GHWP_TAG_SHAPE_COMPONENT_LINE ((guint16) 78)
#define GHWP_TAG_SHAPE_COMPONENT_RECTANGLE ((guint16) 79)
#define GHWP_TAG_SHAPE_COMPONENT_ELLIPSE ((guint16) 80)
#define GHWP_TAG_SHAPE_COMPONENT_ARC ((guint16) 81)
#define GHWP_TAG_SHAPE_COMPONENT_POLYGON ((guint16) 82)
#define GHWP_TAG_SHAPE_COMPONENT_CURVE ((guint16) 83)
#define GHWP_TAG_SHAPE_COMPONENT_OLE ((guint16) 84)
#define GHWP_TAG_SHAPE_COMPONENT_PICTURE ((guint16) 85)
#define GHWP_TAG_SHAPE_COMPONENT_CONTAINER ((guint16) 86)
#define GHWP_TAG_CTRL_DATA ((guint16) 87)
#define GHWP_TAG_EQEDIT ((guint16) 88)
#define GHWP_TAG_RESERVED_89 ((guint16) 89)
#define GHWP_TAG_SHAPE_COMPONENT_TEXTART ((guint16) 90)
#define GHWP_TAG_FORM_OBJECT ((guint16) 91)
#define GHWP_TAG_MEMO_SHAPE ((guint16) 92)
#define GHWP_TAG_MEMO_LIST ((guint16) 93)
#define GHWP_TAG_CHART_DATA ((guint16) 95)
#define GHWP_TAG_SHAPE_COMPONENT_UNKNOWN ((guint16) 115)

const gchar* GHWP_TAG_NAMES[116] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13",
	"14", "15", "DOCUMENT_PROPERTIES", "ID_MAPPINGS", "BIN_DATA", "FACE_NAME",
	"BORDER_FILL", "CHAR_SHAPE", "TAB_DEF", "NUMBERING", "BULLET",
	"PARA_SHAPE", "STYLE", "DOC_DATA", "DISTRIBUTE_DOC_DATA", "RESERVED_29",
	"COMPATIBLE_DOCUMENT", "LAYOUT_COMPATIBILITY", "DOC_INFO_32", "33", "34",
	"35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46",
	"47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58",
	"59", "60", "61", "62", "63", "64", "65", "PARA_HEADER", "PARA_TEXT",
	"PARA_CHAR_SHAPE", "PARA_LINE_SEG", "PARA_RANGE_TAG", "CTRL_HEADER",
	"LIST_HEADER", "PAGE_DEF", "FOOTNOTE_SHAPE", "PAGE_BORDER_FILL",
	"SHAPE_COMPONENT", "TABLE", "SHAPE_COMPONENT_LINE",
	"SHAPE_COMPONENT_RECTANGLE", "SHAPE_COMPONENT_ELLIPSE",
	"SHAPE_COMPONENT_ARC", "SHAPE_COMPONENT_POLYGON", "SHAPE_COMPONENT_CURVE",
	"SHAPE_COMPONENT_OLE", "SHAPE_COMPONENT_PICTURE",
	"SHAPE_COMPONENT_CONTAINER", "CTRL_DATA", "EQEDIT", "RESERVED_89",
	"SHAPE_COMPONENT_TEXTART", "FORM_OBJECT", "MEMO_SHAPE", "MEMO_LIST",
	"FORBIDDEN_CHAR", "95", "96", "97", "98", "99", "100", "101", "102", "103",
	"104", "105", "106", "107", "108", "109", "110", "111", "112", "113",
	"114", "SHAPE_COMPONENT_UNKNOWN"
};

#endif /*_GHWP_TAG_H_ */