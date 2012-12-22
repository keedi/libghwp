/*
 * ghwp-parse.c
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


#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "ghwp-parse.h"

G_DEFINE_TYPE (GHWPContext, ghwp_context, G_TYPE_OBJECT);

#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))

#define GHWP_CONTEXT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GHWP_TYPE_CONTEXT, GHWPContextPrivate))

gboolean ghwp_context_decode_header (GHWPContext* self, guchar* buf, int buf_length1);
static void ghwp_context_finalize (GObject* obj);

GHWPContext* ghwp_context_new (GInputStream* stream)
{
	g_return_val_if_fail (stream != NULL, NULL);
	GHWPContext *self = (GHWPContext*) g_object_new (GHWP_TYPE_CONTEXT, NULL);
	self->priv->stream = g_object_ref (stream);
	return self;
}


gboolean ghwp_context_decode_header (GHWPContext* self, guchar* buf, int buf_length1) {
	gboolean result = FALSE;
	guchar* _tmp0_;
	guchar _tmp1_;
	guchar* _tmp2_;
	guchar _tmp3_;
	guchar* _tmp4_;
	guchar _tmp5_;
	guchar* _tmp6_;
	guchar _tmp7_;
	guint32 _tmp8_;
	guint32 _tmp9_;
	guint32 _tmp10_;
	guint32 _tmp11_;
	GError * _inner_error_ = NULL;
	g_return_val_if_fail (self != NULL, FALSE);
	_tmp0_ = buf;
	_tmp1_ = _tmp0_[3];
	_tmp2_ = buf;
	_tmp3_ = _tmp2_[2];
	_tmp4_ = buf;
	_tmp5_ = _tmp4_[1];
	_tmp6_ = buf;
	_tmp7_ = _tmp6_[0];
	self->priv->header = (guint32) ((((_tmp1_ << 24) | (_tmp3_ << 16)) | (_tmp5_ << 8)) | _tmp7_);
	_tmp8_ = self->priv->header;
	self->tag_id = (guint16) (_tmp8_ & 0x3ff);
	_tmp9_ = self->priv->header;
	self->level = (guint16) ((_tmp9_ >> 10) & 0x3ff);
	_tmp10_ = self->priv->header;
	self->size = (guint32) ((_tmp10_ >> 20) & 0xfff);
	_tmp11_ = self->size;
	if (_tmp11_ == ((guint32) 0xfff)) {
		gsize _tmp16_;
		guchar* _tmp17_;
		guchar _tmp18_;
		guchar* _tmp19_;
		guchar _tmp20_;
		guchar* _tmp21_;
		guchar _tmp22_;
		guchar* _tmp23_;
		guchar _tmp24_;
		{
			GInputStream* _tmp12_;
			guchar* _tmp13_;
			gint _tmp13__length1;
			gsize _tmp14_ = 0UL;
			_tmp12_ = self->priv->stream;
			_tmp13_ = buf;
			_tmp13__length1 = buf_length1;
			g_input_stream_read_all (_tmp12_, (void*) _tmp13_, (gsize) _tmp13__length1, &_tmp14_, NULL, &_inner_error_);
			self->priv->bytes_read = _tmp14_;
			if (_inner_error_ != NULL) {
				goto __catch5_g_error;
			}
		}
		goto __finally5;
		__catch5_g_error:
		{
			GError* e = NULL;
			const gchar* _tmp15_;
			e = _inner_error_;
			_inner_error_ = NULL;
			_tmp15_ = e->message;
			g_error ("parse.vala:38: %s", _tmp15_);
			_g_error_free0 (e);
		}
		__finally5:
		if (_inner_error_ != NULL) {
			g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return FALSE;
		}
		_tmp16_ = self->priv->bytes_read;
		if (_tmp16_ <= ((gsize) 0)) {
			result = FALSE;
			return result;
		}
		_tmp17_ = buf;
		_tmp18_ = _tmp17_[3];
		_tmp19_ = buf;
		_tmp20_ = _tmp19_[2];
		_tmp21_ = buf;
		_tmp22_ = _tmp21_[1];
		_tmp23_ = buf;
		_tmp24_ = _tmp23_[0];
		self->size = (guint32) ((((_tmp18_ << 24) | (_tmp20_ << 16)) | (_tmp22_ << 8)) | _tmp24_);
	}
	result = TRUE;
	return result;
}

gboolean ghwp_context_pull (GHWPContext* self)
{
	gboolean result = FALSE;
	gsize _tmp4_;
	guchar* _tmp5_;
	gint _tmp5__length1;
	gboolean _tmp6_ = FALSE;
	gboolean _tmp7_;
	guint32 _tmp8_;
	guchar* _tmp9_ = NULL;
	guchar* tmp;
	gint tmp_length1;
	gsize _tmp14_;
	guchar* _tmp15_;
	gint _tmp15__length1;
	guchar* _tmp16_;
	gint _tmp16__length1;
	GError * _inner_error_ = NULL;
	g_return_val_if_fail (self != NULL, FALSE);
	{
		GInputStream* _tmp0_;
		guchar* _tmp1_;
		gint _tmp1__length1;
		gsize _tmp2_ = 0UL;
		_tmp0_ = self->priv->stream;
		_tmp1_ = self->priv->buf;
		_tmp1__length1 = self->priv->buf_length1;
		g_input_stream_read_all (_tmp0_, (void*) _tmp1_, (gsize) _tmp1__length1, &_tmp2_, NULL, &_inner_error_);
		self->priv->bytes_read = _tmp2_;
		if (_inner_error_ != NULL) {
			goto __catch6_g_error;
		}
	}
	goto __finally6;
	__catch6_g_error:
	{
		GError* e = NULL;
		const gchar* _tmp3_;
		e = _inner_error_;
		_inner_error_ = NULL;
		_tmp3_ = e->message;
		g_error ("parse.vala:54: %s", _tmp3_);
		_g_error_free0 (e);
	}
	__finally6:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return FALSE;
	}
	_tmp4_ = self->priv->bytes_read;
	if (_tmp4_ <= ((gsize) 0)) {
		result = FALSE;
		return result;
	}
	_tmp5_ = self->priv->buf;
	_tmp5__length1 = self->priv->buf_length1;
	_tmp6_ = ghwp_context_decode_header (self, _tmp5_, _tmp5__length1);
	self->priv->ret = _tmp6_;
	_tmp7_ = self->priv->ret;
	if (_tmp7_ == FALSE) {
		result = FALSE;
		return result;
	}
	_tmp8_ = self->size;
	_tmp9_ = g_new0 (guchar, _tmp8_);
	tmp = _tmp9_;
	tmp_length1 = _tmp8_;
	{
		GInputStream* _tmp10_;
		guchar* _tmp11_;
		gint _tmp11__length1;
		gsize _tmp12_ = 0UL;
		_tmp10_ = self->priv->stream;
		_tmp11_ = tmp;
		_tmp11__length1 = tmp_length1;
		g_input_stream_read_all (_tmp10_, (void*) _tmp11_, (gsize) _tmp11__length1, &_tmp12_, NULL, &_inner_error_);
		self->priv->bytes_read = _tmp12_;
		if (_inner_error_ != NULL) {
			goto __catch7_g_error;
		}
	}
	goto __finally7;
	__catch7_g_error:
	{
		GError* e = NULL;
		const gchar* _tmp13_;
		e = _inner_error_;
		_inner_error_ = NULL;
		_tmp13_ = e->message;
		g_error ("parse.vala:64: %s", _tmp13_);
		_g_error_free0 (e);
	}
	__finally7:
	if (_inner_error_ != NULL) {
		tmp = (g_free (tmp), NULL);
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return FALSE;
	}
	_tmp14_ = self->priv->bytes_read;
	if (_tmp14_ <= ((gsize) 0)) {
		result = FALSE;
		tmp = (g_free (tmp), NULL);
		return result;
	}
	_tmp15_ = tmp;
	_tmp15__length1 = tmp_length1;
	_tmp16_ = (_tmp15_ != NULL) ? g_memdup (_tmp15_, _tmp15__length1 * sizeof (guchar)) : ((gpointer) _tmp15_);
	_tmp16__length1 = _tmp15__length1;
	self->data = (g_free (self->data), NULL);
	self->data = _tmp16_;
	self->data_length1 = _tmp16__length1;
	result = TRUE;
	tmp = (g_free (tmp), NULL);
	return result;
}


static void ghwp_context_class_init (GHWPContextClass * klass) {
	ghwp_context_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (GHWPContextPrivate));
	G_OBJECT_CLASS (klass)->finalize = ghwp_context_finalize;
}


static void ghwp_context_init (GHWPContext * self)
{
	guchar* _tmp0_ = NULL;
	self->priv = GHWP_CONTEXT_GET_PRIVATE (self);
	_tmp0_ = g_new0 (guchar, 4);
	self->priv->buf = _tmp0_;
	self->priv->buf_length1 = 4;
	self->priv->_buf_size_ = self->priv->buf_length1;
}


static void ghwp_context_finalize (GObject* obj) {
	GHWPContext * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, GHWP_TYPE_CONTEXT, GHWPContext);
	_g_object_unref0 (self->priv->stream);
	self->priv->buf = (g_free (self->priv->buf), NULL);
	self->data = (g_free (self->data), NULL);
	G_OBJECT_CLASS (ghwp_context_parent_class)->finalize (obj);
}