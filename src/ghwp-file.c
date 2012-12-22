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
#include <gio/gio.h>
#include <gsf/gsf-input-impl.h>
#include <stdlib.h>
#include <string.h>
#include <gsf/gsf-infile-msole.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-infile-impl.h>
#include <stdio.h>


#define TYPE_GSF_INPUT_STREAM (gsf_input_stream_get_type ())
#define GSF_INPUT_STREAM(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GSF_INPUT_STREAM, GsfInputStream))
#define GSF_INPUT_STREAM_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GSF_INPUT_STREAM, GsfInputStreamClass))
#define IS_GSF_INPUT_STREAM(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_GSF_INPUT_STREAM))
#define IS_GSF_INPUT_STREAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GSF_INPUT_STREAM))
#define GSF_INPUT_STREAM_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GSF_INPUT_STREAM, GsfInputStreamClass))

typedef struct _GsfInputStream GsfInputStream;
typedef struct _GsfInputStreamClass GsfInputStreamClass;
typedef struct _GsfInputStreamPrivate GsfInputStreamPrivate;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))

#define GHWP_TYPE_GHWP_FILE (ghwp_file_get_type ())
#define GHWP_FILE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_GHWP_FILE, GHWPFile))
#define GHWP_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_GHWP_FILE, GHWPFileClass))
#define GHWP_IS_GHWP_FILE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_GHWP_FILE))
#define GHWP_IS_GHWP_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_GHWP_FILE))
#define GHWP_FILE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_GHWP_FILE, GHWPFileClass))

typedef struct _GHWPFile GHWPFile;
typedef struct _GHWPFileClass GHWPFileClass;
typedef struct _GHWPFilePrivate GHWPFilePrivate;

#define GHWP_FILE_TYPE_HEADER (ghwp_file_header_get_type ())
typedef struct _GHWPFileHeader GHWPFileHeader;
#define _g_array_free0(var) ((var == NULL) ? NULL : (var = (g_array_free (var, TRUE), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))

struct _GsfInputStream {
	GInputStream parent_instance;
	GsfInputStreamPrivate * priv;
};

struct _GsfInputStreamClass {
	GInputStreamClass parent_class;
};

struct _GsfInputStreamPrivate {
	GsfInput* input;
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

struct _GHWPFilePrivate {
	GsfInfileMSOle* olefile;
	GInputStream* section_stream;
};


static gpointer gsf_input_stream_parent_class = NULL;
static gpointer ghwp_file_parent_class = NULL;

GType gsf_input_stream_get_type (void) G_GNUC_CONST;
#define GSF_INPUT_STREAM_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSF_INPUT_STREAM, GsfInputStreamPrivate))
enum  {
	GSF_INPUT_STREAM_DUMMY_PROPERTY
};
GsfInputStream* gsf_input_stream_new (GsfInput* input);
GsfInputStream* gsf_input_stream_construct (GType object_type, GsfInput* input);
static gssize gsf_input_stream_real_read (GInputStream* base,
                                          void* buffer,
                                          gsize buffer_length1,
                                          GCancellable* cancellable,
                                          GError** error);
static gboolean gsf_input_stream_real_close (GInputStream* base, GCancellable* cancellable, GError** error);
gssize gsf_input_stream_size (GsfInputStream* self);
static void gsf_input_stream_finalize (GObject* obj);
GType ghwp_file_get_type (void) G_GNUC_CONST;
GType ghwp_file_header_get_type (void) G_GNUC_CONST;
GHWPFileHeader* ghwp_file_header_dup (const GHWPFileHeader* self);
void ghwp_file_header_free (GHWPFileHeader* self);
void ghwp_file_header_copy (const GHWPFileHeader* self, GHWPFileHeader* dest);
void ghwp_file_header_destroy (GHWPFileHeader* self);
#define GHWP_FILE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GHWP_TYPE_GHWP_FILE, GHWPFilePrivate))
enum  {
	GHWP_FILE_DUMMY_PROPERTY
};
GHWPFile* ghwp_file_new_from_uri (const gchar* uri, GError** error);
GHWPFile* ghwp_file_construct_from_uri (GType object_type, const gchar* uri, GError** error);
static void ghwp_file_init (GHWPFile* self);
GHWPFile* ghwp_file_new_from_filename (const gchar* filename, GError** error);
GHWPFile* ghwp_file_construct_from_filename (GType object_type, const gchar* filename, GError** error);
static void ghwp_file_decode_file_header (GHWPFile* self);
GHWPFile* ghwp_file_new (void);
GHWPFile* ghwp_file_construct (GType object_type);
static void ghwp_file_finalize (GObject* obj);


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


GsfInputStream* gsf_input_stream_construct (GType object_type, GsfInput* input) {
	GsfInputStream * self = NULL;
	GsfInput* _tmp0_;
	GsfInput* _tmp1_;
	g_return_val_if_fail (input != NULL, NULL);
	self = (GsfInputStream*) g_object_new (object_type, NULL);
	_tmp0_ = input;
	_tmp1_ = _g_object_ref0 (_tmp0_);
	_g_object_unref0 (self->priv->input);
	self->priv->input = _tmp1_;
	return self;
}


GsfInputStream* gsf_input_stream_new (GsfInput* input) {
	return gsf_input_stream_construct (TYPE_GSF_INPUT_STREAM, input);
}


static gssize gsf_input_stream_real_read (GInputStream* base,
                                          void* buffer,
                                          gsize buffer_length1,
                                          GCancellable* cancellable,
                                          GError** error)
{
	GsfInputStream * self = (GsfInputStream*) base;
	gint64 stamp = gsf_input_remaining (self->priv->input);

	if (gsf_input_remaining (self->priv->input) < ((gint64) buffer_length1)) {
		gsf_input_read (self->priv->input,
					    (gsize) gsf_input_remaining (self->priv->input),
					    buffer);
	} else {
		gsf_input_read (self->priv->input, (gsize) buffer_length1, buffer);
	}
	return (gssize) (stamp - gsf_input_remaining (self->priv->input));
}


static gboolean gsf_input_stream_real_close (GInputStream* base, GCancellable* cancellable, GError** error) {
	/* pseudo TRUE */
	return TRUE;
}


gssize gsf_input_stream_size (GsfInputStream* self) {
	g_return_val_if_fail (self != NULL, 0L);
	return (gssize) gsf_input_size (self->priv->input);
}


static void gsf_input_stream_class_init (GsfInputStreamClass * klass) {
	gsf_input_stream_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (GsfInputStreamPrivate));
	G_INPUT_STREAM_CLASS (klass)->read_fn = gsf_input_stream_real_read;
	G_INPUT_STREAM_CLASS (klass)->close_fn = gsf_input_stream_real_close;
	G_OBJECT_CLASS (klass)->finalize = gsf_input_stream_finalize;
}


static void gsf_input_stream_instance_init (GsfInputStream * self) {
	self->priv = GSF_INPUT_STREAM_GET_PRIVATE (self);
}


static void gsf_input_stream_finalize (GObject* obj) {
	GsfInputStream * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, TYPE_GSF_INPUT_STREAM, GsfInputStream);
	_g_object_unref0 (self->priv->input);
	G_OBJECT_CLASS (gsf_input_stream_parent_class)->finalize (obj);
}


GType gsf_input_stream_get_type (void) {
	static volatile gsize gsf_input_stream_type_id__volatile = 0;
	if (g_once_init_enter (&gsf_input_stream_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (GsfInputStreamClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsf_input_stream_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GsfInputStream), 0, (GInstanceInitFunc) gsf_input_stream_instance_init, NULL };
		GType gsf_input_stream_type_id;
		gsf_input_stream_type_id = g_type_register_static (G_TYPE_INPUT_STREAM, "GsfInputStream", &g_define_type_info, 0);
		g_once_init_leave (&gsf_input_stream_type_id__volatile, gsf_input_stream_type_id);
	}
	return gsf_input_stream_type_id__volatile;
}


GHWPFile* ghwp_file_construct_from_uri (GType object_type, const gchar* uri, GError** error) {
	GHWPFile * self = NULL;
	const gchar* _tmp0_;
	gchar* _tmp1_ = NULL;
	gchar* filename;
	GError * _inner_error_ = NULL;
	g_return_val_if_fail (uri != NULL, NULL);
	self = (GHWPFile*) g_object_new (object_type, NULL);
	_tmp0_ = uri;
	_tmp1_ = g_filename_from_uri (_tmp0_, NULL, &_inner_error_);
	filename = _tmp1_;
	if (_inner_error_ != NULL) {
		g_propagate_error (error, _inner_error_);
		_g_object_unref0 (self);
		return NULL;
	}
	{
		GsfInputStdio* _tmp2_;
		GsfInputStdio* input;
		GsfInputStdio* _tmp3_;
		GsfInfileMSOle* _tmp4_;
		GsfInfileMSOle* _tmp5_;
		_tmp2_ = (GsfInputStdio*) gsf_input_stdio_new (filename, &_inner_error_);
		input = _tmp2_;
		if (_inner_error_ != NULL) {
			goto __catch0_g_error;
		}
		_tmp3_ = input;
		_tmp4_ = (GsfInfileMSOle*) gsf_infile_msole_new ((GsfInput*) _tmp3_, &_inner_error_);
		_tmp5_ = _tmp4_;
		if (_inner_error_ != NULL) {
			_g_object_unref0 (input);
			goto __catch0_g_error;
		}
		_g_object_unref0 (self->priv->olefile);
		self->priv->olefile = _tmp5_;
		_g_object_unref0 (input);
	}
	goto __finally0;
	__catch0_g_error:
	{
		GError* e = NULL;
		const gchar* _tmp6_;
		e = _inner_error_;
		_inner_error_ = NULL;
		_tmp6_ = e->message;
		g_error ("ghwp-file.vala:98: %s", _tmp6_);
		_g_error_free0 (e);
	}
	__finally0:
	if (_inner_error_ != NULL) {
		g_propagate_error (error, _inner_error_);
		_g_free0 (filename);
		_g_object_unref0 (self);
		return NULL;
	}
	ghwp_file_init (self);
	_g_free0 (filename);
	return self;
}


GHWPFile* ghwp_file_new_from_uri (const gchar* uri, GError** error) {
	return ghwp_file_construct_from_uri (GHWP_TYPE_GHWP_FILE, uri, error);
}


GHWPFile* ghwp_file_construct_from_filename (GType object_type, const gchar* filename, GError** error) {
	GHWPFile * self = NULL;
	const gchar* _tmp0_;
	GFile* _tmp1_ = NULL;
	GFile* file;
	GError * _inner_error_ = NULL;
	g_return_val_if_fail (filename != NULL, NULL);
	self = (GHWPFile*) g_object_new (object_type, NULL);
	_tmp0_ = filename;
	_tmp1_ = g_file_new_for_path (_tmp0_);
	file = _tmp1_;
	{
		GFile* _tmp2_;
		gchar* _tmp3_ = NULL;
		gchar* _tmp4_;
		GsfInputStdio* _tmp5_;
		GsfInputStdio* _tmp6_;
		GsfInputStdio* input;
		GsfInputStdio* _tmp7_;
		GsfInfileMSOle* _tmp8_;
		GsfInfileMSOle* _tmp9_;
		_tmp2_ = file;
		_tmp3_ = g_file_get_path (_tmp2_);
		_tmp4_ = _tmp3_;
		_tmp5_ = (GsfInputStdio*) gsf_input_stdio_new (_tmp4_, &_inner_error_);
		_tmp6_ = _tmp5_;
		_g_free0 (_tmp4_);
		input = _tmp6_;
		if (_inner_error_ != NULL) {
			goto __catch1_g_error;
		}
		_tmp7_ = input;
		_tmp8_ = (GsfInfileMSOle*) gsf_infile_msole_new ((GsfInput*) _tmp7_, &_inner_error_);
		_tmp9_ = _tmp8_;
		if (_inner_error_ != NULL) {
			_g_object_unref0 (input);
			goto __catch1_g_error;
		}
		_g_object_unref0 (self->priv->olefile);
		self->priv->olefile = _tmp9_;
		_g_object_unref0 (input);
	}
	goto __finally1;
	__catch1_g_error:
	{
		GError* e = NULL;
		const gchar* _tmp10_;
		e = _inner_error_;
		_inner_error_ = NULL;
		_tmp10_ = e->message;
		g_error ("ghwp-file.vala:112: %s", _tmp10_);
		_g_error_free0 (e);
	}
	__finally1:
	if (_inner_error_ != NULL) {
		g_propagate_error (error, _inner_error_);
		_g_object_unref0 (file);
		_g_object_unref0 (self);
		return NULL;
	}
	ghwp_file_init (self);
	_g_object_unref0 (file);
	return self;
}


GHWPFile* ghwp_file_new_from_filename (const gchar* filename, GError** error) {
	return ghwp_file_construct_from_filename (GHWP_TYPE_GHWP_FILE, filename, error);
}


static void ghwp_file_init (GHWPFile* self) {
	GsfInfileMSOle* _tmp0_;
	gint _tmp1_ = 0;
	gint n_children;
	gint _tmp2_;
	g_return_if_fail (self != NULL);
	_tmp0_ = self->priv->olefile;
	_tmp1_ = gsf_infile_num_children ((GsfInfile*) _tmp0_);
	n_children = _tmp1_;
	_tmp2_ = n_children;
	if (_tmp2_ < 1) {
		FILE* _tmp3_;
		_tmp3_ = stderr;
		fprintf (_tmp3_, "invalid hwp file\n");
		return;
	}
	{
		gint i;
		i = 0;
		{
			gboolean _tmp4_;
			_tmp4_ = TRUE;
			while (TRUE) {
				gboolean _tmp5_;
				gint _tmp7_;
				gint _tmp8_;
				GsfInfileMSOle* _tmp9_;
				gint _tmp10_;
				const gchar* _tmp11_ = NULL;
				gchar* _tmp12_;
				gchar* name;
				const gchar* _tmp13_;
				const gchar* _tmp14_;
				GQuark _tmp16_ = 0U;
				static GQuark _tmp15_label0 = 0;
				static GQuark _tmp15_label1 = 0;
				static GQuark _tmp15_label2 = 0;
				static GQuark _tmp15_label3 = 0;
				static GQuark _tmp15_label4 = 0;
				static GQuark _tmp15_label5 = 0;
				static GQuark _tmp15_label6 = 0;
				_tmp5_ = _tmp4_;
				if (!_tmp5_) {
					gint _tmp6_;
					_tmp6_ = i;
					i = _tmp6_ + 1;
				}
				_tmp4_ = FALSE;
				_tmp7_ = i;
				_tmp8_ = n_children;
				if (!(_tmp7_ < _tmp8_)) {
					break;
				}
				_tmp9_ = self->priv->olefile;
				_tmp10_ = i;
				_tmp11_ = gsf_infile_name_by_index ((GsfInfile*) _tmp9_, _tmp10_);
				_tmp12_ = g_strdup (_tmp11_);
				name = _tmp12_;
				_tmp13_ = name;
				_tmp14_ = _tmp13_;
				_tmp16_ = (NULL == _tmp14_) ? 0 : g_quark_from_string (_tmp14_);
				if (_tmp16_ == ((0 != _tmp15_label0) ? _tmp15_label0 : (_tmp15_label0 = g_quark_from_static_string ("PrvText")))) {
					switch (0) {
						default:
						{
							GsfInfileMSOle* _tmp17_;
							GsfInput* _tmp18_ = NULL;
							GsfInput* _tmp19_;
							GsfInput* input;
							GsfInput* _tmp20_;
							gint _tmp21_ = 0;
							GsfInput* _tmp23_;
							GsfInputStream* _tmp24_;
							_tmp17_ = self->priv->olefile;
							_tmp18_ = gsf_infile_child_by_name ((GsfInfile*) _tmp17_, "PrvText");
							_tmp19_ = _g_object_ref0 (_tmp18_);
							input = _tmp19_;
							_tmp20_ = input;
							_tmp21_ = gsf_infile_num_children (G_TYPE_CHECK_INSTANCE_CAST (_tmp20_, GSF_INFILE_TYPE, GsfInfile));
							if (_tmp21_ > 0) {
								FILE* _tmp22_;
								_tmp22_ = stderr;
								fprintf (_tmp22_, "invalid\n");
							}
							_tmp23_ = input;
							_tmp24_ = gsf_input_stream_new (_tmp23_);
							_g_object_unref0 (self->prv_text_stream);
							self->prv_text_stream = (GInputStream*) _tmp24_;
							_g_object_unref0 (input);
							break;
						}
					}
				} else if (_tmp16_ == ((0 != _tmp15_label1) ? _tmp15_label1 : (_tmp15_label1 = g_quark_from_static_string ("PrvImage")))) {
					switch (0) {
						default:
						{
							GsfInfileMSOle* _tmp25_;
							GsfInput* _tmp26_ = NULL;
							GsfInput* _tmp27_;
							GsfInput* input;
							GsfInput* _tmp28_;
							gint _tmp29_ = 0;
							GsfInput* _tmp31_;
							GsfInputStream* _tmp32_;
							_tmp25_ = self->priv->olefile;
							_tmp26_ = gsf_infile_child_by_name ((GsfInfile*) _tmp25_, "PrvImage");
							_tmp27_ = _g_object_ref0 (_tmp26_);
							input = _tmp27_;
							_tmp28_ = input;
							_tmp29_ = gsf_infile_num_children (G_TYPE_CHECK_INSTANCE_CAST (_tmp28_, GSF_INFILE_TYPE, GsfInfile));
							if (_tmp29_ > 0) {
								FILE* _tmp30_;
								_tmp30_ = stderr;
								fprintf (_tmp30_, "invalid\n");
							}
							_tmp31_ = input;
							_tmp32_ = gsf_input_stream_new (_tmp31_);
							_g_object_unref0 (self->prv_image_stream);
							self->prv_image_stream = (GInputStream*) _tmp32_;
							_g_object_unref0 (input);
							break;
						}
					}
				} else if (_tmp16_ == ((0 != _tmp15_label2) ? _tmp15_label2 : (_tmp15_label2 = g_quark_from_static_string ("FileHeader")))) {
					switch (0) {
						default:
						{
							GsfInfileMSOle* _tmp33_;
							GsfInput* _tmp34_ = NULL;
							GsfInput* _tmp35_;
							GsfInput* input;
							GsfInput* _tmp36_;
							gint _tmp37_ = 0;
							GsfInput* _tmp39_;
							GsfInputStream* _tmp40_;
							_tmp33_ = self->priv->olefile;
							_tmp34_ = gsf_infile_child_by_name ((GsfInfile*) _tmp33_, "FileHeader");
							_tmp35_ = _g_object_ref0 (_tmp34_);
							input = _tmp35_;
							_tmp36_ = input;
							_tmp37_ = gsf_infile_num_children (G_TYPE_CHECK_INSTANCE_CAST (_tmp36_, GSF_INFILE_TYPE, GsfInfile));
							if (_tmp37_ > 0) {
								FILE* _tmp38_;
								_tmp38_ = stderr;
								fprintf (_tmp38_, "invalid\n");
							}
							_tmp39_ = input;
							_tmp40_ = gsf_input_stream_new (_tmp39_);
							_g_object_unref0 (self->file_header_stream);
							self->file_header_stream = (GInputStream*) _tmp40_;
							ghwp_file_decode_file_header (self);
							_g_object_unref0 (input);
							break;
						}
					}
				} else if (_tmp16_ == ((0 != _tmp15_label3) ? _tmp15_label3 : (_tmp15_label3 = g_quark_from_static_string ("DocInfo")))) {
					switch (0) {
						default:
						{
							GsfInfileMSOle* _tmp41_;
							GsfInput* _tmp42_ = NULL;
							GsfInput* _tmp43_;
							GsfInput* input;
							GsfInput* _tmp44_;
							gint _tmp45_ = 0;
							GHWPFileHeader _tmp47_;
							gboolean _tmp48_;
							_tmp41_ = self->priv->olefile;
							_tmp42_ = gsf_infile_child_by_name ((GsfInfile*) _tmp41_, "DocInfo");
							_tmp43_ = _g_object_ref0 (_tmp42_);
							input = _tmp43_;
							_tmp44_ = input;
							_tmp45_ = gsf_infile_num_children (G_TYPE_CHECK_INSTANCE_CAST (_tmp44_, GSF_INFILE_TYPE, GsfInfile));
							if (_tmp45_ > 0) {
								FILE* _tmp46_;
								_tmp46_ = stderr;
								fprintf (_tmp46_, "invalid\n");
							}
							_tmp47_ = self->header;
							_tmp48_ = _tmp47_.is_compress;
							if (_tmp48_) {
								GsfInput* _tmp49_;
								GsfInputStream* _tmp50_;
								GsfInputStream* gis;
								GZlibDecompressor* _tmp51_;
								GZlibDecompressor* zd;
								GsfInputStream* _tmp52_;
								GZlibDecompressor* _tmp53_;
								GConverterInputStream* _tmp54_;
								_tmp49_ = input;
								_tmp50_ = gsf_input_stream_new (_tmp49_);
								gis = _tmp50_;
								_tmp51_ = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
								zd = _tmp51_;
								_tmp52_ = gis;
								_tmp53_ = zd;
								_tmp54_ = (GConverterInputStream*) g_converter_input_stream_new ((GInputStream*) _tmp52_, (GConverter*) _tmp53_);
								_g_object_unref0 (self->doc_info_stream);
								self->doc_info_stream = (GInputStream*) _tmp54_;
								_g_object_unref0 (zd);
								_g_object_unref0 (gis);
							} else {
								GsfInput* _tmp55_;
								GsfInputStream* _tmp56_;
								_tmp55_ = input;
								_tmp56_ = gsf_input_stream_new (_tmp55_);
								_g_object_unref0 (self->doc_info_stream);
								self->doc_info_stream = (GInputStream*) _tmp56_;
							}
							_g_object_unref0 (input);
							break;
						}
					}
				} else if ((_tmp16_ == ((0 != _tmp15_label4) ? _tmp15_label4 : (_tmp15_label4 = g_quark_from_static_string ("BodyText")))) || (_tmp16_ == ((0 != _tmp15_label5) ? _tmp15_label5 : (_tmp15_label5 = g_quark_from_static_string ("VeiwText"))))) {
					switch (0) {
						default:
						{
							GArray* _tmp57_;
							GsfInfileMSOle* _tmp58_;
							gint _tmp59_;
							GsfInput* _tmp60_ = NULL;
							GsfInfile* _tmp61_;
							GsfInfile* infile;
							GsfInfile* _tmp62_;
							gint _tmp63_ = 0;
							_tmp57_ = g_array_new (TRUE, TRUE, sizeof (GInputStream*));
							_g_array_free0 (self->section_streams);
							self->section_streams = _tmp57_;
							_tmp58_ = self->priv->olefile;
							_tmp59_ = i;
							_tmp60_ = gsf_infile_child_by_index ((GsfInfile*) _tmp58_, _tmp59_);
							_tmp61_ = _g_object_ref0 (G_TYPE_CHECK_INSTANCE_CAST (_tmp60_, GSF_INFILE_TYPE, GsfInfile));
							infile = _tmp61_;
							_tmp62_ = infile;
							_tmp63_ = gsf_infile_num_children (_tmp62_);
							if (_tmp63_ == 0) {
								FILE* _tmp64_;
								_tmp64_ = stderr;
								fprintf (_tmp64_, "nothing in BodyText\n");
							}
							{
								gint j;
								j = 0;
								{
									gboolean _tmp65_;
									_tmp65_ = TRUE;
									while (TRUE) {
										gboolean _tmp66_;
										gint _tmp68_;
										GsfInfile* _tmp69_;
										gint _tmp70_ = 0;
										GsfInfile* _tmp71_;
										gint _tmp72_;
										GsfInput* _tmp73_ = NULL;
										GsfInfile* _tmp74_;
										GsfInfile* section;
										GsfInfile* _tmp75_;
										gint _tmp76_ = 0;
										GHWPFileHeader _tmp78_;
										gboolean _tmp79_;
										GArray* _tmp88_;
										GInputStream* _tmp89_;
										GInputStream* _tmp90_;
										_tmp66_ = _tmp65_;
										if (!_tmp66_) {
											gint _tmp67_;
											_tmp67_ = j;
											j = _tmp67_ + 1;
										}
										_tmp65_ = FALSE;
										_tmp68_ = j;
										_tmp69_ = infile;
										_tmp70_ = gsf_infile_num_children (_tmp69_);
										if (!(_tmp68_ < _tmp70_)) {
											break;
										}
										_tmp71_ = infile;
										_tmp72_ = j;
										_tmp73_ = gsf_infile_child_by_index (_tmp71_, _tmp72_);
										_tmp74_ = _g_object_ref0 (G_TYPE_CHECK_INSTANCE_CAST (_tmp73_, GSF_INFILE_TYPE, GsfInfile));
										section = _tmp74_;
										_tmp75_ = section;
										_tmp76_ = gsf_infile_num_children (_tmp75_);
										if (_tmp76_ > 0) {
											FILE* _tmp77_;
											_tmp77_ = stderr;
											fprintf (_tmp77_, "invalid section\n");
										}
										_tmp78_ = self->header;
										_tmp79_ = _tmp78_.is_compress;
										if (_tmp79_) {
											GsfInfile* _tmp80_;
											GsfInputStream* _tmp81_;
											GsfInputStream* gis;
											GZlibDecompressor* _tmp82_;
											GZlibDecompressor* zd;
											GsfInputStream* _tmp83_;
											GZlibDecompressor* _tmp84_;
											GConverterInputStream* _tmp85_;
											_tmp80_ = section;
											_tmp81_ = gsf_input_stream_new ((GsfInput*) _tmp80_);
											gis = _tmp81_;
											_tmp82_ = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
											zd = _tmp82_;
											_tmp83_ = gis;
											_tmp84_ = zd;
											_tmp85_ = (GConverterInputStream*) g_converter_input_stream_new ((GInputStream*) _tmp83_, (GConverter*) _tmp84_);
											_g_object_unref0 (self->priv->section_stream);
											self->priv->section_stream = (GInputStream*) _tmp85_;
											_g_object_unref0 (zd);
											_g_object_unref0 (gis);
										} else {
											GsfInfile* _tmp86_;
											GsfInputStream* _tmp87_;
											_tmp86_ = section;
											_tmp87_ = gsf_input_stream_new ((GsfInput*) _tmp86_);
											_g_object_unref0 (self->priv->section_stream);
											self->priv->section_stream = (GInputStream*) _tmp87_;
										}
										_tmp88_ = self->section_streams;
										_tmp89_ = self->priv->section_stream;
										_tmp90_ = _g_object_ref0 (_tmp89_);
										g_array_append_val (_tmp88_, _tmp90_);
										_g_object_unref0 (section);
									}
								}
							}
							_g_object_unref0 (infile);
							break;
						}
					}
				} else if (_tmp16_ == ((0 != _tmp15_label6) ? _tmp15_label6 : (_tmp15_label6 = g_quark_from_static_string ("\005HwpSummaryInformation")))) {
					switch (0) {
						default:
						{
							GsfInfileMSOle* _tmp91_;
							GsfInput* _tmp92_ = NULL;
							GsfInput* _tmp93_;
							GsfInput* input;
							GsfInput* _tmp94_;
							gint _tmp95_ = 0;
							GsfInput* _tmp97_;
							GsfInputStream* _tmp98_;
							_tmp91_ = self->priv->olefile;
							_tmp92_ = gsf_infile_child_by_name ((GsfInfile*) _tmp91_, "\005HwpSummaryInformation");
							_tmp93_ = _g_object_ref0 (_tmp92_);
							input = _tmp93_;
							_tmp94_ = input;
							_tmp95_ = gsf_infile_num_children (G_TYPE_CHECK_INSTANCE_CAST (_tmp94_, GSF_INFILE_TYPE, GsfInfile));
							if (_tmp95_ > 0) {
								FILE* _tmp96_;
								_tmp96_ = stderr;
								fprintf (_tmp96_, "invalid\n");
							}
							_tmp97_ = input;
							_tmp98_ = gsf_input_stream_new (_tmp97_);
							_g_object_unref0 (self->summary_info_stream);
							self->summary_info_stream = (GInputStream*) _tmp98_;
							_g_object_unref0 (input);
							break;
						}
					}
				} else {
					switch (0) {
						default:
						{
							FILE* _tmp99_;
							const gchar* _tmp100_;
							_tmp99_ = stderr;
							_tmp100_ = name;
							fprintf (_tmp99_, "not implemented error: %s\n", _tmp100_);
							break;
						}
					}
				}
				_g_free0 (name);
			}
		}
	}
}


static void ghwp_file_decode_file_header (GHWPFile* self) {
	GInputStream* _tmp0_;
	GsfInputStream* _tmp1_;
	GsfInputStream* gis;
	gssize _tmp2_ = 0L;
	gssize size;
	guchar* _tmp3_ = NULL;
	guchar* buf;
	gint buf_length1;
	gchar* _tmp5_;
	guchar _tmp6_;
	guchar _tmp7_;
	guchar _tmp8_;
	guchar _tmp9_;
	guchar _tmp10_;
	guchar _tmp11_;
	guchar _tmp12_;
	guchar _tmp13_;
	guint32 prop;
	guint32 _tmp14_;
	guint32 _tmp15_;
	guint32 _tmp16_;
	guint32 _tmp17_;
	guint32 _tmp18_;
	guint32 _tmp19_;
	guint32 _tmp20_;
	guint32 _tmp21_;
	guint32 _tmp22_;
	guint32 _tmp23_;
	guint32 _tmp24_;
	guint32 _tmp25_;
	GError * _inner_error_ = NULL;
	g_return_if_fail (self != NULL);
	_tmp0_ = self->file_header_stream;
	_tmp1_ = _g_object_ref0 (G_TYPE_CHECK_INSTANCE_CAST (_tmp0_, TYPE_GSF_INPUT_STREAM, GsfInputStream));
	gis = _tmp1_;
	_tmp2_ = gsf_input_stream_size (gis);
	size = _tmp2_;
	_tmp3_ = g_new0 (guchar, size);
	buf = _tmp3_;
	buf_length1 = size;
	{
		g_input_stream_read ((GInputStream*) gis, buf, (gsize) buf_length1, NULL, &_inner_error_);
		if (_inner_error_ != NULL) {
			goto __catch2_g_error;
		}
	}
	goto __finally2;
	__catch2_g_error:
	{
		GError* e = NULL;
		const gchar* _tmp4_;
		e = _inner_error_;
		_inner_error_ = NULL;
		_tmp4_ = e->message;
		g_error ("ghwp-file.vala:210: %s", _tmp4_);
		_g_error_free0 (e);
	}
	__finally2:
	if (_inner_error_ != NULL) {
		buf = (g_free (buf), NULL);
		_g_object_unref0 (gis);
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return;
	}
	_tmp5_ = g_strdup ((const gchar*) (buf + 0));
	_g_free0 (self->header.signature);
	self->header.signature = _tmp5_;
	_tmp6_ = buf[35];
	_tmp7_ = buf[34];
	_tmp8_ = buf[33];
	_tmp9_ = buf[32];
	self->header.version = (guint32) ((((_tmp6_ << 24) | (_tmp7_ << 16)) | (_tmp8_ << 8)) | _tmp9_);
	_tmp10_ = buf[39];
	_tmp11_ = buf[38];
	_tmp12_ = buf[37];
	_tmp13_ = buf[36];
	prop = (guint32) ((((_tmp10_ << 24) | (_tmp11_ << 16)) | (_tmp12_ << 8)) | _tmp13_);
	_tmp14_ = prop;
	if ((_tmp14_ & (1 << 0)) == ((guint32) 1)) {
		self->header.is_compress = TRUE;
	}
	_tmp15_ = prop;
	if ((_tmp15_ & (1 << 1)) == ((guint32) 1)) {
		self->header.is_encrypt = TRUE;
	}
	_tmp16_ = prop;
	if ((_tmp16_ & (1 << 2)) == ((guint32) 1)) {
		self->header.is_distribute = TRUE;
	}
	_tmp17_ = prop;
	if ((_tmp17_ & (1 << 3)) == ((guint32) 1)) {
		self->header.is_script = TRUE;
	}
	_tmp18_ = prop;
	if ((_tmp18_ & (1 << 4)) == ((guint32) 1)) {
		self->header.is_drm = TRUE;
	}
	_tmp19_ = prop;
	if ((_tmp19_ & (1 << 5)) == ((guint32) 1)) {
		self->header.is_xml_template = TRUE;
	}
	_tmp20_ = prop;
	if ((_tmp20_ & (1 << 6)) == ((guint32) 1)) {
		self->header.is_history = TRUE;
	}
	_tmp21_ = prop;
	if ((_tmp21_ & (1 << 7)) == ((guint32) 1)) {
		self->header.is_sign = TRUE;
	}
	_tmp22_ = prop;
	if ((_tmp22_ & (1 << 8)) == ((guint32) 1)) {
		self->header.is_certificate_encrypt = TRUE;
	}
	_tmp23_ = prop;
	if ((_tmp23_ & (1 << 9)) == ((guint32) 1)) {
		self->header.is_sign_spare = TRUE;
	}
	_tmp24_ = prop;
	if ((_tmp24_ & (1 << 10)) == ((guint32) 1)) {
		self->header.is_certificate_drm = TRUE;
	}
	_tmp25_ = prop;
	if ((_tmp25_ & (1 << 11)) == ((guint32) 1)) {
		self->header.is_ccl = TRUE;
	}
	buf = (g_free (buf), NULL);
	_g_object_unref0 (gis);
}


GHWPFile* ghwp_file_construct (GType object_type) {
	GHWPFile * self = NULL;
	self = (GHWPFile*) g_object_new (object_type, NULL);
	return self;
}


GHWPFile* ghwp_file_new (void) {
	return ghwp_file_construct (GHWP_TYPE_GHWP_FILE);
}


void ghwp_file_header_copy (const GHWPFileHeader* self, GHWPFileHeader* dest) {
	const gchar* _tmp0_;
	gchar* _tmp1_;
	guint32 _tmp2_;
	gboolean _tmp3_;
	gboolean _tmp4_;
	gboolean _tmp5_;
	gboolean _tmp6_;
	gboolean _tmp7_;
	gboolean _tmp8_;
	gboolean _tmp9_;
	gboolean _tmp10_;
	gboolean _tmp11_;
	gboolean _tmp12_;
	gboolean _tmp13_;
	gboolean _tmp14_;
	_tmp0_ = (*self).signature;
	_tmp1_ = g_strdup (_tmp0_);
	_g_free0 ((*dest).signature);
	(*dest).signature = _tmp1_;
	_tmp2_ = (*self).version;
	(*dest).version = _tmp2_;
	_tmp3_ = (*self).is_compress;
	(*dest).is_compress = _tmp3_;
	_tmp4_ = (*self).is_encrypt;
	(*dest).is_encrypt = _tmp4_;
	_tmp5_ = (*self).is_distribute;
	(*dest).is_distribute = _tmp5_;
	_tmp6_ = (*self).is_script;
	(*dest).is_script = _tmp6_;
	_tmp7_ = (*self).is_drm;
	(*dest).is_drm = _tmp7_;
	_tmp8_ = (*self).is_xml_template;
	(*dest).is_xml_template = _tmp8_;
	_tmp9_ = (*self).is_history;
	(*dest).is_history = _tmp9_;
	_tmp10_ = (*self).is_sign;
	(*dest).is_sign = _tmp10_;
	_tmp11_ = (*self).is_certificate_encrypt;
	(*dest).is_certificate_encrypt = _tmp11_;
	_tmp12_ = (*self).is_sign_spare;
	(*dest).is_sign_spare = _tmp12_;
	_tmp13_ = (*self).is_certificate_drm;
	(*dest).is_certificate_drm = _tmp13_;
	_tmp14_ = (*self).is_ccl;
	(*dest).is_ccl = _tmp14_;
}


void ghwp_file_header_destroy (GHWPFileHeader* self) {
	_g_free0 ((*self).signature);
}


GHWPFileHeader* ghwp_file_header_dup (const GHWPFileHeader* self) {
	GHWPFileHeader* dup;
	dup = g_new0 (GHWPFileHeader, 1);
	ghwp_file_header_copy (self, dup);
	return dup;
}


void ghwp_file_header_free (GHWPFileHeader* self) {
	ghwp_file_header_destroy (self);
	g_free (self);
}


GType ghwp_file_header_get_type (void) {
	static volatile gsize ghwp_file_header_type_id__volatile = 0;
	if (g_once_init_enter (&ghwp_file_header_type_id__volatile)) {
		GType ghwp_file_header_type_id;
		ghwp_file_header_type_id = g_boxed_type_register_static ("GHWPFileHeader", (GBoxedCopyFunc) ghwp_file_header_dup, (GBoxedFreeFunc) ghwp_file_header_free);
		g_once_init_leave (&ghwp_file_header_type_id__volatile, ghwp_file_header_type_id);
	}
	return ghwp_file_header_type_id__volatile;
}


static void ghwp_file_class_init (GHWPFileClass * klass) {
	ghwp_file_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (GHWPFilePrivate));
	G_OBJECT_CLASS (klass)->finalize = ghwp_file_finalize;
}


static void ghwp_file_instance_init (GHWPFile * self) {
	GHWPFileHeader _tmp0_ = {0};
	self->priv = GHWP_FILE_GET_PRIVATE (self);
	memset (&_tmp0_, 0, sizeof (GHWPFileHeader));
	self->header = _tmp0_;
}


static void ghwp_file_finalize (GObject* obj) {
	GHWPFile * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, GHWP_TYPE_GHWP_FILE, GHWPFile);
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


GType ghwp_file_get_type (void) {
	static volatile gsize ghwp_file_type_id__volatile = 0;
	if (g_once_init_enter (&ghwp_file_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (GHWPFileClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) ghwp_file_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GHWPFile), 0, (GInstanceInitFunc) ghwp_file_instance_init, NULL };
		GType ghwp_file_type_id;
		ghwp_file_type_id = g_type_register_static (G_TYPE_OBJECT, "GHWPFile", &g_define_type_info, 0);
		g_once_init_leave (&ghwp_file_type_id__volatile, ghwp_file_type_id);
	}
	return ghwp_file_type_id__volatile;
}



