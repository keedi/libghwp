/*
 * ghwp-document.c
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

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <gsf/gsf-doc-meta-data.h>
#include <gio/gio.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-msole-utils.h>
#include <gsf/gsf-input-impl.h>
#include <cairo.h>

#include "ghwp-document.h"
#include "gsf-input-stream.h"
#include "ghwp-parse.h"
#include "ghwp-tags.h"

G_DEFINE_TYPE (GHWPDocument, ghwp_document, G_TYPE_OBJECT);
G_DEFINE_TYPE (TextSpan, text_span, G_TYPE_OBJECT);
G_DEFINE_TYPE (TextP, text_p, G_TYPE_OBJECT);

static void ghwp_document_parse (GHWPDocument* self);
static void ghwp_document_parse_doc_info (GHWPDocument* self);
static void ghwp_document_parse_body_text (GHWPDocument* self);
static void ghwp_document_parse_prv_text (GHWPDocument* self);
static void ghwp_document_parse_summary_info (GHWPDocument* self);

static gchar*
ghwp_document_get_text_from_raw_data (GHWPDocument *self,
                                      guchar       *raw,
                                      int           raw_len);
static void ghwp_document_make_pages (GHWPDocument* self);
static void ghwp_document_finalize (GObject* obj);

static void text_p_finalize (GObject* obj);

static void text_span_finalize (GObject* obj);

#define _g_array_free0(var) ((var == NULL) ? NULL : (var = (g_array_free (var, TRUE), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))

static gpointer _g_object_ref0 (gpointer self)
{
    return self ? g_object_ref (self) : NULL;
}


void text_p_add_textspan (TextP* self, TextSpan* textspan)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (textspan != NULL);
    TextSpan* _tmp2_ = _g_object_ref0 (textspan);
    g_array_append_val (self->textspans, _tmp2_);
}


TextP* text_p_new (void)
{
    return (TextP*) g_object_new (TEXT_TYPE_P, NULL);
}


static void text_p_class_init (TextPClass * klass)
{
    text_p_parent_class = g_type_class_peek_parent (klass);
    G_OBJECT_CLASS (klass)->finalize = text_p_finalize;
}


static void text_p_init (TextP * self)
{
    self->textspans = g_array_new (TRUE, TRUE, sizeof (TextSpan*));
}


static void text_p_finalize (GObject* obj)
{
    TextP * self = G_TYPE_CHECK_INSTANCE_CAST (obj, TEXT_TYPE_P, TextP);
    _g_array_free0 (self->textspans);
    G_OBJECT_CLASS (text_p_parent_class)->finalize (obj);
}


TextSpan* text_span_new (const gchar* text)
{
    g_return_val_if_fail (text != NULL, NULL);
    TextSpan * self = (TextSpan*) g_object_new (TEXT_TYPE_SPAN, NULL);
    _g_free0 (self->text);
    self->text = g_strdup (text);
    return self;
}


static void text_span_class_init (TextSpanClass * klass) {
    text_span_parent_class = g_type_class_peek_parent (klass);
    G_OBJECT_CLASS (klass)->finalize = text_span_finalize;
}


static void text_span_init (TextSpan * self)
{
}


static void text_span_finalize (GObject* obj)
{
    TextSpan * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, TEXT_TYPE_SPAN, TextSpan);
    _g_free0 (self->text);
    G_OBJECT_CLASS (text_span_parent_class)->finalize (obj);
}


GHWPDocument* ghwp_document_new_from_uri (const gchar* uri, GError** error)
{
    g_return_val_if_fail (uri != NULL, NULL);
    GHWPFile *file = ghwp_file_new_from_uri (uri, error);

    if (file == NULL) {
        return NULL;
    }

    GHWPDocument *doc = ghwp_document_new();
    _g_object_unref0 (doc->ghwp_file);
    doc->ghwp_file = file;
    ghwp_document_parse (doc);
    return doc;
}


GHWPDocument*
ghwp_document_new_from_filename (const gchar* filename, GError** error)
{
    g_return_val_if_fail (filename != NULL, NULL);
    GHWPFile *file = ghwp_file_new_from_filename (filename, error);

    if (file == NULL) {
        return NULL;
    }

    GHWPDocument *doc = ghwp_document_new();
    _g_object_unref0 (doc->ghwp_file);
    doc->ghwp_file = file;
    ghwp_document_parse (doc);
    return doc;
}


static void ghwp_document_parse (GHWPDocument* self)
{
    g_return_if_fail (self != NULL);
    ghwp_document_parse_doc_info (self);
    ghwp_document_parse_body_text (self);
    ghwp_document_parse_prv_text (self);
    ghwp_document_parse_summary_info (self);
}


guint ghwp_document_get_n_pages (GHWPDocument* self)
{
    g_return_val_if_fail (self != NULL, 0U);
    return self->pages->len;
}


GHWPPage* ghwp_document_get_page (GHWPDocument* self, gint n_page)
{
    g_return_val_if_fail (self != NULL, NULL);
    GHWPPage* page = g_array_index (self->pages, GHWPPage*, (guint) n_page);
    return _g_object_ref0 (page);
}


static void ghwp_document_parse_doc_info (GHWPDocument* self)
{
    g_return_if_fail (self != NULL);
    GInputStream* stream  = self->ghwp_file->doc_info_stream;
    GHWPContext*  context = ghwp_context_new (stream);
    while (ghwp_context_pull (context)) {
        /* TODO */
    }
    _g_object_unref0 (context);
}


static gchar*
ghwp_document_get_text_from_raw_data (GHWPDocument *self,
                                      guchar       *raw,
                                      int           raw_len)
{
    g_return_val_if_fail (self != NULL, NULL);
    gunichar ch;
    GString *text = g_string_new("");

    guint i;
    for (i = 0; i < raw_len; i = i + 2)
    {
        ch = (gunichar) ((raw[i+1] << 8) | raw[i]);
        switch (ch) {
            case 0:
                break;
            case 1:
            case 2:
            case 3:
            case 4: /* inline */
            case 5: /* inline */
            case 6: /* inline */
            case 7: /* inline */
            case 8: /* inline */
            {
                i = i + 14;
                break;
            }
            case 9: /* inline */ /* tab */
            {
                i = i + 14;
                g_string_append_unichar(text, ch);
                break;
            }
            case 10:
                break;
            case 11:
            case 12:
            {
                i = i + 14;
                break;
            }
            case 13:
                break;
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19: /* inline */
            case 20: /* inline */
            case 21:
            case 22:
            case 23:
            {
                i = i + 14;
                break;
            }
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
            case 31:
                break;
            default:
            {
                g_string_append_unichar(text, ch);
                break;
            }
        }
    }

    return g_string_free(text, FALSE);
}


static void ghwp_document_parse_body_text (GHWPDocument* self)
{
    g_return_if_fail (self != NULL);
    guint curr_lv = 0;
    guint prev_lv = 0;
    guint index;

    for (index = 0; index < self->ghwp_file->section_streams->len; index++) {
        GInputStream *section_stream;
        GHWPContext  *context;
        section_stream = g_array_index (self->ghwp_file->section_streams,
                                        GInputStream*,
                                        index);
        section_stream = _g_object_ref0 (section_stream);

        context = ghwp_context_new (section_stream);

        while (ghwp_context_pull(context)) {
            curr_lv = (guint) context->level;

            switch (context->tag_id) {
                case GHWP_TAG_PARA_HEADER:
                {
                    TextP* _textp_ = text_p_new();
                    g_array_append_val (self->office_text, _textp_);
                    break;
                }
                case GHWP_TAG_PARA_TEXT:
                    if (curr_lv > prev_lv) {
                        TextP *textp;
                        textp = g_array_index (self->office_text,
                                               TextP*,
                                               self->office_text->len - 1);
                        textp = _g_object_ref0 (textp);

                        gchar *text;
                        text = ghwp_document_get_text_from_raw_data (self,
                                                            context->data,
                                                            context->data_len);
                        TextSpan *_textspan_ = text_span_new (text);
                        text_p_add_textspan (textp, _textspan_);
                        _g_object_unref0 (_textspan_);
                        _g_free0 (text);
                        _g_object_unref0 (textp);
                        break;
                    }
                case GHWP_TAG_PARA_CHAR_SHAPE:
                    break;
                case GHWP_TAG_PARA_LINE_SEG:
                    break;
                case GHWP_TAG_CTRL_HEADER:
                    break;
                case GHWP_TAG_PAGE_DEF:
                    break;
                case GHWP_TAG_FOOTNOTE_SHAPE:
                    break;
                case GHWP_TAG_PAGE_BORDER_FILL:
                    break;
                case GHWP_TAG_LIST_HEADER:
                    break;
                case GHWP_TAG_EQEDIT:
                    break;
                default:
                {
                    fprintf (stderr, "%s: not implemented\n",
                             GHWP_TAG_NAMES[context->tag_id]);
                    break;
                }
            }
            prev_lv = curr_lv;
        }
        _g_object_unref0 (context);
        _g_object_unref0 (section_stream);
    }
    ghwp_document_make_pages (self);
}


static void ghwp_document_make_pages (GHWPDocument* self)
{
    g_return_if_fail (self != NULL);
    gdouble   y    = 0.0;
    guint     len  = 0;
    GHWPPage *page = ghwp_page_new ();
    guint     i, j;

    for (i = 0; i < self->office_text->len; i++) {
        TextP* textp;
        textp = g_array_index (self->office_text, TextP*, i);
        textp = _g_object_ref0 (textp);

        for(j = 0; j < textp->textspans->len; j++) {

            TextSpan* textspan;
            textspan = g_array_index (textp->textspans, TextSpan*, j);
            textspan = _g_object_ref0 (textspan);

            len = g_utf8_strlen (textspan->text, -1);

            y = y + (18.0 * ceil (len / 33.0));


            if (y > 842.0 - 80.0) {
                page = _g_object_ref0 (page);
                g_array_append_val (self->pages, page);
                _g_object_unref0 (page);
                page = ghwp_page_new ();
                textspan = _g_object_ref0 (textspan);
                g_array_append_val (page->elements, textspan);
                y = 0.0;
            } else {
                textspan = _g_object_ref0 (textspan);
                g_array_append_val (page->elements, textspan);
            } /* if */
            _g_object_unref0 (textspan);
        } /* for */
        _g_object_unref0 (textp);
    } /* for */
    /* add last page */
    page = _g_object_ref0 (page);
    g_array_append_val (self->pages, page);
    _g_object_unref0 (page);
}


static void ghwp_document_parse_prv_text (GHWPDocument* self)
{
    g_return_if_fail (self != NULL);

    GsfInputStream *gis   = _g_object_ref0 (self->ghwp_file->prv_text_stream);
    gssize          size  = gsf_input_stream_size (gis);
    guchar         *buf   = g_new0 (guchar, size);
    GError         *error = NULL;

    g_input_stream_read ((GInputStream*) gis, buf, size, NULL, &error);

    if (error != NULL) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, error->message);
        _g_free0 (self->prv_text);
        g_clear_error (&error);
        buf = (g_free (buf), NULL);
        _g_object_unref0 (gis);
        return;
    }

    self->prv_text = g_convert ((const gchar*) buf, (gssize) size,
                                "UTF-8", "UTF-16LE", NULL, NULL, &error);

    if (error != NULL) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, error->message);
        _g_free0 (self->prv_text);
        g_clear_error (&error);
        buf = (g_free (buf), NULL);
        _g_object_unref0 (gis);
        return;
    }

    buf = (g_free (buf), NULL);
    _g_object_unref0 (gis);
}


static void ghwp_document_parse_summary_info (GHWPDocument* self)
{
    g_return_if_fail (self != NULL);

    GsfInputStream *gis;
    gssize          size;
    guchar         *buf;
    GsfInputMemory *summary;
    GsfDocMetaData *meta;
    GError         *error = NULL;

    gis  = _g_object_ref0 (self->ghwp_file->summary_info_stream);
    size = gsf_input_stream_size (gis);
    buf  = g_new0 (guchar, size);

    g_input_stream_read ((GInputStream*) gis, buf, (gsize) size, NULL, &error);

    if (error != NULL) {
        buf = (g_free (buf), NULL);
        _g_object_unref0 (gis);
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, error->message);
        g_clear_error (&error);
        return;
    }

    /* changwoo's solution, thanks to changwoo.
     * https://groups.google.com/forum/#!topic/libhwp/gFDD7UMCXBc
     * https://github.com/changwoo/gnome-hwp-support/blob/master/properties/props-data.c
     * Trick the libgsf's MSOLE property set parser, by changing
     * its GUID. The \005HwpSummaryInformation is compatible with
     * the summary property set.
     */
    guint8 component_guid [] = {
        0xe0, 0x85, 0x9f, 0xf2, 0xf9, 0x4f, 0x68, 0x10,
        0xab, 0x91, 0x08, 0x00, 0x2b, 0x27, 0xb3, 0xd9
    };

    memcpy (buf + 28, component_guid, (gsize) sizeof(component_guid));
    summary = (GsfInputMemory*) gsf_input_memory_new (buf, size, FALSE);

    meta = gsf_doc_meta_data_new ();
    gsf_msole_metadata_read ((GsfInput*) summary, meta);

    _g_object_unref0 (self->summary_info);
    self->summary_info = _g_object_ref0 (meta);
    _g_object_unref0 (meta);
    _g_object_unref0 (summary);
    buf = (g_free (buf), NULL);
    _g_object_unref0 (gis);
}


GHWPDocument* ghwp_document_new (void)
{
    return (GHWPDocument*) g_object_new (GHWP_TYPE_DOCUMENT, NULL);
}


static void ghwp_document_class_init (GHWPDocumentClass * klass) {
    ghwp_document_parent_class = g_type_class_peek_parent (klass);
    G_OBJECT_CLASS (klass)->finalize = ghwp_document_finalize;
}


static void ghwp_document_init (GHWPDocument * self) {
    self->office_text = g_array_new (TRUE, TRUE, sizeof (TextP*));
    self->pages = g_array_new (TRUE, TRUE, sizeof (GHWPPage*));
}


static void ghwp_document_finalize (GObject* obj) {
    GHWPDocument * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, GHWP_TYPE_DOCUMENT, GHWPDocument);
    _g_object_unref0 (self->ghwp_file);
    _g_free0 (self->prv_text);
    _g_array_free0 (self->office_text);
    _g_array_free0 (self->pages);
    _g_object_unref0 (self->summary_info);
    G_OBJECT_CLASS (ghwp_document_parent_class)->finalize (obj);
}
