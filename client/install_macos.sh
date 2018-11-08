#!/bin/sh
# install SDL2 and mount dmg
curl https://www.libsdl.org/release/SDL2-2.0.9.dmg -o SDL2.dmg
hdiutil attach SDL2.dmg
# install SDL2_image
curl https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.4.dmg -o SDL2_image.dmg
hdiutil attach SDL2_image.dmg
# install SDL2_image
curl https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.14.dmg -o SDL2_ttf.dmg
hdiutil attach SDL2_ttf.dmg
# move
mv SDL2.framework SDL2_image.framework SDL2_ttf.framework /Libary/Frameworks/
