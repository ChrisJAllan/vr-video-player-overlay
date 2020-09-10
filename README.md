A virtual reality video player for Linux running X11, based on Valve's openvr `hellovr_opengl` sample code: https://github.com/ValveSoftware/openvr/tree/master/samples
Still early in development, not very user friendly.

Currently only works with stereo video when used for vr videos, but if the vr video player is launched with the `--plane` option then you can view
the video as a regular video in vr without depth (like a cinema).

# Building
vr video player can be built by running `./build.sh` or by using [sibs](https://git.dec05eba.com/sibs) or if you are running Arch Linux, then you can find it on aur under the name vr-video-player-git (`yay -S vr-video-player-git`).
Dependencies needed when building using `build.sh`: `glm, glew, sdl2, openvr, libx11, libxcomposite, libxfixes`.

# How to use
Install xdotool and launch a video in a video player (I recommend mpv) and fullscreen it or resize it to fit your monitor for best quality and then,

if you want to watch 180 degree stereoscopic videos then run:
```
./vr-video-player $(xdotool selectwindow)
```
and click on your video player.

if you want to watch side-by-side stereoscopic videos or games (flat) then run:
```
./vr-video-player --flat $(xdotool selectwindow)`
```
and click on your video player.

if the side-by-side video is mirrored so that the left side is on the right side of the video, then run:
```
./vr-video-player --flat --right-left $(xdotool selectwindow)
```
and click on your video player.

if you want to watch a regular non-stereoscopic video, then run:
```
./vr-video-player --plane $(xdotool selectwindow)
```
and click on your video player.

The video might not be in front of you, so to move the video in front of you, you can do any of the following:
* Pull the trigger on the vr controller
* Press "Alt + F1"
* Press the "W" key while the vr-video-player is focused
* Press the select/back button on an xbox controller while the application is focused
* Send a SIGUSR1 signal to the application, using the following command: `killall -USR1 vr-video-player`

You can zoom the view with the Q and E keys when the vr-video-player is focused.

You can launch vr-video-player without any arguments to show a list of all arguments.

Note: If the cursor position is weird and does not match what you are seeing in stereoscopic vr mode, then try running the vr video player with the --cursor-wrap option:

--cursor-wrap and --no-cursor-wrap changes the behavior of the cursor in steroscopic mode. Usually in games the game view is mirrored but the cursor is not and the center of the
game which is normally at the center moves to 1/4 and 3/4 of the window. With --cursor-wrap, the cursor position in VR will match the real position of the
cursor relative to the window and with --no-cursor-wrap the cursor will match the position of the cursor as the game sees it.

Note: --cursor-scale is set to 0 by default in 180 degrees stereoscopic mode and 2 in other modes. Also --no-cursor-wrap is set by default in --flat mode.

# Games
This vr video player can also be used to play games in VR to to get a 3D effect, and even for games that don't support VR.\
For games such as Trine that have built-in side-by-side view, you can launch it with proton and there is a launch option for side-by-side view. Select this and when the game launches, get the X11 window id of the game
and launch vr video player with the `--flat` option.\
For games that do not have built-in side-by-side view, you can use [ReShade](https://reshade.me/) (or [vkBasalt](https://github.com/DadSchoorse/vkBasalt) for linux native games) and [SuperDepth3D_VR.fx](https://github.com/BlueSkyDefender/Depth3D) effect with proton. This will make the game render with side-by-side view and you can then get the X11 window id of the game and launch vr video player with the `--flat` option. The game you are playing might require settings to be changed manually in ReShade for SuperDepth3D_VR to make it look better.

# SteamVR issues
SteamVR on linux has several issues. For example if you launch vr-video-player it may get stuck with a "Next up" window inside vr. If that is the case, then close SteamVR and make sure all SteamVR are dead (kill them if they aren't) and launch vr-video-player and it should launch SteamVR (this is different than launching the SteamVR application in steam).

# Screenshots
![](https://www.dec05eba.com/images/vr-video-player.jpg)
