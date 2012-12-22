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
#define _g_array_free0(var) ((var == NULL) ? NULL : (var = (g_array_free (var, TRUE), NULL)))
typedef struct _TextSpanPrivate TextSpanPrivate;
#define _g_free0(var) (var = (g_free (var), NULL))

#define GHWP_TYPE_DOCUMENT (ghwp_document_get_type ())
#define GHWP_DOCUMENT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_DOCUMENT, GHWPDocument))
#define GHWP_DOCUMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_DOCUMENT, GHWPDocumentClass))
#define GHWP_IS_DOCUMENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_DOCUMENT))
#define GHWP_IS_DOCUMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_DOCUMENT))
#define GHWP_DOCUMENT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_DOCUMENT, GHWPDocumentClass))

typedef struct _GHWPDocument GHWPDocument;
typedef struct _GHWPDocumentClass GHWPDocumentClass;
typedef struct _GHWPDocumentPrivate GHWPDocumentPrivate;

#define GHWP_TYPE_GHWP_FILE (ghwp_file_get_type ())
#define GHWP_FILE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_GHWP_FILE, GHWPFile))
#define GHWP_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_GHWP_FILE, GHWPFileClass))
#define GHWP_IS_GHWP_FILE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_GHWP_FILE))
#define GHWP_IS_GHWP_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_GHWP_FILE))
#define GHWP_FILE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_GHWP_FILE, GHWPFileClass))

typedef struct _GHWPFile GHWPFile;
typedef struct _GHWPFileClass GHWPFileClass;

#define GHWP_TYPE_PAGE (ghwp_page_get_type ())
#define GHWP_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_PAGE, GHWPPage))
#define GHWP_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_PAGE, GHWPPageClass))
#define GHWP_IS_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_PAGE))
#define GHWP_IS_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_PAGE))
#define GHWP_PAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_PAGE, GHWPPageClass))

typedef struct _GHWPPage GHWPPage;
typedef struct _GHWPPageClass GHWPPageClass;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
typedef struct _GHWPFilePrivate GHWPFilePrivate;

#define GHWP_FILE_TYPE_HEADER (ghwp_file_header_get_type ())
typedef struct _GHWPFileHeader GHWPFileHeader;

#define GHWP_TYPE_CONTEXT (ghwp_context_get_type ())
#define GHWP_CONTEXT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_CONTEXT, GHWPContext))
#define GHWP_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_CONTEXT, GHWPContextClass))
#define GHWP_IS_CONTEXT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_CONTEXT))
#define GHWP_IS_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_CONTEXT))
#define GHWP_CONTEXT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_CONTEXT, GHWPContextClass))

typedef struct _GHWPContext GHWPContext;
typedef struct _GHWPContextClass GHWPContextClass;
typedef struct _GHWPContextPrivate GHWPContextPrivate;
typedef struct _GHWPPagePrivate GHWPPagePrivate;

#define TYPE_GSF_INPUT_STREAM (gsf_input_stream_get_type ())
#define GSF_INPUT_STREAM(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GSF_INPUT_STREAM, GsfInputStream))
#define GSF_INPUT_STREAM_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GSF_INPUT_STREAM, GsfInputStreamClass))
#define IS_GSF_INPUT_STREAM(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_GSF_INPUT_STREAM))
#define IS_GSF_INPUT_STREAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GSF_INPUT_STREAM))
#define GSF_INPUT_STREAM_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GSF_INPUT_STREAM, GsfInputStreamClass))

typedef struct _GsfInputStream GsfInputStream;
typedef struct _GsfInputStreamClass GsfInputStreamClass;
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))

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
	GHWPFile* ghwp_file;
	gchar* prv_text;
	GArray* office_text;
	GArray* pages;
	GsfDocMetaData* summary_info;
};

struct _GHWPDocumentClass {
	GObjectClass parent_class;
};

struct _GHWPFileHeader {
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

struct _GHWPFile {
	GObject parent_instance;
	GHWPFilePrivate * priv;
	GHWPFileHeader header;
	GInputStream* prv_text_stream;
	GInputStream* prv_image_stream;
	GInputStream* file_header_stream;
	GInputStream* doc_info_stream;
	GArray* section_streams;
	GInputStream* summary_info_stream;
};

struct _GHWPFileClass {
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

struct _GHWPPage {
	GObject parent_instance;
	GHWPPagePrivate * priv;
	GArray* elements;
};

struct _GHWPPageClass {
	GObjectClass parent_class;
};


static gpointer text_p_parent_class = NULL;
static gpointer text_span_parent_class = NULL;
static gpointer ghwp_document_parent_class = NULL;
static gpointer ghwp_page_parent_class = NULL;

GType text_p_get_type (void) G_GNUC_CONST;
GType text_span_get_type (void) G_GNUC_CONST;
enum  {
	TEXT_P_DUMMY_PROPERTY
};
void text_p_add_textspan (TextP* self, TextSpan* textspan);
TextP* text_p_new (void);
TextP* text_p_construct (GType object_type);
static void text_p_finalize (GObject* obj);
enum  {
	TEXT_SPAN_DUMMY_PROPERTY
};
TextSpan* text_span_new (const gchar* text);
TextSpan* text_span_construct (GType object_type, const gchar* text);
static void text_span_finalize (GObject* obj);
GType ghwp_document_get_type (void) G_GNUC_CONST;
GType ghwp_file_get_type (void) G_GNUC_CONST;
GType ghwp_page_get_type (void) G_GNUC_CONST;
enum  {
	GHWP_DOCUMENT_DUMMY_PROPERTY
};
GHWPDocument* ghwp_document_new_from_uri (const gchar* uri, GError** error);
GHWPDocument* ghwp_document_construct_from_uri (GType object_type, const gchar* uri, GError** error);
GHWPFile* ghwp_file_new_from_uri (const gchar* uri, GError** error);
GHWPFile* ghwp_file_construct_from_uri (GType object_type, const gchar* uri, GError** error);
static void ghwp_document_init (GHWPDocument* self);
GHWPDocument* ghwp_document_new_from_filename (const gchar* filename, GError** error);
GHWPDocument* ghwp_document_construct_from_filename (GType object_type, const gchar* filename, GError** error);
GHWPFile* ghwp_file_new_from_filename (const gchar* filename, GError** error);
GHWPFile* ghwp_file_construct_from_filename (GType object_type, const gchar* filename, GError** error);
static void ghwp_document_parse_doc_info (GHWPDocument* self);
static void ghwp_document_parse_body_text (GHWPDocument* self);
static void ghwp_document_parse_prv_text (GHWPDocument* self);
static void ghwp_document_parse_summary_info (GHWPDocument* self);
guint ghwp_document_get_n_pages (GHWPDocument* self);
GHWPPage* ghwp_document_get_page (GHWPDocument* self, gint n_page);
GType ghwp_file_header_get_type (void) G_GNUC_CONST;
GHWPFileHeader* ghwp_file_header_dup (const GHWPFileHeader* self);
void ghwp_file_header_free (GHWPFileHeader* self);
void ghwp_file_header_copy (const GHWPFileHeader* self, GHWPFileHeader* dest);
void ghwp_file_header_destroy (GHWPFileHeader* self);
GHWPContext* ghwp_context_new (GInputStream* stream);
GHWPContext* ghwp_context_construct (GType object_type, GInputStream* stream);
GType ghwp_context_get_type (void) G_GNUC_CONST;
gboolean ghwp_context_pull (GHWPContext* self);
static gchar* ghwp_document_get_text_from_raw_data (GHWPDocument* self, guchar* raw, int raw_length1);
#define GHWP_TAG_PARA_HEADER ((guint16) 66)
#define GHWP_TAG_PARA_TEXT ((guint16) 67)
#define GHWP_TAG_PARA_CHAR_SHAPE ((guint16) 68)
#define GHWP_TAG_PARA_LINE_SEG ((guint16) 69)
#define GHWP_TAG_CTRL_HEADER ((guint16) 71)
#define GHWP_TAG_PAGE_DEF ((guint16) 73)
#define GHWP_TAG_FOOTNOTE_SHAPE ((guint16) 74)
#define GHWP_TAG_PAGE_BORDER_FILL ((guint16) 75)
#define GHWP_TAG_LIST_HEADER ((guint16) 72)
#define GHWP_TAG_EQEDIT ((guint16) 88)
static void ghwp_document_make_pages (GHWPDocument* self);
GHWPPage* ghwp_page_new (void);
GHWPPage* ghwp_page_construct (GType object_type);
GType gsf_input_stream_get_type (void) G_GNUC_CONST;
gssize gsf_input_stream_size (GsfInputStream* self);
GHWPDocument* ghwp_document_new (void);
GHWPDocument* ghwp_document_construct (GType object_type);
static void ghwp_document_finalize (GObject* obj);
enum  {
	GHWP_PAGE_DUMMY_PROPERTY
};
void ghwp_page_get_size (GHWPPage* self, gdouble* width, gdouble* height);
gboolean ghwp_page_render (GHWPPage* self, cairo_t* cr);
static gboolean ghwp_page_draw_page (cairo_t* cr, GArray* elements);
static void ghwp_page_finalize (GObject* obj);

extern const gchar* GHWP_TAG_NAMES[116];

static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


void text_p_add_textspan (TextP* self, TextSpan* textspan) {
	GArray* _tmp0_;
	TextSpan* _tmp1_;
	TextSpan* _tmp2_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (textspan != NULL);
	_tmp0_ = self->textspans;
	_tmp1_ = textspan;
	_tmp2_ = _g_object_ref0 (_tmp1_);
	g_array_append_val (_tmp0_, _tmp2_);
}


TextP* text_p_construct (GType object_type) {
	TextP * self = NULL;
	self = (TextP*) g_object_new (object_type, NULL);
	return self;
}


TextP* text_p_new (void) {
	return text_p_construct (TYPE_TEXT_P);
}


static void text_p_class_init (TextPClass * klass) {
	text_p_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->finalize = text_p_finalize;
}


static void text_p_instance_init (TextP * self) {
	GArray* _tmp0_;
	_tmp0_ = g_array_new (TRUE, TRUE, sizeof (TextSpan*));
	self->textspans = _tmp0_;
}


static void text_p_finalize (GObject* obj) {
	TextP * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, TYPE_TEXT_P, TextP);
	_g_array_free0 (self->textspans);
	G_OBJECT_CLASS (text_p_parent_class)->finalize (obj);
}


GType text_p_get_type (void) {
	static volatile gsize text_p_type_id__volatile = 0;
	if (g_once_init_enter (&text_p_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (TextPClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) text_p_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (TextP), 0, (GInstanceInitFunc) text_p_instance_init, NULL };
		GType text_p_type_id;
		text_p_type_id = g_type_register_static (G_TYPE_OBJECT, "TextP", &g_define_type_info, 0);
		g_once_init_leave (&text_p_type_id__volatile, text_p_type_id);
	}
	return text_p_type_id__volatile;
}


TextSpan* text_span_construct (GType object_type, const gchar* text) {
	TextSpan * self = NULL;
	const gchar* _tmp0_;
	gchar* _tmp1_;
	g_return_val_if_fail (text != NULL, NULL);
	self = (TextSpan*) g_object_new (object_type, NULL);
	_tmp0_ = text;
	_tmp1_ = g_strdup (_tmp0_);
	_g_free0 (self->text);
	self->text = _tmp1_;
	return self;
}


TextSpan* text_span_new (const gchar* text) {
	return text_span_construct (TYPE_TEXT_SPAN, text);
}


static void text_span_class_init (TextSpanClass * klass) {
	text_span_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->finalize = text_span_finalize;
}


static void text_span_instance_init (TextSpan * self) {
}


static void text_span_finalize (GObject* obj) {
	TextSpan * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, TYPE_TEXT_SPAN, TextSpan);
	_g_free0 (self->text);
	G_OBJECT_CLASS (text_span_parent_class)->finalize (obj);
}


GType text_span_get_type (void) {
	static volatile gsize text_span_type_id__volatile = 0;
	if (g_once_init_enter (&text_span_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (TextSpanClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) text_span_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (TextSpan), 0, (GInstanceInitFunc) text_span_instance_init, NULL };
		GType text_span_type_id;
		text_span_type_id = g_type_register_static (G_TYPE_OBJECT, "TextSpan", &g_define_type_info, 0);
		g_once_init_leave (&text_span_type_id__volatile, text_span_type_id);
	}
	return text_span_type_id__volatile;
}


GHWPDocument* ghwp_document_construct_from_uri (GType object_type, const gchar* uri, GError** error) {
	GHWPDocument * self = NULL;
	const gchar* _tmp0_;
	GHWPFile* _tmp1_;
	GHWPFile* _tmp2_;
	GError * _inner_error_ = NULL;
	g_return_val_if_fail (uri != NULL, NULL);
	self = (GHWPDocument*) g_object_new (object_type, NULL);
	_tmp0_ = uri;
	_tmp1_ = ghwp_file_new_from_uri (_tmp0_, &_inner_error_);
	_tmp2_ = _tmp1_;
	if (_inner_error_ != NULL) {
		g_propagate_error (error, _inner_error_);
		_g_object_unref0 (self);
		return NULL;
	}
	_g_object_unref0 (self->ghwp_file);
	self->ghwp_file = _tmp2_;
	ghwp_document_init (self);
	return self;
}


GHWPDocument* ghwp_document_new_from_uri (const gchar* uri, GError** error) {
	return ghwp_document_construct_from_uri (GHWP_TYPE_DOCUMENT, uri, error);
}


GHWPDocument* ghwp_document_construct_from_filename (GType object_type, const gchar* filename, GError** error) {
	GHWPDocument * self = NULL;
	const gchar* _tmp0_;
	GHWPFile* _tmp1_;
	GHWPFile* _tmp2_;
	GError * _inner_error_ = NULL;
	g_return_val_if_fail (filename != NULL, NULL);
	self = (GHWPDocument*) g_object_new (object_type, NULL);
	_tmp0_ = filename;
	_tmp1_ = ghwp_file_new_from_filename (_tmp0_, &_inner_error_);
	_tmp2_ = _tmp1_;
	if (_inner_error_ != NULL) {
		g_propagate_error (error, _inner_error_);
		_g_object_unref0 (self);
		return NULL;
	}
	_g_object_unref0 (self->ghwp_file);
	self->ghwp_file = _tmp2_;
	ghwp_document_init (self);
	return self;
}


GHWPDocument* ghwp_document_new_from_filename (const gchar* filename, GError** error) {
	return ghwp_document_construct_from_filename (GHWP_TYPE_DOCUMENT, filename, error);
}


static void ghwp_document_init (GHWPDocument* self) {
	g_return_if_fail (self != NULL);
	ghwp_document_parse_doc_info (self);
	ghwp_document_parse_body_text (self);
	ghwp_document_parse_prv_text (self);
	ghwp_document_parse_summary_info (self);
}


guint ghwp_document_get_n_pages (GHWPDocument* self) {
    g_return_val_if_fail (self != NULL, 0U);
    return self->pages->len;
}


GHWPPage* ghwp_document_get_page (GHWPDocument* self, gint n_page) {
	GHWPPage* result = NULL;
	GArray* _tmp0_;
	gint _tmp1_;
	GHWPPage* _tmp2_ = NULL;
	GHWPPage* _tmp3_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->pages;
	_tmp1_ = n_page;
	_tmp2_ = g_array_index (_tmp0_, GHWPPage*, (guint) _tmp1_);
	_tmp3_ = _g_object_ref0 (_tmp2_);
	result = _tmp3_;
	return result;
}


static void ghwp_document_parse_doc_info (GHWPDocument* self) {
	GHWPFile* _tmp0_;
	GInputStream* _tmp1_;
	GHWPContext* _tmp2_;
	GHWPContext* context;
	g_return_if_fail (self != NULL);
	_tmp0_ = self->ghwp_file;
	_tmp1_ = _tmp0_->doc_info_stream;
	_tmp2_ = ghwp_context_new (_tmp1_);
	context = _tmp2_;
	while (TRUE) {
		GHWPContext* _tmp3_;
		gboolean _tmp4_ = FALSE;
		_tmp3_ = context;
		_tmp4_ = ghwp_context_pull (_tmp3_);
		if (!_tmp4_) {
			break;
		}
	}
	_g_object_unref0 (context);
}


static gchar* g_unichar_to_string (gunichar self) {
	gchar* result = NULL;
	gchar* _tmp0_ = NULL;
	gchar* str;
	_tmp0_ = g_new0 (gchar, 7);
	str = (gchar*) _tmp0_;
	g_unichar_to_utf8 (self, str);
	result = str;
	return result;
}


static gchar* ghwp_document_get_text_from_raw_data (GHWPDocument* self, guchar* raw, int raw_length1) {
	gchar* result = NULL;
	gunichar ch = 0U;
	gchar* _tmp0_;
	gchar* text;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = g_strdup ("");
	text = _tmp0_;
	{
		gint i;
		i = 0;
		{
			gboolean _tmp1_;
			_tmp1_ = TRUE;
			while (TRUE) {
				gboolean _tmp2_;
				gint _tmp4_;
				gint _tmp5__length1;
				guchar* _tmp6_;
				gint _tmp7_;
				guchar _tmp8_;
				guchar* _tmp9_;
				gint _tmp10_;
				guchar _tmp11_;
				gunichar _tmp12_;
				_tmp2_ = _tmp1_;
				if (!_tmp2_) {
					gint _tmp3_;
					_tmp3_ = i;
					i = _tmp3_ + 2;
				}
				_tmp1_ = FALSE;
				_tmp4_ = i;
				_tmp5__length1 = raw_length1;
				if (!(_tmp4_ < _tmp5__length1)) {
					break;
				}
				_tmp6_ = raw;
				_tmp7_ = i;
				_tmp8_ = _tmp6_[_tmp7_ + 1];
				_tmp9_ = raw;
				_tmp10_ = i;
				_tmp11_ = _tmp9_[_tmp10_];
				ch = (gunichar) ((_tmp8_ << 8) | _tmp11_);
				_tmp12_ = ch;
				switch (_tmp12_) {
					case 0:
					{
						break;
					}
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					{
						gint _tmp13_;
						_tmp13_ = i;
						i = _tmp13_ + 14;
						break;
					}
					case 9:
					{
						gint _tmp14_;
						const gchar* _tmp15_;
						gunichar _tmp16_;
						gchar* _tmp17_ = NULL;
						gchar* _tmp18_;
						gchar* _tmp19_;
						_tmp14_ = i;
						i = _tmp14_ + 14;
						_tmp15_ = text;
						_tmp16_ = ch;
						_tmp17_ = g_unichar_to_string (_tmp16_);
						_tmp18_ = _tmp17_;
						_tmp19_ = g_strconcat (_tmp15_, _tmp18_, NULL);
						_g_free0 (text);
						text = _tmp19_;
						_g_free0 (_tmp18_);
						break;
					}
					case 10:
					{
						break;
					}
					case 11:
					case 12:
					{
						gint _tmp20_;
						_tmp20_ = i;
						i = _tmp20_ + 14;
						break;
					}
					case 13:
					{
						break;
					}
					case 14:
					case 15:
					case 16:
					case 17:
					case 18:
					case 19:
					case 20:
					case 21:
					case 22:
					case 23:
					{
						gint _tmp21_;
						_tmp21_ = i;
						i = _tmp21_ + 14;
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
					{
						break;
					}
					default:
					{
						const gchar* _tmp22_;
						gunichar _tmp23_;
						gchar* _tmp24_ = NULL;
						gchar* _tmp25_;
						gchar* _tmp26_;
						_tmp22_ = text;
						_tmp23_ = ch;
						_tmp24_ = g_unichar_to_string (_tmp23_);
						_tmp25_ = _tmp24_;
						_tmp26_ = g_strconcat (_tmp22_, _tmp25_, NULL);
						_g_free0 (text);
						text = _tmp26_;
						_g_free0 (_tmp25_);
						break;
					}
				}
			}
		}
	}
	result = text;
	return result;
}


static void ghwp_document_parse_body_text (GHWPDocument* self) {
	guint curr_lv;
	guint prev_lv;
	g_return_if_fail (self != NULL);
	curr_lv = (guint) 0;
	prev_lv = (guint) 0;
	{
		guint index;
		index = (guint) 0;
		{
			gboolean _tmp0_;
			_tmp0_ = TRUE;
			while (TRUE) {
				gboolean _tmp1_;
				guint _tmp3_;
				GHWPFile* _tmp4_;
				GArray* _tmp5_;
				guint _tmp6_;
				GHWPFile* _tmp7_;
				GArray* _tmp8_;
				guint _tmp9_;
				GInputStream* _tmp10_ = NULL;
				GInputStream* _tmp11_;
				GInputStream* section_stream;
				GInputStream* _tmp12_;
				GHWPContext* _tmp13_;
				GHWPContext* context;
				_tmp1_ = _tmp0_;
				if (!_tmp1_) {
					guint _tmp2_;
					_tmp2_ = index;
					index = _tmp2_ + 1;
				}
				_tmp0_ = FALSE;
				_tmp3_ = index;
				_tmp4_ = self->ghwp_file;
				_tmp5_ = _tmp4_->section_streams;
				_tmp6_ = _tmp5_->len;
				if (!(_tmp3_ < _tmp6_)) {
					break;
				}
				_tmp7_ = self->ghwp_file;
				_tmp8_ = _tmp7_->section_streams;
				_tmp9_ = index;
				_tmp10_ = g_array_index (_tmp8_, GInputStream*, _tmp9_);
				_tmp11_ = _g_object_ref0 (_tmp10_);
				section_stream = _tmp11_;
				_tmp12_ = section_stream;
				_tmp13_ = ghwp_context_new (_tmp12_);
				context = _tmp13_;
				while (TRUE) {
					GHWPContext* _tmp14_;
					gboolean _tmp15_ = FALSE;
					GHWPContext* _tmp22_;
					guint16 _tmp23_;
					GHWPContext* _tmp24_;
					guint16 _tmp25_;
					guint _tmp60_;
					_tmp14_ = context;
					_tmp15_ = ghwp_context_pull (_tmp14_);
					if (!_tmp15_) {
						break;
					}
					{
						gint i;
						i = 0;
						{
							gboolean _tmp16_;
							_tmp16_ = TRUE;
							while (TRUE) {
								gboolean _tmp17_;
								gint _tmp19_;
								GHWPContext* _tmp20_;
								guint16 _tmp21_;
								_tmp17_ = _tmp16_;
								if (!_tmp17_) {
									gint _tmp18_;
									_tmp18_ = i;
									i = _tmp18_ + 1;
								}
								_tmp16_ = FALSE;
								_tmp19_ = i;
								_tmp20_ = context;
								_tmp21_ = _tmp20_->level;
								if (!(_tmp19_ < ((gint) _tmp21_))) {
									break;
								}
							}
						}
					}
					_tmp22_ = context;
					_tmp23_ = _tmp22_->level;
					curr_lv = (guint) _tmp23_;
					_tmp24_ = context;
					_tmp25_ = _tmp24_->tag_id;
					switch (_tmp25_) {
						case GHWP_TAG_PARA_HEADER:
						{
							guint _tmp26_;
							guint _tmp27_;
							_tmp26_ = curr_lv;
							_tmp27_ = prev_lv;
							if (_tmp26_ > _tmp27_) {
								GArray* _tmp28_;
								TextP* _tmp29_;
								_tmp28_ = self->office_text;
								_tmp29_ = text_p_new ();
								g_array_append_val (_tmp28_, _tmp29_);
							} else {
								guint _tmp30_;
								guint _tmp31_;
								_tmp30_ = curr_lv;
								_tmp31_ = prev_lv;
								if (_tmp30_ < _tmp31_) {
									GArray* _tmp32_;
									TextP* _tmp33_;
									_tmp32_ = self->office_text;
									_tmp33_ = text_p_new ();
									g_array_append_val (_tmp32_, _tmp33_);
								} else {
									guint _tmp34_;
									guint _tmp35_;
									_tmp34_ = curr_lv;
									_tmp35_ = prev_lv;
									if (_tmp34_ == _tmp35_) {
										GArray* _tmp36_;
										TextP* _tmp37_;
										_tmp36_ = self->office_text;
										_tmp37_ = text_p_new ();
										g_array_append_val (_tmp36_, _tmp37_);
									}
								}
							}
							break;
						}
						case GHWP_TAG_PARA_TEXT:
						{
							guint _tmp38_;
							guint _tmp39_;
							_tmp38_ = curr_lv;
							_tmp39_ = prev_lv;
							if (_tmp38_ > _tmp39_) {
								GArray* _tmp40_;
								GArray* _tmp41_;
								guint _tmp42_;
								TextP* _tmp43_ = NULL;
								TextP* _tmp44_;
								TextP* textp;
								GHWPContext* _tmp45_;
								guchar* _tmp46_;
								gint _tmp46__length1;
								gchar* _tmp47_ = NULL;
								gchar* text;
								TextP* _tmp48_;
								const gchar* _tmp49_;
								TextSpan* _tmp50_;
								TextSpan* _tmp51_;
								_tmp40_ = self->office_text;
								_tmp41_ = self->office_text;
								_tmp42_ = _tmp41_->len;
								_tmp43_ = g_array_index (_tmp40_, TextP*, _tmp42_ - 1);
								_tmp44_ = _g_object_ref0 (_tmp43_);
								textp = _tmp44_;
								_tmp45_ = context;
								_tmp46_ = _tmp45_->data;
								_tmp46__length1 = _tmp45_->data_length1;
								_tmp47_ = ghwp_document_get_text_from_raw_data (self, _tmp46_, _tmp46__length1);
								text = _tmp47_;
								_tmp48_ = textp;
								_tmp49_ = text;
								_tmp50_ = text_span_new (_tmp49_);
								_tmp51_ = _tmp50_;
								text_p_add_textspan (G_TYPE_CHECK_INSTANCE_CAST (_tmp48_, TYPE_TEXT_P, TextP), _tmp51_);
								_g_object_unref0 (_tmp51_);
								_g_free0 (text);
								_g_object_unref0 (textp);
							} else {
								guint _tmp52_;
								guint _tmp53_;
								_tmp52_ = curr_lv;
								_tmp53_ = prev_lv;
								if (_tmp52_ < _tmp53_) {
								} else {
									guint _tmp54_;
									guint _tmp55_;
									_tmp54_ = curr_lv;
									_tmp55_ = prev_lv;
									if (_tmp54_ == _tmp55_) {
									}
								}
							}
							break;
						}
						case GHWP_TAG_PARA_CHAR_SHAPE:
						{
							break;
						}
						case GHWP_TAG_PARA_LINE_SEG:
						{
							break;
						}
						case GHWP_TAG_CTRL_HEADER:
						{
							break;
						}
						case GHWP_TAG_PAGE_DEF:
						{
							break;
						}
						case GHWP_TAG_FOOTNOTE_SHAPE:
						{
							break;
						}
						case GHWP_TAG_PAGE_BORDER_FILL:
						{
							break;
						}
						case GHWP_TAG_LIST_HEADER:
						{
							break;
						}
						case GHWP_TAG_EQEDIT:
						{
							break;
						}
						default:
						{
							FILE* _tmp56_;
							GHWPContext* _tmp57_;
							guint16 _tmp58_;
							const gchar* _tmp59_;
							_tmp56_ = stderr;
							_tmp57_ = context;
							_tmp58_ = _tmp57_->tag_id;
							_tmp59_ = GHWP_TAG_NAMES[_tmp58_];
							fprintf (_tmp56_, "%s: not implemented\n", _tmp59_);
							break;
						}
					}
					_tmp60_ = curr_lv;
					prev_lv = _tmp60_;
				}
				_g_object_unref0 (context);
				_g_object_unref0 (section_stream);
			}
		}
	}
	ghwp_document_make_pages (self);
}


static void ghwp_document_make_pages (GHWPDocument* self) {
	gdouble y;
	guint len;
	GHWPPage* _tmp0_;
	GHWPPage* page;
	GArray* _tmp43_;
	GHWPPage* _tmp44_;
	GHWPPage* _tmp45_;
	g_return_if_fail (self != NULL);
	y = 0.0;
	len = (guint) 0;
	_tmp0_ = ghwp_page_new ();
	page = _tmp0_;
	{
		gint i;
		i = 0;
		{
			gboolean _tmp1_;
			_tmp1_ = TRUE;
			while (TRUE) {
				gboolean _tmp2_;
				gint _tmp4_;
				GArray* _tmp5_;
				guint _tmp6_;
				GArray* _tmp7_;
				gint _tmp8_;
				TextP* _tmp9_ = NULL;
				TextP* _tmp10_;
				TextP* textp;
				_tmp2_ = _tmp1_;
				if (!_tmp2_) {
					gint _tmp3_;
					_tmp3_ = i;
					i = _tmp3_ + 1;
				}
				_tmp1_ = FALSE;
				_tmp4_ = i;
				_tmp5_ = self->office_text;
				_tmp6_ = _tmp5_->len;
				if (!(((guint) _tmp4_) < _tmp6_)) {
					break;
				}
				_tmp7_ = self->office_text;
				_tmp8_ = i;
				_tmp9_ = g_array_index (_tmp7_, TextP*, (guint) _tmp8_);
				_tmp10_ = _g_object_ref0 (_tmp9_);
				textp = _tmp10_;
				{
					gint j;
					j = 0;
					{
						gboolean _tmp11_;
						_tmp11_ = TRUE;
						while (TRUE) {
							gboolean _tmp12_;
							gint _tmp14_;
							TextP* _tmp15_;
							GArray* _tmp16_;
							guint _tmp17_;
							TextP* _tmp18_;
							GArray* _tmp19_;
							gint _tmp20_;
							TextSpan* _tmp21_ = NULL;
							TextSpan* _tmp22_;
							TextSpan* textspan;
							TextSpan* _tmp23_;
							const gchar* _tmp24_;
							gint _tmp25_;
							gint _tmp26_;
							gdouble _tmp27_;
							guint _tmp28_;
							gdouble _tmp29_ = 0.0;
							gdouble _tmp30_;
							_tmp12_ = _tmp11_;
							if (!_tmp12_) {
								gint _tmp13_;
								_tmp13_ = j;
								j = _tmp13_ + 1;
							}
							_tmp11_ = FALSE;
							_tmp14_ = j;
							_tmp15_ = textp;
							_tmp16_ = _tmp15_->textspans;
							_tmp17_ = _tmp16_->len;
							if (!(((guint) _tmp14_) < _tmp17_)) {
								break;
							}
							_tmp18_ = textp;
							_tmp19_ = _tmp18_->textspans;
							_tmp20_ = j;
							_tmp21_ = g_array_index (_tmp19_, TextSpan*, (guint) _tmp20_);
							_tmp22_ = _g_object_ref0 (_tmp21_);
							textspan = _tmp22_;
							_tmp23_ = textspan;
							_tmp24_ = _tmp23_->text;
							_tmp25_ = strlen (_tmp24_);
							_tmp26_ = _tmp25_;
							len = (guint) _tmp26_;
							_tmp27_ = y;
							_tmp28_ = len;
							_tmp29_ = ceil (_tmp28_ / 100.0);
							y = _tmp27_ + (18.0 * _tmp29_);
							_tmp30_ = y;
							if (_tmp30_ > (842.0 - 80.0)) {
								GArray* _tmp31_;
								GHWPPage* _tmp32_;
								GHWPPage* _tmp33_;
								GHWPPage* _tmp34_;
								GHWPPage* _tmp35_;
								GArray* _tmp36_;
								TextSpan* _tmp37_;
								GObject* _tmp38_;
								_tmp31_ = self->pages;
								_tmp32_ = page;
								_tmp33_ = _g_object_ref0 (_tmp32_);
								g_array_append_val (_tmp31_, _tmp33_);
								_tmp34_ = ghwp_page_new ();
								_g_object_unref0 (page);
								page = _tmp34_;
								_tmp35_ = page;
								_tmp36_ = _tmp35_->elements;
								_tmp37_ = textspan;
								_tmp38_ = _g_object_ref0 ((GObject*) _tmp37_);
								g_array_append_val (_tmp36_, _tmp38_);
								y = 0.0;
							} else {
								GHWPPage* _tmp39_;
								GArray* _tmp40_;
								TextSpan* _tmp41_;
								GObject* _tmp42_;
								_tmp39_ = page;
								_tmp40_ = _tmp39_->elements;
								_tmp41_ = textspan;
								_tmp42_ = _g_object_ref0 ((GObject*) _tmp41_);
								g_array_append_val (_tmp40_, _tmp42_);
							}
							_g_object_unref0 (textspan);
						}
					}
				}
				_g_object_unref0 (textp);
			}
		}
	}
	_tmp43_ = self->pages;
	_tmp44_ = page;
	_tmp45_ = _g_object_ref0 (_tmp44_);
	g_array_append_val (_tmp43_, _tmp45_);
	_g_object_unref0 (page);
}


static void ghwp_document_parse_prv_text (GHWPDocument* self) {
	GHWPFile* _tmp0_;
	GInputStream* _tmp1_;
	GsfInputStream* _tmp2_;
	GsfInputStream* gis;
	GsfInputStream* _tmp3_;
	gssize _tmp4_ = 0L;
	gssize size;
	gssize _tmp5_;
	guchar* _tmp6_ = NULL;
	guchar* buf;
	gint buf_length1;
	GError * _inner_error_ = NULL;
	g_return_if_fail (self != NULL);
	_tmp0_ = self->ghwp_file;
	_tmp1_ = _tmp0_->prv_text_stream;
	_tmp2_ = _g_object_ref0 (G_TYPE_CHECK_INSTANCE_CAST (_tmp1_, TYPE_GSF_INPUT_STREAM, GsfInputStream));
	gis = _tmp2_;
	_tmp3_ = gis;
	_tmp4_ = gsf_input_stream_size (_tmp3_);
	size = _tmp4_;
	_tmp5_ = size;
	_tmp6_ = g_new0 (guchar, _tmp5_);
	buf = _tmp6_;
	buf_length1 = _tmp5_;
	{
		GsfInputStream* _tmp7_;
		guchar* _tmp8_;
		gint _tmp8__length1;
		guchar* _tmp9_;
		gssize _tmp10_;
		gchar* _tmp11_ = NULL;
		gchar* _tmp12_;
		_tmp7_ = gis;
		_tmp8_ = buf;
		_tmp8__length1 = buf_length1;
		g_input_stream_read ((GInputStream*) _tmp7_, _tmp8_, (gsize) _tmp8__length1, NULL, &_inner_error_);
		if (_inner_error_ != NULL) {
			goto __catch3_g_error;
		}
		_tmp9_ = buf;
		_tmp10_ = size;
		_tmp11_ = g_convert ((const gchar*) _tmp9_, (gssize) _tmp10_, "UTF-8", "UTF-16LE", NULL, NULL, &_inner_error_);
		_tmp12_ = _tmp11_;
		if (_inner_error_ != NULL) {
			goto __catch3_g_error;
		}
		_g_free0 (self->prv_text);
		self->prv_text = _tmp12_;
	}
	goto __finally3;
	__catch3_g_error:
	{
		GError* e = NULL;
		const gchar* _tmp13_;
		e = _inner_error_;
		_inner_error_ = NULL;
		_tmp13_ = e->message;
		g_error ("document.vala:266: %s", _tmp13_);
		_g_error_free0 (e);
	}
	__finally3:
	if (_inner_error_ != NULL) {
		buf = (g_free (buf), NULL);
		_g_object_unref0 (gis);
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return;
	}
	buf = (g_free (buf), NULL);
	_g_object_unref0 (gis);
}


static void ghwp_document_parse_summary_info (GHWPDocument* self) {
	GHWPFile* _tmp0_;
	GInputStream* _tmp1_;
	GsfInputStream* _tmp2_;
	GsfInputStream* gis;
	gssize _tmp3_ = 0L;
	gssize size;
	guchar* _tmp4_ = NULL;
	guchar* buf;
	gint buf_length1;
	guint8* _tmp6_ = NULL;
	guint8* component_guid;
	gint component_guid_length1;
	GsfInputMemory* _tmp7_;
	GsfInputMemory* summary;
	GsfDocMetaData* _tmp8_;
	GsfDocMetaData* meta;
	GsfDocMetaData* _tmp9_;
	GError * _inner_error_ = NULL;
	g_return_if_fail (self != NULL);
	_tmp0_ = self->ghwp_file;
	_tmp1_ = _tmp0_->summary_info_stream;
	_tmp2_ = _g_object_ref0 (G_TYPE_CHECK_INSTANCE_CAST (_tmp1_, TYPE_GSF_INPUT_STREAM, GsfInputStream));
	gis = _tmp2_;
	_tmp3_ = gsf_input_stream_size (gis);
	size = _tmp3_;
	_tmp4_ = g_new0 (guchar, size);
	buf = _tmp4_;
	buf_length1 = size;
	{
		g_input_stream_read ((GInputStream*) gis, buf, (gsize) buf_length1, NULL, &_inner_error_);
		if (_inner_error_ != NULL) {
			goto __catch4_g_error;
		}
	}
	goto __finally4;
	__catch4_g_error:
	{
		GError* e = NULL;
		const gchar* _tmp5_;
		e = _inner_error_;
		_inner_error_ = NULL;
		_tmp5_ = e->message;
		g_error ("document.vala:279: %s", _tmp5_);
		_g_error_free0 (e);
	}
	__finally4:
	if (_inner_error_ != NULL) {
		buf = (g_free (buf), NULL);
		_g_object_unref0 (gis);
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return;
	}
	_tmp6_ = g_new0 (guint8, 16);
	_tmp6_[0] = (guint8) 0xe0;
	_tmp6_[1] = (guint8) 0x85;
	_tmp6_[2] = (guint8) 0x9f;
	_tmp6_[3] = (guint8) 0xf2;
	_tmp6_[4] = (guint8) 0xf9;
	_tmp6_[5] = (guint8) 0x4f;
	_tmp6_[6] = (guint8) 0x68;
	_tmp6_[7] = (guint8) 0x10;
	_tmp6_[8] = (guint8) 0xab;
	_tmp6_[9] = (guint8) 0x91;
	_tmp6_[10] = (guint8) 0x08;
	_tmp6_[11] = (guint8) 0x00;
	_tmp6_[12] = (guint8) 0x2b;
	_tmp6_[13] = (guint8) 0x27;
	_tmp6_[14] = (guint8) 0xb3;
	_tmp6_[15] = (guint8) 0xd9;
	component_guid = _tmp6_;
	component_guid_length1 = 16;
	memcpy (buf + ((guchar) 28), component_guid, (gsize) component_guid_length1);
	_tmp7_ = (GsfInputMemory*) gsf_input_memory_new (buf, buf_length1, FALSE);
	summary = _tmp7_;
	_tmp8_ = gsf_doc_meta_data_new ();
	meta = _tmp8_;
	gsf_msole_metadata_read ((GsfInput*) summary, meta);
	_tmp9_ = _g_object_ref0 (meta);
	_g_object_unref0 (self->summary_info);
	self->summary_info = _tmp9_;
	_g_object_unref0 (meta);
	_g_object_unref0 (summary);
	component_guid = (g_free (component_guid), NULL);
	buf = (g_free (buf), NULL);
	_g_object_unref0 (gis);
}


GHWPDocument* ghwp_document_construct (GType object_type) {
	GHWPDocument * self = NULL;
	self = (GHWPDocument*) g_object_new (object_type, NULL);
	return self;
}


GHWPDocument* ghwp_document_new (void) {
	return ghwp_document_construct (GHWP_TYPE_DOCUMENT);
}


static void ghwp_document_class_init (GHWPDocumentClass * klass) {
	ghwp_document_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->finalize = ghwp_document_finalize;
}


static void ghwp_document_instance_init (GHWPDocument * self) {
	GArray* _tmp0_;
	GArray* _tmp1_;
	_tmp0_ = g_array_new (TRUE, TRUE, sizeof (TextP*));
	self->office_text = _tmp0_;
	_tmp1_ = g_array_new (TRUE, TRUE, sizeof (GHWPPage*));
	self->pages = _tmp1_;
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


GType ghwp_document_get_type (void) {
	static volatile gsize ghwp_document_type_id__volatile = 0;
	if (g_once_init_enter (&ghwp_document_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (GHWPDocumentClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) ghwp_document_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GHWPDocument), 0, (GInstanceInitFunc) ghwp_document_instance_init, NULL };
		GType ghwp_document_type_id;
		ghwp_document_type_id = g_type_register_static (G_TYPE_OBJECT, "GHWPDocument", &g_define_type_info, 0);
		g_once_init_leave (&ghwp_document_type_id__volatile, ghwp_document_type_id);
	}
	return ghwp_document_type_id__volatile;
}


void ghwp_page_get_size (GHWPPage* self,
                         gdouble*  width,
                         gdouble*  height)
{
    g_return_if_fail (self != NULL);
    *width  = 595.0;
    *height = 842.0;
}


gboolean ghwp_page_render (GHWPPage* self, cairo_t* cr) {
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (cr != NULL, FALSE);
    cairo_save (cr);
    ghwp_page_draw_page (cr, self->elements);
    cairo_restore (cr);
    return TRUE;
}


#include <cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

static FT_Library ft_lib;
static FT_Face    ft_face;
/*한 번만 초기화, 로드*/
static void
once_ft_init_and_new (void)
{
    static gsize ft_init = 0;

    if (g_once_init_enter (&ft_init)) {

        FT_Init_FreeType (&ft_lib);
        FT_New_Face (ft_lib,
                    "/usr/share/fonts/truetype/nanum/NanumGothic.ttf",
                     0,
                    &ft_face);

        g_once_init_leave (&ft_init, (gsize)1);
    }
}

static gboolean ghwp_page_draw_page (cairo_t* cr, GArray* elements) {
    g_return_val_if_fail (cr != NULL, FALSE);
    g_return_val_if_fail (elements != NULL, FALSE);

    gint      i, j;
    TextSpan *textspan;

    cairo_glyph_t        *glyphs = NULL; /* NULL로 지정하면 자동 할당됨 */
    int                   num_glyphs;
    cairo_scaled_font_t  *scaled_font;
    cairo_font_face_t    *font_face;
    cairo_matrix_t        font_matrix;
    cairo_matrix_t        ctm;
    cairo_font_options_t *font_options;
    cairo_text_extents_t  extents;

    gchar *ch;
    double x = 20.0;
    double y = 40.0;

    once_ft_init_and_new(); /*한 번만 초기화, 로드*/

    font_face = cairo_ft_font_face_create_for_ft_face (ft_face, 0);

    cairo_matrix_init_identity (&font_matrix);
    cairo_matrix_scale (&font_matrix, 12.0, 12.0);
    cairo_get_matrix (cr, &ctm);

    font_options = cairo_font_options_create ();
    cairo_get_font_options (cr, font_options);
    scaled_font = cairo_scaled_font_create (font_face,
                                           &font_matrix, &ctm, font_options);
    cairo_font_options_destroy (font_options);
    cairo_set_scaled_font(cr, scaled_font); /* 요 문장 없으면 fault 떨어짐 */
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

    for (i = 0; i < elements->len; i++)
    {
        textspan = g_array_index (elements, TextSpan*, (guint) i);

        x = 20.0;

        for (j = 0; j < g_utf8_strlen(textspan->text, -1); j++) {
            ch = g_utf8_substring(textspan->text, j, j+1);

            cairo_scaled_font_text_to_glyphs (scaled_font,
                                              x, y, /* x, y 좌표 */
                                              ch, -1,
                                             &glyphs, &num_glyphs,
                                              NULL, NULL, NULL);
            g_free(ch);
            cairo_glyph_extents(cr, glyphs, num_glyphs, &extents);

            if (x >= 595.0 - extents.x_advance - 20.0) {
                glyphs[0].x = 20.0;
                glyphs[0].y += 16.0;
                x = 20.0 + extents.x_advance;
                y = y + 16.0;
            }
            else {
                x = x + extents.x_advance;
            }

            cairo_show_glyphs (cr, glyphs, num_glyphs);
        }
        y = y + 18.0;

    }
    cairo_glyph_free (glyphs);
    cairo_scaled_font_destroy (scaled_font);
    return TRUE;
}


GHWPPage* ghwp_page_construct (GType object_type) {
    return (GHWPPage*) g_object_new (object_type, NULL);
}


GHWPPage* ghwp_page_new (void) {
	return ghwp_page_construct (GHWP_TYPE_PAGE);
}


static void ghwp_page_class_init (GHWPPageClass * klass) {
	ghwp_page_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->finalize = ghwp_page_finalize;
}


static void ghwp_page_instance_init (GHWPPage * self) {
    self->elements = g_array_new (TRUE, TRUE, sizeof (GObject*));
}


static void ghwp_page_finalize (GObject* obj) {
	GHWPPage * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, GHWP_TYPE_PAGE, GHWPPage);
	_g_array_free0 (self->elements);
	G_OBJECT_CLASS (ghwp_page_parent_class)->finalize (obj);
}


GType ghwp_page_get_type (void) {
	static volatile gsize ghwp_page_type_id__volatile = 0;
	if (g_once_init_enter (&ghwp_page_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (GHWPPageClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) ghwp_page_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GHWPPage), 0, (GInstanceInitFunc) ghwp_page_instance_init, NULL };
		GType ghwp_page_type_id;
		ghwp_page_type_id = g_type_register_static (G_TYPE_OBJECT, "GHWPPage", &g_define_type_info, 0);
		g_once_init_leave (&ghwp_page_type_id__volatile, ghwp_page_type_id);
	}
	return ghwp_page_type_id__volatile;
}
