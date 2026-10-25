glad spec :
https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D4.6&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=core&loader=on
later i used the glad package on archlinux

sudo pacman -S glad
glad --api gl:core=4.6 --out-path . --reproducible c
