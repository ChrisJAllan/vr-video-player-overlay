#include "../include/window_texture.h"
#include <X11/extensions/Xcomposite.h>

static int x11_supports_composite_named_window_pixmap(Display *display) {
    int extension_major;
    int extension_minor;
    if(!XCompositeQueryExtension(display, &extension_major, &extension_minor))
        return 0;

    int major_version;
    int minor_version;
    return XCompositeQueryVersion(display, &major_version, &minor_version) && (major_version > 0 || minor_version >= 2);
}

int window_texture_init(WindowTexture *window_texture, Display *display, Window window) {
    if(!x11_supports_composite_named_window_pixmap(display))
        return 1;

    window_texture->display = display;
    window_texture->window = window;
    window_texture->pixmap = None;
    window_texture->glx_pixmap = None;
    window_texture->texture_id = 0;

    XCompositeRedirectWindow(display, window, CompositeRedirectAutomatic);
    return window_texture_on_resize(window_texture);
}

static void window_texture_cleanup(WindowTexture *self) {
    if(self->texture_id) {
        glDeleteTextures(1, &self->texture_id);
        self->texture_id = 0;
    }

    if(self->glx_pixmap) {
        glXDestroyPixmap(self->display, self->glx_pixmap);
        glXReleaseTexImageEXT(self->display, self->glx_pixmap, GLX_FRONT_EXT);
        self->glx_pixmap = None;
    }

    if(self->pixmap) {
        XFreePixmap(self->display, self->pixmap);
        self->pixmap = None;
    }
}

void window_texture_deinit(WindowTexture *self) {
    window_texture_cleanup(self);
}

int window_texture_on_resize(WindowTexture *self) {
    window_texture_cleanup(self);

    int result = 0;

    const int pixmap_config[] = {
        GLX_BIND_TO_TEXTURE_RGBA_EXT, True,
        GLX_DRAWABLE_TYPE, GLX_PIXMAP_BIT,
        GLX_BIND_TO_TEXTURE_TARGETS_EXT, GLX_TEXTURE_2D_BIT_EXT,
        /*GLX_BIND_TO_MIPMAP_TEXTURE_EXT, True,*/
        GLX_DOUBLEBUFFER, False,
        GLX_BUFFER_SIZE, 32,
        GLX_ALPHA_SIZE, 8,
        None
    };

    const int pixmap_attribs[] = {
        GLX_TEXTURE_TARGET_EXT, GLX_TEXTURE_2D_EXT,
        GLX_TEXTURE_FORMAT_EXT, GLX_TEXTURE_FORMAT_RGB_EXT,
        /*GLX_MIPMAP_TEXTURE_EXT, True,*/
        None
    };

    int c;
    GLXFBConfig *configs = glXChooseFBConfig(self->display, 0, pixmap_config, &c);
    if(!configs)
        return 1;

    self->pixmap = XCompositeNameWindowPixmap(self->display, self->window);
    if(!self->pixmap) {
        result = 2;
        goto cleanup;
    }

    self->glx_pixmap = glXCreatePixmap(self->display, configs[0], self->pixmap, pixmap_attribs);
    if(!self->glx_pixmap) {
        result = 3;
        goto cleanup;
    }

    glGenTextures(1, &self->texture_id);
    glBindTexture(GL_TEXTURE_2D, self->texture_id);
    if(self->texture_id == 0) {
        result = 4;
        goto cleanup;
    }

    glXBindTexImageEXT(self->display, self->glx_pixmap, GLX_FRONT_EXT, NULL);
    /*glGenerateMipmap(GL_TEXTURE_2D)*/

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR_MIPMAP_LINEAR );
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
 
    float fLargest = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

    glBindTexture(GL_TEXTURE_2D, 0);

    cleanup:
    XFree(configs);
    return result;
}

GLuint window_texture_get_opengl_texture_id(WindowTexture *self) {
    return self->texture_id;
}