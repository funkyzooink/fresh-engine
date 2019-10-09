# funky fresh engine
fresh engine is a platformer engine build on top of cocos2dx


# How to
1. Clone the repo from GitHub.

        $ git clone https://github.com/funkyzooink/fresh-engine.git

2. Get cocos2dx submodule

        $ cd fresh-engine
        fresh-engine $ git submodule update --init

3. Setup cocos2dx 

        fresh-engine $ cd cocos2d
        fresh-engine/cocos2d $ python2 download-deps.py
        fresh-engine/cocos2d $ git submodule update --init

4. clone a demo project in the same source folder

`project\fresh engine`

`project\`[little-ninja](https://github.com/funkyzooink/little-ninja)

`project\`[the-dragon-kid](https://github.com/funkyzooink/the-dragon-kid)


5. create project files

        fresh-engine $ python2 build.py -n ../the-dragon-kid/

6. compile 

        fresh-engine $ python2 build.py -p android


# Contributing to the Project

Fresh Engine is licensed under the [MIT License](https://opensource.org/licenses/MIT). We welcome participation!
