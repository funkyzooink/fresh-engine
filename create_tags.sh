#!/bin/bash

version=$(git rev-list HEAD --count)
git tag "release-little-ninja-v$version"
git tag "release-little-robot-adventure-v$version"
git tag "release-the-dragon-kid-v$version"
git push origin --tags