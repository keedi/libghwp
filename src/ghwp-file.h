/*
 * ghwp-file.h
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

#ifndef _GHWP_FILE_H_
#define _GHWP_FILE_H_

#include <glib-object.h>
#include <gio/gio.h>
#include <gsf/gsf-infile-msole.h>

G_BEGIN_DECLS

#define GHWP_TYPE_FILE_HEADER             (ghwp_file_header_get_type ())
#define GHWP_FILE_HEADER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_FILE_HEADER, GHWPFileHeader))
#define GHWP_FILE_HEADER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_FILE_HEADER, GHWPFileHeaderClass))
#define GHWP_IS_FILE_HEADER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_FILE_HEADER))
#define GHWP_IS_FILE_HEADER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_FILE_HEADER))
#define GHWP_FILE_HEADER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_FILE_HEADER, GHWPFileHeaderClass))

typedef struct _GHWPFileHeaderClass GHWPFileHeaderClass;
typedef struct _GHWPFileHeader GHWPFileHeader;

struct _GHWPFileHeaderClass
{
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

GType ghwp_file_header_get_type (void) G_GNUC_CONST;




#define GHWP_TYPE_FILE             (ghwp_file_get_type ())
#define GHWP_FILE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_FILE, GHWPFile))
#define GHWP_FILE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_FILE, GHWPFileClass))
#define GHWP_IS_FILE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_FILE))
#define GHWP_IS_FILE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_FILE))
#define GHWP_FILE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_FILE, GHWPFileClass))

typedef struct _GHWPFile GHWPFile;
typedef struct _GHWPFileClass GHWPFileClass;
typedef struct _GHWPFilePrivate GHWPFilePrivate;

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

struct _GHWPFilePrivate {
	GsfInfileMSOle* olefile;
	GInputStream* section_stream;
};

GType ghwp_file_get_type (void) G_GNUC_CONST;

GHWPFile* ghwp_file_new_from_uri (const gchar* uri, GError** error);
GHWPFile* ghwp_file_new_from_filename (const gchar* filename, GError** error);
GHWPFile* ghwp_file_new (void);

G_END_DECLS

#endif /* _GHWP_FILE_H_ */