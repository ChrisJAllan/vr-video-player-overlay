A virtual reality video player for Linux running X11, based on Valve's openvr `hellovr_opengl` sample code: https://github.com/ValveSoftware/openvr/tree/master/samples
Still early in development, not very user friendly.

Currently only works with stereo video.

# Installation
vr video player can be built using [sibs](https://github.com/DEC05EBA/sibs) or if you are running Arch Linux, then you can find it on aur under the name vr-video-player-git (`yay -S vr-video-player-git`).

# How to use
Start a video in your video player of choice (tested with mpv) and then get the x11 window id (this can be done with xwininfo) and then fullscreen the video (for best quality).
Then launch `vr_video_player` with the x11 window id.
