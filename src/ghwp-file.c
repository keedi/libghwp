/*
 * ghwp-file.c
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

#include <gsf/gsf-input-impl.h>
#include <stdlib.h>
#include <string.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-infile-impl.h>
#include <stdio.h>

#include "gsf-input-stream.h"
#include "ghwp-file.h"

G_DEFINE_TYPE (GHWPFile, ghwp_file, G_TYPE_OBJECT);
G_DEFINE_TYPE (GHWPFileHeader, ghwp_file_header, G_TYPE_OBJECT);

#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_array_free0(var) ((var == NULL) ? NULL : (var = (g_array_free (var, TRUE), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))

GHWPFileHeader* ghwp_file_header_dup (const GHWPFileHeader* self);
void ghwp_file_header_free (GHWPFileHeader* self);
void ghwp_file_header_copy (const GHWPFileHeader* self, GHWPFileHeader* dest);
void ghwp_file_header_destroy (GHWPFileHeader* self);
#define GHWP_FILE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GHWP_TYPE_FILE, GHWPFilePrivate))

static void ghwp_file_make_stream (GHWPFile* self);
static void ghwp_file_decode_file_header (GHWPFile* self);
static void ghwp_file_finalize (GObject* obj);

static gpointer _g_object_ref0 (gpointer self)
{
	return self ? g_object_ref (self) : NULL;
}


GHWPFile* ghwp_file_new_from_uri (const gchar* uri, GError** error)
{
	g_return_val_if_fail (uri != NULL, NULL);

	gchar *filename = g_filename_from_uri (uri, NULL, error);
	if (filename == NULL) {
	    g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
		return NULL;
	}

	GsfInputStdio *input;
	input = (GsfInputStdio*) gsf_input_stdio_new (filename, error);
	if (input == NULL) {
	    g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
	    _g_free0 (filename);
	    return NULL;
	}

	GsfInfileMSOle *olefile;
	olefile = (GsfInfileMSOle*) gsf_infile_msole_new ((GsfInput*) input,
	                                                  error);
	if (olefile == NULL) {
	    g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
		_g_object_unref0 (input);
	    _g_free0 (filename);
	    return NULL;
	}

	GHWPFile *self = ghwp_file_new();
	_g_object_unref0 (self->priv->olefile);
	self->priv->olefile = olefile;
	_g_object_unref0 (input);
	_g_free0 (filename);
	ghwp_file_make_stream (self);
	return self;
}


GHWPFile* ghwp_file_new_from_filename (const gchar* filename, GError** error)
{
	g_return_val_if_fail (filename != NULL, NULL);

	GFile *file = g_file_new_for_path (filename);

	GsfInputStdio* input;
	GsfInfileMSOle* olefile;

    gchar *path = g_file_get_path(file);
	input = (GsfInputStdio*) gsf_input_stdio_new (path, error);
	_g_free0 (path);

	if (input == NULL) {
		g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
		_g_object_unref0 (file);
		return NULL;
	}

	olefile = (GsfInfileMSOle*) gsf_infile_msole_new ((GsfInput*) input,
	                                                  error);

	if (olefile == NULL) {
		g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
		_g_object_unref0 (input);
		_g_object_unref0 (file);
		return NULL;
	}

	GHWPFile *self = ghwp_file_new();
	_g_object_unref0 (self->priv->olefile);
	self->priv->olefile = olefile;
	_g_object_unref0 (input);
	_g_object_unref0 (file);
	ghwp_file_make_stream (self);
	return self;
}


static void ghwp_file_make_stream (GHWPFile* self)
{
	g_return_if_fail (self != NULL);

	gint n_children;
	n_children = gsf_infile_num_children ((GsfInfile*) self->priv->olefile);

	if (n_children < 1) {
		fprintf (stderr, "invalid hwp file\n");
		return;
	}

    gint i;
	for (i = 0; i < n_children; i++) {
		const gchar* name;
		name = gsf_infile_name_by_index ((GsfInfile*) self->priv->olefile, i);
		GsfInput* input;
		gint num_children = 0;

        if (g_strcmp0 (name, "PrvText") == 0) {
			input = gsf_infile_child_by_name ((GsfInfile*) self->priv->olefile,
			                                  name);
			input = _g_object_ref0 (input);
			num_children = gsf_infile_num_children ((GsfInfile*) input);

			if (num_children > 0) {
				fprintf (stderr, "invalid\n");
			}

			_g_object_unref0 (self->prv_text_stream);
			self->prv_text_stream = (GInputStream*) gsf_input_stream_new (input);
			_g_object_unref0 (input);
		}
		else if (g_strcmp0 (name, "PrvImage") == 0) {
			input = gsf_infile_child_by_name ((GsfInfile*) self->priv->olefile,
			                                  name);
			input = _g_object_ref0 (input);
			num_children = gsf_infile_num_children ((GsfInfile*) input);

			if (num_children > 0) {
				fprintf (stderr, "invalid\n");
			}

			_g_object_unref0 (self->prv_image_stream);
			self->prv_image_stream = (GInputStream*) gsf_input_stream_new (input);
			_g_object_unref0 (input);
		}
		else if (g_strcmp0 (name, "FileHeader") == 0) {
			input = gsf_infile_child_by_name ((GsfInfile*) self->priv->olefile,
			                                  name);
			input = _g_object_ref0 (input);
			num_children = gsf_infile_num_children ((GsfInfile*) input);

			if (num_children > 0) {
				fprintf (stderr, "invalid\n");
			}

			_g_object_unref0 (self->file_header_stream);
			self->file_header_stream = (GInputStream*) gsf_input_stream_new (input);
			_g_object_unref0 (input);
			ghwp_file_decode_file_header (self);
		}
		else if (g_strcmp0 (name, "DocInfo") == 0) {
			input = gsf_infile_child_by_name ((GsfInfile*) self->priv->olefile,
			                                  name);
			input = _g_object_ref0 (input);
			num_children = gsf_infile_num_children ((GsfInfile*) input);

			if (num_children > 0) {
				fprintf (stderr, "invalid\n");
			}

			if (self->header.is_compress) {
				GsfInputStream    *gis;
				GZlibDecompressor *zd;
				GInputStream      *cis;

				gis = gsf_input_stream_new (input);
				zd = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
				cis = g_converter_input_stream_new ((GInputStream*) gis,
				                                    (GConverter*) zd);
				_g_object_unref0 (self->doc_info_stream);
				self->doc_info_stream = cis;

				_g_object_unref0 (zd);
				_g_object_unref0 (gis);
			} else {
				_g_object_unref0 (self->doc_info_stream);
				self->doc_info_stream = (GInputStream*) gsf_input_stream_new (input);
			}
			_g_object_unref0 (input);
		}
		else if ((g_strcmp0 (name, "BodyText") == 0) |
		         (g_strcmp0 (name, "VeiwText") == 0)) {

			GsfInfile* infile;

			_g_array_free0 (self->section_streams);
			self->section_streams = g_array_new (TRUE, TRUE,
			                                     sizeof (GInputStream*));

			infile = (GsfInfile*) gsf_infile_child_by_index (
			                             (GsfInfile*) self->priv->olefile, i);
			infile = _g_object_ref0 (infile);

			num_children = gsf_infile_num_children (infile);

			if (num_children == 0) {
				fprintf (stderr, "nothing in BodyText\n");
			}

            gint j;
			for (j = 0; j < num_children; j++) {
				input = gsf_infile_child_by_index (infile, j);
				GsfInfile *section = _g_object_ref0 (input);
				num_children = gsf_infile_num_children (section);

				if (num_children > 0) {
					fprintf (stderr, "invalid section\n");
				}

				if (self->header.is_compress) {
					GsfInputStream* gis;
					GZlibDecompressor* zd;
					GConverterInputStream* cis;

					gis = gsf_input_stream_new ((GsfInput*) section);
					zd = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
					cis = (GConverterInputStream*) g_converter_input_stream_new ((GInputStream*) gis, (GConverter*) zd);

					_g_object_unref0 (self->priv->section_stream);
					self->priv->section_stream = (GInputStream*) cis;
					_g_object_unref0 (zd);
					_g_object_unref0 (gis);
				} else {
					GsfInputStream* stream;
					stream = gsf_input_stream_new ((GsfInput*) section);
					_g_object_unref0 (self->priv->section_stream);
					self->priv->section_stream = (GInputStream*) stream;
				}

				GInputStream *_stream_ = self->priv->section_stream;
				_stream_ = _g_object_ref0 (_stream_);
				g_array_append_val (self->section_streams, _stream_);
				_g_object_unref0 (section);
			} /* for */
			_g_object_unref0 (infile);
		}
		else if (g_strcmp0 (name, "\005HwpSummaryInformation") == 0) {
			input = gsf_infile_child_by_name ((GsfInfile*) self->priv->olefile,
			                                  name);
			input = _g_object_ref0 (input);
			num_children = gsf_infile_num_children ((GsfInfile*) input);

			if (num_children > 0) {
				fprintf (stderr, "invalid\n");
			}

			_g_object_unref0 (self->summary_info_stream);
			self->summary_info_stream = (GInputStream*) gsf_input_stream_new (input);
			_g_object_unref0 (input);
		}
        else {
			fprintf (stderr, "not implemented error: %s\n", name);
		} /* if */
	} /* for */
}


static void ghwp_file_decode_file_header (GHWPFile* self)
{
	g_return_if_fail (self != NULL);

    GsfInputStream *gis;
	gssize size;
	guchar* buf;
	guint32 prop;
	GError *error = NULL;

	gis = (GsfInputStream *)_g_object_ref0 (self->file_header_stream);
	size = gsf_input_stream_size (gis);
	buf = g_new0 (guchar, size);

	g_input_stream_read ((GInputStream*) gis, buf,
	                     (gsize) size, NULL, &error);
	if (error != NULL) {
		buf = (g_free (buf), NULL);
		_g_object_unref0 (gis);
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)",
		    __FILE__, __LINE__, error->message,
		    g_quark_to_string (error->domain), error->code);
		g_clear_error (&error);
		return;
	}

	_g_free0 (self->header.signature);
	self->header.signature = g_strdup ((const gchar*) (buf + 0));
	self->header.version = (guint32) (buf[35] << 24 |
	                                  buf[34] << 16 |
	                                  buf[33] <<  8 |
	                                  buf[32]);
	prop = (guint32) (buf[39] << 24 |
	                  buf[38] << 16 |
	                  buf[37] <<  8 |
	                  buf[36]);
	if ((prop & (1 << 0)) == ((guint32) 1))
		self->header.is_compress = TRUE;
	if ((prop & (1 << 1)) == ((guint32) 1))
		self->header.is_encrypt = TRUE;
	if ((prop & (1 << 2)) == ((guint32) 1))
		self->header.is_distribute = TRUE;
	if ((prop & (1 << 3)) == ((guint32) 1))
		self->header.is_script = TRUE;
	if ((prop & (1 << 4)) == ((guint32) 1))
		self->header.is_drm = TRUE;
	if ((prop & (1 << 5)) == ((guint32) 1))
		self->header.is_xml_template = TRUE;
	if ((prop & (1 << 6)) == ((guint32) 1))
		self->header.is_history = TRUE;
	if ((prop & (1 << 7)) == ((guint32) 1))
		self->header.is_sign = TRUE;
	if ((prop & (1 << 8)) == ((guint32) 1))
		self->header.is_certificate_encrypt = TRUE;
	if ((prop & (1 << 9)) == ((guint32) 1))
		self->header.is_sign_spare = TRUE;
	if ((prop & (1 << 10)) == ((guint32) 1))
		self->header.is_certificate_drm = TRUE;
	if ((prop & (1 << 11)) == ((guint32) 1))
		self->header.is_ccl = TRUE;
	buf = (g_free (buf), NULL);
	_g_object_unref0 (gis);
}


GHWPFile* ghwp_file_new (void)
{
	return (GHWPFile*) g_object_new (GHWP_TYPE_FILE, NULL);
}


void ghwp_file_header_copy (const GHWPFileHeader *self, GHWPFileHeader *dest)
{
	_g_free0 ((*dest).signature);
	(*dest).signature = g_strdup ((*self).signature);
	(*dest).version = (*self).version;
	(*dest).is_compress = (*self).is_compress;
	(*dest).is_encrypt = (*self).is_encrypt;
	(*dest).is_distribute = (*self).is_distribute;
	(*dest).is_script = (*self).is_script;
	(*dest).is_drm = (*self).is_drm;
	(*dest).is_xml_template = (*self).is_xml_template;
	(*dest).is_history = (*self).is_history;
	(*dest).is_sign = (*self).is_sign;
	(*dest).is_certificate_encrypt = (*self).is_certificate_encrypt;
	(*dest).is_sign_spare = (*self).is_sign_spare;
	(*dest).is_certificate_drm = (*self).is_certificate_drm;
	(*dest).is_ccl = (*self).is_ccl;
}


void ghwp_file_header_destroy (GHWPFileHeader* self) {
	_g_free0 ((*self).signature);
}


GHWPFileHeader* ghwp_file_header_dup (const GHWPFileHeader* self)
{
	GHWPFileHeader* dup = g_new0 (GHWPFileHeader, 1);
	ghwp_file_header_copy (self, dup);
	return dup;
}


void ghwp_file_header_free (GHWPFileHeader* self) {
	ghwp_file_header_destroy (self);
	g_free (self);
}


static void ghwp_file_class_init (GHWPFileClass * klass) {
	ghwp_file_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (GHWPFilePrivate));
	G_OBJECT_CLASS (klass)->finalize = ghwp_file_finalize;
}


static void ghwp_file_init (GHWPFile * self) {
	GHWPFileHeader _tmp0_ = {0};
	self->priv = GHWP_FILE_GET_PRIVATE (self);
	memset (&_tmp0_, 0, sizeof (GHWPFileHeader));
	self->header = _tmp0_;
}


static void ghwp_file_finalize (GObject* obj)
{
	GHWPFile *self = G_TYPE_CHECK_INSTANCE_CAST (obj, GHWP_TYPE_FILE, GHWPFile);
	_g_object_unref0 (self->priv->olefile);
	ghwp_file_header_destroy (&self->header);
	_g_object_unref0 (self->prv_text_stream);
	_g_object_unref0 (self->prv_image_stream);
	_g_object_unref0 (self->file_header_stream);
	_g_object_unref0 (self->doc_info_stream);
	_g_array_free0 (self->section_streams);
	_g_object_unref0 (self->priv->section_stream);
	_g_object_unref0 (self->summary_info_stream);
	G_OBJECT_CLASS (ghwp_file_parent_class)->finalize (obj);
}

static void ghwp_file_header_class_init (GHWPFileHeaderClass * klass)
{
}

static void ghwp_file_header_init (GHWPFileHeader * self)
{
}
