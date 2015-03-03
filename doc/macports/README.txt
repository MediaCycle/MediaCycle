MacPorts for MediaCycle Howto

Once MacPorts is installed:

1) Adapt /opt/local/etc/macports/variants.conf:
## OpenCV
#+openni
+opencl
## FFmpeg (uncomment if you produce releases/bundles to be distributed)
#-gpl2 -gpl3 -nonfree
## Qt5 support for OpenCV and OpenSceneGraph and Qwt
+qt5

2) Add the full path to this ports folder to /opt/local/etc/macports/sources.conf ABOVE all the other entries:
file://.../doc/macports/ports
(replace '...' by the appropriate path to your MediaCycle source root)

3) Open a Terminal in ".../doc/macports/ports" and execute:
portindex

4) The local repository should be available, it can currently be tested by searching for the default opencv Porfile:
port file opencv
(should return: ".../doc/macports/ports/graphics/opencv/Portfile")