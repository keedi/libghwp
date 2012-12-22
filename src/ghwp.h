/*
 * ghwp.h
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

#ifndef __GHWP_H__
#define __GHWP_H__

#include <glib.h>
#include <gio/gio.h>
#include <gsf/gsf-input-impl.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <gsf/gsf-doc-meta-data.h>
#include <float.h>
#include <math.h>
#include <cairo.h>

G_BEGIN_DECLS


#define TYPE_GSF_INPUT_STREAM (gsf_input_stream_get_type ())
#define GSF_INPUT_STREAM(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GSF_INPUT_STREAM, GsfInputStream))
#define GSF_INPUT_STREAM_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GSF_INPUT_STREAM, GsfInputStreamClass))
#define IS_GSF_INPUT_STREAM(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_GSF_INPUT_STREAM))
#define IS_GSF_INPUT_STREAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GSF_INPUT_STREAM))
#define GSF_INPUT_STREAM_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GSF_INPUT_STREAM, GsfInputStreamClass))

typedef struct _GsfInputStream GsfInputStream;
typedef struct _GsfInputStreamClass GsfInputStreamClass;
typedef struct _GsfInputStreamPrivate GsfInputStreamPrivate;

#define GHWP_TYPE_GHWP_FILE (ghwp_ghwp_file_get_type ())
#define GHWP_GHWP_FILE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_GHWP_FILE, GHWPGHWPFile))
#define GHWP_GHWP_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_GHWP_FILE, GHWPGHWPFileClass))
#define GHWP_IS_GHWP_FILE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_GHWP_FILE))
#define GHWP_IS_GHWP_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_GHWP_FILE))
#define GHWP_GHWP_FILE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_GHWP_FILE, GHWPGHWPFileClass))

typedef struct _GHWPGHWPFile GHWPGHWPFile;
typedef struct _GHWPGHWPFileClass GHWPGHWPFileClass;
typedef struct _GHWPGHWPFilePrivate GHWPGHWPFilePrivate;

#define GHWP_GHWP_FILE_TYPE_HEADER (ghwp_ghwp_file_header_get_type ())
typedef struct _GHWPGHWPFileHeader GHWPGHWPFileHeader;

#define TYPE_TEXT_P (text_p_get_type ())
#define TEXT_P(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TEXT_P, TextP))
#define TEXT_P_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TEXT_P, TextPClass))
#define IS_TEXT_P(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TEXT_P))
#define IS_TEXT_P_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TEXT_P))
#define TEXT_P_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TEXT_P, TextPClass))

typedef struct _TextP TextP;
typedef struct _TextPClass TextPClass;
typedef struct _TextPPrivate TextPPrivate;

#define TYPE_TEXT_SPAN (text_span_get_type ())
#define TEXT_SPAN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TEXT_SPAN, TextSpan))
#define TEXT_SPAN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TEXT_SPAN, TextSpanClass))
#define IS_TEXT_SPAN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TEXT_SPAN))
#define IS_TEXT_SPAN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TEXT_SPAN))
#define TEXT_SPAN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TEXT_SPAN, TextSpanClass))

typedef struct _TextSpan TextSpan;
typedef struct _TextSpanClass TextSpanClass;
typedef struct _TextSpanPrivate TextSpanPrivate;

#define GHWP_TYPE_DOCUMENT (ghwp_document_get_type ())
#define GHWP_DOCUMENT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_DOCUMENT, GHWPDocument))
#define GHWP_DOCUMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_DOCUMENT, GHWPDocumentClass))
#define GHWP_IS_DOCUMENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_DOCUMENT))
#define GHWP_IS_DOCUMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_DOCUMENT))
#define GHWP_DOCUMENT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_DOCUMENT, GHWPDocumentClass))

typedef struct _GHWPDocument GHWPDocument;
typedef struct _GHWPDocumentClass GHWPDocumentClass;
typedef struct _GHWPDocumentPrivate GHWPDocumentPrivate;

#define GHWP_TYPE_PAGE (ghwp_page_get_type ())
#define GHWP_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_PAGE, GHWPPage))
#define GHWP_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_PAGE, GHWPPageClass))
#define GHWP_IS_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_PAGE))
#define GHWP_IS_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_PAGE))
#define GHWP_PAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_PAGE, GHWPPageClass))

typedef struct _GHWPPage GHWPPage;
typedef struct _GHWPPageClass GHWPPageClass;
typedef struct _GHWPPagePrivate GHWPPagePrivate;

#define GHWP_TYPE_CONTEXT (ghwp_context_get_type ())
#define GHWP_CONTEXT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_CONTEXT, GHWPContext))
#define GHWP_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_CONTEXT, GHWPContextClass))
#define GHWP_IS_CONTEXT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_CONTEXT))
#define GHWP_IS_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_CONTEXT))
#define GHWP_CONTEXT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_CONTEXT, GHWPContextClass))

typedef struct _GHWPContext GHWPContext;
typedef struct _GHWPContextClass GHWPContextClass;
typedef struct _GHWPContextPrivate GHWPContextPrivate;

struct _GsfInputStream {
	GInputStream parent_instance;
	GsfInputStreamPrivate * priv;
};

struct _GsfInputStreamClass {
	GInputStreamClass parent_class;
};

struct _GHWPGHWPFileHeader {
	gchar* signature;
	guint32 version;
	gboolean is_compress;
	gboolean is_encrypt;
	gboolean is_distribute;
	gboolean is_script;
	gboolean is_drm;
	gboolean is_xml_template;
	gboolean is_history;
	gboolean is_sign;
	gboolean is_certificate_encrypt;
	gboolean is_sign_spare;
	gboolean is_certificate_drm;
	gboolean is_ccl;
};

struct _GHWPGHWPFile {
	GObject parent_instance;
	GHWPGHWPFilePrivate * priv;
	GHWPGHWPFileHeader header;
	GInputStream* prv_text_stream;
	GInputStream* prv_image_stream;
	GInputStream* file_header_stream;
	GInputStream* doc_info_stream;
	GArray* section_streams;
	GInputStream* summary_info_stream;
};

struct _GHWPGHWPFileClass {
	GObjectClass parent_class;
};

struct _TextP {
	GObject parent_instance;
	TextPPrivate * priv;
	GArray* textspans;
};

struct _TextPClass {
	GObjectClass parent_class;
};

struct _TextSpan {
	GObject parent_instance;
	TextSpanPrivate * priv;
	gchar* text;
};

struct _TextSpanClass {
	GObjectClass parent_class;
};

struct _GHWPDocument {
	GObject parent_instance;
	GHWPDocumentPrivate * priv;
	GHWPGHWPFile* ghwp_file;
	gchar* prv_text;
	GArray* office_text;
	GArray* pages;
	GsfDocMetaData* summary_info;
};

struct _GHWPDocumentClass {
	GObjectClass parent_class;
};

struct _GHWPPage {
	GObject parent_instance;
	GHWPPagePrivate * priv;
	GArray* elements;
};

struct _GHWPPageClass {
	GObjectClass parent_class;
};

struct _GHWPContext {
	GObject parent_instance;
	GHWPContextPrivate * priv;
	guint16 tag_id;
	guint16 level;
	guint32 size;
	guchar* data;
	gint data_length1;
};

struct _GHWPContextClass {
	GObjectClass parent_class;
};


GType gsf_input_stream_get_type (void) G_GNUC_CONST;
GsfInputStream* gsf_input_stream_new (GsfInput* input);
GsfInputStream* gsf_input_stream_construct (GType object_type, GsfInput* input);
gssize gsf_input_stream_size (GsfInputStream* self);
GType ghwp_ghwp_file_get_type (void) G_GNUC_CONST;
GType ghwp_ghwp_file_header_get_type (void) G_GNUC_CONST;
GHWPGHWPFileHeader* ghwp_ghwp_file_header_dup (const GHWPGHWPFileHeader* self);
void ghwp_ghwp_file_header_free (GHWPGHWPFileHeader* self);
void ghwp_ghwp_file_header_copy (const GHWPGHWPFileHeader* self, GHWPGHWPFileHeader* dest);
void ghwp_ghwp_file_header_destroy (GHWPGHWPFileHeader* self);
GHWPGHWPFile* ghwp_ghwp_file_new_from_uri (const gchar* uri, GError** error);
GHWPGHWPFile* ghwp_ghwp_file_construct_from_uri (GType object_type, const gchar* uri, GError** error);
GHWPGHWPFile* ghwp_ghwp_file_new_from_filename (const gchar* filename, GError** error);
GHWPGHWPFile* ghwp_ghwp_file_construct_from_filename (GType object_type, const gchar* filename, GError** error);
GHWPGHWPFile* ghwp_ghwp_file_new (void);
GHWPGHWPFile* ghwp_ghwp_file_construct (GType object_type);
GType text_p_get_type (void) G_GNUC_CONST;
GType text_span_get_type (void) G_GNUC_CONST;
void text_p_add_textspan (TextP* self, TextSpan* textspan);
TextP* text_p_new (void);
TextP* text_p_construct (GType object_type);
TextSpan* text_span_new (const gchar* text);
TextSpan* text_span_construct (GType object_type, const gchar* text);
GType ghwp_document_get_type (void) G_GNUC_CONST;
GType ghwp_page_get_type (void) G_GNUC_CONST;
GHWPDocument* ghwp_document_new_from_uri (const gchar* uri, GError** error);
GHWPDocument* ghwp_document_construct_from_uri (GType object_type, const gchar* uri, GError** error);
GHWPDocument* ghwp_document_new_from_filename (const gchar* filename, GError** error);
GHWPDocument* ghwp_document_construct_from_filename (GType object_type, const gchar* filename, GError** error);
guint ghwp_document_get_n_pages (GHWPDocument* self);
GHWPPage* ghwp_document_get_page (GHWPDocument* self, gint n_page);
GHWPDocument* ghwp_document_new (void);
GHWPDocument* ghwp_document_construct (GType object_type);
void ghwp_page_get_size (GHWPPage* self, gdouble* width, gdouble* height);
gboolean ghwp_page_render (GHWPPage* self, cairo_t* cr);
GHWPPage* ghwp_page_new (void);
GHWPPage* ghwp_page_construct (GType object_type);
GType ghwp_context_get_type (void) G_GNUC_CONST;
GHWPContext* ghwp_context_new (GInputStream* stream);
GHWPContext* ghwp_context_construct (GType object_type, GInputStream* stream);
gboolean ghwp_context_decode_header (GHWPContext* self, guchar* buf, int buf_length1);
gboolean ghwp_context_pull (GHWPContext* self);


G_END_DECLS

#endif
