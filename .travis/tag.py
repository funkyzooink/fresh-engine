#!/usr/bin/env python

import os

if os.environ.get('TRAVIS_TAG'):
    tagname = os.environ["TRAVIS_TAG"]

    if "little-ninja" in tagname:
        os.environ["SUBMODULE_PATH"] = "examples/little-ninja/"
    elif "the-dragon-kid" in tagname:
        os.environ["SUBMODULE_PATH"] = "examples/the-dragon-kid/"
    elif "4friends" in tagname:
        os.environ["SUBMODULE_PATH"] = "examples/4friends/"

if os.environ.get('SUBMODULE_PATH'):
    print(os.environ["SUBMODULE_PATH"])