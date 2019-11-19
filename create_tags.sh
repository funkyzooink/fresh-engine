#!/bin/bash

version=$(git rev-list HEAD --count)
# create and push tag then wait 30s - otherwise travis ignores tags
git tag "release-little-ninja-v$version"
git push origin --tags
sleep 30

git tag "release-little-robot-adventure-v$version"
git push origin --tags
sleep 30

git tag "release-the-dragon-kid-v$version"
git push origin --tags
sleep 30