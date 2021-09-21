#ifndef WINDOW_TEXTURE_H
#define WINDOW_TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xlib.h>

typedef struct {
    Display *display;
    Window window;
    Pixmap pixmap;
    GLXPixmap glx_pixmap;
    GLuint texture_id;
    int redirected;
} WindowTexture;

/* Returns 0 on success */
int window_texture_init(WindowTexture *window_texture, Display *display, Window window);
void window_texture_deinit(WindowTexture *self);

/*
    This should ONLY be called when the target window is resized.
    Returns 0 on success.
*/
int window_texture_on_resize(WindowTexture *self);

GLuint window_texture_get_opengl_texture_id(WindowTexture *self);

#ifdef __cplusplus
}
#endif

#endif /* WINDOW_TEXTURE_H */
