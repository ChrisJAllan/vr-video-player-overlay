A virtual reality video player for Linux running X11, based on Valve's openvr `hellovr_opengl` sample code: https://github.com/ValveSoftware/openvr/tree/master/samples
Still early in development, not very user friendly.

Currently only works with stereo video.

# Installation
vr video player can be built using [sibs](https://github.com/DEC05EBA/sibs) or if you are running Arch Linux, then you can find it on aur under the name vr-video-player-git (`yay -S vr-video-player-git`).

# How to use
Start a video in your video player of choice (tested with mpv) and then get the x11 window id (this can be done with xwininfo) and then fullscreen the video (for best quality).
Then launch `vr_video_player` with the x11 window id.

If the video is not meant to be viewed as a sphere but as a rectangle, then pass the `--flat` option when running vr video player.\
If the video is flipped where the right eye is on the left side, then pass the `--right-left` option when running vr video player.\
If the video is stretched, then pass the `--no-stretch`option when running vr video player. Note: This option only works when also using the `--flat` option.

To rotate the video to be in front of you, pull the trigger on the vr controller or press `w` on your keyboard while the vr video player is focused.

# Games
This vr video player can also be used to play games in VR to to get a 3D effect, and even for games that don't support VR.\
For games such as Trine that have built-in side-by-side view, you can launch it with proton and there is a launch option for side-by-side view. Select this and when the game launches, get the X11 window id of the game
and launch vr video player with the `--flat` option.\
For games that do not have built-in side-by-side view, you can use [ReShade](https://reshade.me/) and [SuperDepth3D_VR.fx](https://github.com/BlueSkyDefender/Depth3D) effect with proton. This will make the game render with side-by-side view and you can then get the X11 window id of the game and launch vr video player with the `--flat` option. The game you are playing might require settings to be changed manually in ReShade for SuperDepth3D_VR to make it look better.
