A virtual reality video player for Linux running X11, based on Valve's openvr `hellovr_opengl` sample code: https://github.com/ValveSoftware/openvr/tree/master/samples\
Still early in development, not very user friendly.\
Currently only works with stereo video.\
# How to use
Start a video in your video player of choice (tested with mpv) and then get the x11 window id (this can be done with xwininfo) and then fullscreen the video (for best quality).
Then launch `vr_video_player` with the x11 window id.
