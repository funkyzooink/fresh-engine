# funky fresh engine
[![Build Status](https://api.travis-ci.com/funkyzooink/fresh-engine.svg?branch=master)](https://travis-ci.com/github/funkyzooink/fresh-engine)

fresh engine is a platformer engine build on top of cocos2dx.

curently Android, iOS, Linux and MAC are supported

[see here for some demo projects](https://funkyzooink.github.io/projects/)


# How to
1. Clone the repo from GitHub.

        $ git clone https://github.com/funkyzooink/fresh-engine.git

2. Get cocos2dx submodule

        $ cd fresh-engine
        fresh-engine $ git submodule update --init

3. Setup [cocos2dx](https://github.com/cocos2d/cocos2d-x) , for more details check their [github](https://github.com/cocos2d/cocos2d-x)

        fresh-engine $ cd cocos2d
        fresh-engine/cocos2d $ python2 download-deps.py
        fresh-engine/cocos2d $ git submodule update --init


4. create project files for one of the examples

`fresh-engine\examples\`[4friends](https://github.com/funkyzooink/4friends)

`fresh-engine\examples\`[little-ninja](https://github.com/funkyzooink/little-ninja)

`fresh-engine\examples\`[little-robot-adventure](https://github.com/funkyzooink/little-robot-adventure)

`fresh-engine\examples\`[the-dragon-kid](https://github.com/funkyzooink/the-dragon-kid)

        fresh-engine $ python2 build.py -n examples/little-ninja/ --ios --android

5. open project files 
- android studio: `proj.android`
- xcode: `build-mac` / `build-ios`
- linux: `build-linux`


# Contributing to the Project

Fresh Engine is licensed under the [MIT License](https://opensource.org/licenses/MIT). We welcome participation!
