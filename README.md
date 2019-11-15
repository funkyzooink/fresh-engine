# funky fresh engine
fresh engine is a platformer engine build on top of cocos2dx


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

4. clone a demo project in the same source folder



5. create project files for one of the examples

`fresh-engine\examples\`[little-ninja](https://github.com/funkyzooink/little-ninja)

`fresh-engine\examples\`[little-robot-adventure](https://github.com/funkyzooink/little-robot-adventure)

`fresh-engine\examples\`[the-dragon-kid](https://github.com/funkyzooink/the-dragon-kid)

        fresh-engine $ python2 build.py -n examples/little-ninja

6a. compile using commandline

        fresh-engine $ python2 build.py -p android

6b. or open project files in android studio / xcode / qt creator (linux)


# Contributing to the Project

Fresh Engine is licensed under the [MIT License](https://opensource.org/licenses/MIT). We welcome participation!
