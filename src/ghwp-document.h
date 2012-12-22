/*
 * ghwp-document.h
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

#ifndef _GHWP_DOCUMENT_H_
#define _GHWP_DOCUMENT_H_

#include <glib-object.h>

#include "ghwp-file.h"
#include "ghwp-page.h"

G_BEGIN_DECLS

#define GHWP_TYPE_DOCUMENT             (ghwp_document_get_type ())
#define GHWP_DOCUMENT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_DOCUMENT, GHWPDocument))
#define GHWP_DOCUMENT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_DOCUMENT, GHWPDocumentClass))
#define GHWP_IS_DOCUMENT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_DOCUMENT))
#define GHWP_IS_DOCUMENT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_DOCUMENT))
#define GHWP_DOCUMENT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_DOCUMENT, GHWPDocumentClass))

typedef struct _GHWPDocument GHWPDocument;
typedef struct _GHWPDocumentClass GHWPDocumentClass;
typedef struct _GHWPDocumentPrivate GHWPDocumentPrivate;

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

GType ghwp_document_get_type (void) G_GNUC_CONST;

GHWPDocument* ghwp_document_new (void);
GHWPDocument* ghwp_document_new_from_uri (const gchar* uri, GError** error);
GHWPDocument* ghwp_document_new_from_filename (const gchar* filename, GError** error);
guint ghwp_document_get_n_pages (GHWPDocument* self);
GHWPPage* ghwp_document_get_page (GHWPDocument* self, gint n_page);




#define TEXT_TYPE_SPAN             (text_span_get_type ())
#define TEXT_SPAN(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEXT_TYPE_SPAN, TextSpan))
#define TEXT_SPAN_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), TEXT_TYPE_SPAN, TextSpanClass))
#define TEXT_IS_SPAN(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEXT_TYPE_SPAN))
#define TEXT_IS_SPAN_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), TEXT_TYPE_SPAN))
#define TEXT_SPAN_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), TEXT_TYPE_SPAN, TextSpanClass))

typedef struct _TextSpan TextSpan;
typedef struct _TextSpanClass TextSpanClass;
typedef struct _TextSpanPrivate TextSpanPrivate;

struct _TextSpan {
	GObject parent_instance;
	TextSpanPrivate * priv;
	gchar* text;
};

struct _TextSpanClass {
	GObjectClass parent_class;
};

GType text_span_get_type (void) G_GNUC_CONST;

TextSpan* text_span_new (const gchar* text);


G_END_DECLS

#endif /* _GHWP_DOCUMENT_H_ */