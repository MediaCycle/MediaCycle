MacPorts for MediaCycle Howto

Once MacPorts is installed:

1) Edit /opt/local/etc/macports/variants.conf and update it accordingly to variants.conf from the current folder

2) Add the full path to this ports folder to /opt/local/etc/macports/sources.conf ABOVE all the other entries:
file://.../doc/macports/ports
(replace '...' by the appropriate path to your MediaCycle source root)
