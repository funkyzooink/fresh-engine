#!/usr/bin/env python

import getopt
import json
import os
import shutil
import subprocess
import sys
import urllib2

BUILD_PATH_IOS = "build-ios"
BUILD_PATH_MAC = "build-mac"
BUILD_PATH_LINUX = "build-linux"

app_name = ""
config_file_path = ""

def terminal_output(text):
    print text

def reset_root():
    path = sys.path[0]
    os.chdir(path)

def setup_android(config):    
    android_bundle_id = config['android_bundle_id']
    version_name = config['version_name']

    copy_android_files(android_bundle_id, version_name)

def setup_ios(config):
    ios_bundle_id = config['ios_bundle_id']
    version_name = config['version_name']

    copy_ios_files(ios_bundle_id, version_name)
    run_ios_cmake()

def setup_mac(config):
    ios_bundle_id = config['ios_bundle_id']

    copy_mac_files(ios_bundle_id)
    run_mac_cmake()

def setup_linux(config):
    copy_linux_files()
    run_linux_cmake()

def copy_color_plugin():
    # TODO this is only for LRA - refactor
    src = config_file_path + '/code'
    if os.path.isdir(src): 
        terminal_output('copy color plugin')
        shutil.copyfile(src + '/ColorPlugin.h', 'Classes/Helpers/ColorPlugin.h')
        shutil.copyfile(src + '/ColorPlugin.cpp', 'Classes/Helpers/ColorPlugin.cpp')

def copy_resources():
    src = config_file_path + '/Resources' #todo
    dest = 'Resources'
    copy_files(src, dest)

def copy_ios_files(bundle_id, version_name):
    # iOS
    dest = 'proj.ios_mac/'
    if not os.path.isdir(dest): 
        create_directory(dest)

    src = 'templates/proj.ios_mac/ios'
    dest = 'proj.ios_mac/ios'
    copy_files(src, dest)

    infoplistpath = dest + '/Info.plist'
    replace_in_file(infoplistpath, bundle_id, 'org.cocos2dx.hellocpp')
    terminal_output('ios App bundleid set to: %s in folder: %s ' % (bundle_id, infoplistpath))

    #ios specific resources
    src = config_file_path + '/ios/Images.xcassets' #todo
    dest = dest + '/Images.xcassets'
    copy_files(src, dest)

def copy_android_files(bundle_id, version_name):
    # Android
    src = 'templates/proj.android'
    dest = 'proj.android'
    if os.path.isdir(dest): 
        terminal_output('Workspace not clean')
        sys.exit(2)
    copy_files(src, dest)

    src = 'templates/proj.android'
    dest = 'proj.android'
    set_android_values(dest, bundle_id, version_name)

    # rename android folders
    short_app_name = app_name.replace(" ", "")
    src = 'proj.android/app/jni/hellocpp'
    dest = 'proj.android/app/jni/' + short_app_name.lower()
    os.rename(src, dest)

    #android specific ressources
    src = config_file_path + 'android/res/mipmap-hdpi' #todo
    dest = 'proj.android/app/res/mipmap-hdpi'
    copy_files(src, dest)

    src = config_file_path + 'android/res/mipmap-mdpi' #todo
    dest = 'proj.android/app/res/mipmap-mdpi'
    copy_files(src, dest)

    src = config_file_path + 'android/res/mipmap-xhdpi' #todo
    dest = 'proj.android/app/res/mipmap-xhdpi'
    copy_files(src, dest)

    src = config_file_path + 'android/res/mipmap-xxhdpi' #todo
    dest = 'proj.android/app/res/mipmap-xxhdpi'
    copy_files(src, dest)

    src = config_file_path + 'android/res/mipmap-xxxhdpi' #todo
    dest = 'proj.android/app/res/mipmap-xxxhdpi'
    copy_files(src, dest)

def copy_linux_files():
    # Linux
    src = 'templates/proj.linux'
    dest = 'proj.linux'
    if os.path.isdir(dest): 
        terminal_output('Workspace not clean')
        sys.exit(2)
    copy_files(src, dest)

def copy_mac_files(bundle_id):
    dest = 'proj.ios_mac/'
    if not os.path.isdir(dest): 
        create_directory(dest)

    src = 'templates/proj.ios_mac/mac'
    dest = 'proj.ios_mac/mac'
    copy_files(src, dest)

    infoplistpath = dest + '/Info.plist'
    replace_in_file(infoplistpath, bundle_id, 'org.cocos2dx.hellocpp')
    terminal_output('mac App bundleid set to: %s in folder: %s ' % (bundle_id, infoplistpath))

    #mac specific resources
    src = config_file_path + '/mac/Images.xcassets' #todo
    dest = dest + '/Images.xcassets'
    copy_files(src, dest)

def copy_cmake():
    # cmake
    src = 'templates/CMakeLists.txt'
    dest = 'CMakeLists.txt'
    shutil.copyfile(src, dest)
    short_app_name = app_name.replace(" ", "")  
    replace_in_file(dest, short_app_name.lower(), 'hellocpp') #TODO remove lower breaks android ci build

def run_ios_cmake():
    dest = BUILD_PATH_IOS
    create_directory(dest)
    os.chdir(dest)
    terminal_output('create ios project file')
    subprocess.call(["cmake", "..",  "-GXcode", "-DCMAKE_SYSTEM_NAME=iOS", "-DCMAKE_OSX_SYSROOT=iphoneos"])

    reset_root()

def run_mac_cmake():
    dest = BUILD_PATH_MAC
    create_directory(dest)
    os.chdir(dest)
    terminal_output('create mac project file')
    subprocess.call(["cmake", "..",  "-GXcode"])

    reset_root()

def run_linux_cmake():
    dest = BUILD_PATH_LINUX
    create_directory(dest)
    os.chdir(dest)
    terminal_output('create linux project file')
    subprocess.call(["cmake", ".."])

    reset_root()
def copy_files(src, dest):
    try:
        shutil.copytree(src, dest)
    # Directories are the same
    except shutil.Error as e:
        print('Directory not copied. Error: %s' % e)
    # Any error saying that the directory doesn't exist
    except OSError as e:
        print('Directory not copied. Error: %s' % e)

    terminal_output('Files copied to: %s' % dest)

def copy_file(src, dest):
    if os.path.exists(src): 
        terminal_output('copy files:' + src)
        shutil.copyfile(src, dest)
    else :
        terminal_output('files does not exist:' + src)

def set_android_values(path, bundle_id, version_name):
    manifestpath = path + '/app/AndroidManifest.xml'
    replace_in_file(manifestpath, bundle_id, 'org.cocos2dx.hellocpp')
    terminal_output('App bundleid set to: %s in folder: %s ' % (bundle_id, manifestpath))

    stringvaluepath = path + '/app/res/values/strings.xml'
    replace_in_file(stringvaluepath, app_name, 'HelloCpp')
    terminal_output('App name set to: %s in folder: %s ' % (app_name, stringvaluepath))

    gradlepath = path + '/settings.gradle'
    replace_in_file(gradlepath, app_name, 'HelloCpp')

    gradlepath = path + '/app/build.gradle'
    replace_in_file(gradlepath, bundle_id, 'org.cocos2dx.hellocpp')

    code = 'majorVersion = ' + str(version_name)
    replace_in_file(gradlepath, code , 'majorVersion = replace')

    name = 'versionName \"' + version_name + '\"'
    replace_in_file(gradlepath, name, 'versionName \"1.0\"')

def replace_in_file(filepath, new_value, old_value):
    with open (filepath, 'r') as file:
        filedata = file.read().decode("utf8")
        filedata = filedata.replace(old_value, new_value)
    with open (filepath, 'w') as file:
        file.write(filedata.encode("utf8"))

def clean_folders():
    dest = 'proj.ios_mac'
    if os.path.isdir(dest): 
        shutil.rmtree(dest)
        terminal_output('Removed %s' % dest)

    dest = 'proj.android'
    if os.path.isdir(dest): 
        shutil.rmtree(dest)
        terminal_output('Removed %s' % dest)

    dest = 'proj.linux'
    if os.path.isdir(dest): 
        shutil.rmtree(dest)
        terminal_output('Removed %s' % dest)

    dest = 'Resources'
    if os.path.isdir(dest): 
        shutil.rmtree(dest)
        terminal_output('Removed %s' % dest)

    dest = BUILD_PATH_IOS
    if os.path.isdir(dest): 
        shutil.rmtree(dest)
        terminal_output('Removed %s' % dest)

    dest = BUILD_PATH_MAC
    if os.path.isdir(dest): 
        shutil.rmtree(dest)
        terminal_output('Removed %s' % dest)

    dest = BUILD_PATH_LINUX
    if os.path.isdir(dest): 
        shutil.rmtree(dest)
        terminal_output('Removed %s' % dest)

def ci_build(platforms):
    terminal_output('Starting Ci Build')
    global config_file_path
    config_file_path = "examples/little-ninja/" # if not on tag use this as fallback CI build

    if os.environ.get('TRAVIS_TAG'):
        tagname = os.environ["TRAVIS_TAG"]

        if "little-ninja" in tagname:
            config_file_path = "examples/little-ninja/"
        elif "little-robot-adventure" in tagname:
            config_file_path = "examples/little-robot-adventure/"
        elif "the-dragon-kid" in tagname:
            config_file_path = "examples/the-dragon-kid/"
        elif "4friends" in tagname:
            config_file_path = "examples/4friends/"

    project_copy_helper(platforms)

def create_directory(path):
        try:
            os.mkdir(path)
        except OSError:
            print ("Creation of the directory %s failed" % path)
        else:
            print ("Successfully created the directory %s " % path)

def ci_appimage():

    if os.environ.get('TRAVIS_TAG'):
        tagname = os.environ["TRAVIS_TAG"]
        project_name = "little-ninja" # if not on tag use this as fallback CI build 

        if "little-ninja" in tagname:
            project_name = "little-ninja"
        elif "little-robot-adventure" in tagname:
            project_name = "little-robot-adventure"
        elif "the-dragon-kid" in tagname:
            project_name = "the-dragon-kid"
        elif "4friends" in tagname:
            project_name = "4friends"
        else :
            sys.exit(0)

        short_app_name = app_name.replace(" ", "").lower()

        # create directories
        dest = project_name + '.AppDir'
        create_directory(dest)

        path = dest + '/usr'
        create_directory(path)
        
        src_path = 'examples/'+ project_name + '/android/web_hi_res_512.png'
        dest_path = dest + '/' + project_name + '.png'
        copy_file(src_path, dest_path)

        src_path = 'examples/'+ project_name + '/'+ project_name + '.desktop'
        dest_path = dest + '/' + project_name + '.desktop'
        copy_file(src_path, dest_path)

        # bin files
        src_path = BUILD_PATH_LINUX + '/bin/' + short_app_name
        dest_path = dest + '/bin'
        copy_files(src_path, dest_path)

        # lib files
        src_path = BUILD_PATH_LINUX + '/lib'
        dest_path = dest + '/usr/lib'
        copy_files(src_path, dest_path)

        src_path = 'cocos2d/external/linux-specific/fmod/prebuilt/64-bit/libfmod.so'
        dest_path = dest + '/usr/lib/libfmod.so'
        copy_file(src_path, dest_path)

        src_path = 'cocos2d/external/linux-specific/fmod/prebuilt/64-bit/libfmod.so.6'
        dest_path = dest + '/usr/lib/libfmod.so.6'
        copy_file(src_path, dest_path)

        src_path = 'cocos2d/external/linux-specific/fmod/prebuilt/64-bit/libfmodL.so'
        dest_path = dest + '/usr/lib/libfmodL.so.6'
        copy_file(src_path, dest_path)

        src_path = 'cocos2d/external/linux-specific/fmod/prebuilt/64-bit/libfmodL.so'
        dest_path = dest + '/usr/lib/libfmodL.so.6'
        copy_file(src_path, dest_path)

        src_path = 'cocos2d/external/linux-specific/fmod/prebuilt/64-bit/libfmodL.so'
        dest_path = dest + '/usr/lib/libfmodL.so.6'
        copy_file(src_path, dest_path)

        src_path = '/usr/lib/x86_64-linux-gnu/libpng12.so.0'
        dest_path = dest + '/usr/lib/libpng12.so.0'
        copy_file(src_path, dest_path)

        src_path = '/usr/lib/x86_64-linux-gnu/libcurl-gnutls.so.4'
        dest_path = dest + '/usr/lib/libcurl-gnutls.so.4'
        copy_file(src_path, dest_path)

        src_path = '/usr/lib/x86_64-linux-gnu/libGLEW.so.1.13'
        dest_path = dest + '/usr/lib/libGLEW.so.1.13'
        copy_file(src_path, dest_path)

        # get apprun file        
        url = 'https://raw.githubusercontent.com/AppImage/AppImageKit/master/resources/AppRun'
        filedata = urllib2.urlopen(url)
        datatowrite = filedata.read()
        
        with open(dest + '/AppRun', 'wb') as f:
            f.write(datatowrite)

        os.chmod(dest + '/AppRun', 0o755)

        # create appimage
        url = 'https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage'
        filedata = urllib2.urlopen(url)
        datatowrite = filedata.read()
        
        with open('appimagetool-x86_64.AppImage', 'wb') as f:
            f.write(datatowrite)
            
        os.chmod('appimagetool-x86_64.AppImage', 0o755)
        os.environ["ARCH"] = "x86_64 "
        subprocess.call('./appimagetool-x86_64.AppImage ' + dest, shell = True)

        # rename appimage file
        tagname = os.environ["TRAVIS_TAG"]
        os.rename(project_name + '-x86_64.AppImage', tagname + '-linux.AppImage')

def ci_macimage():

    short_app_name = app_name.replace(" ", "").lower()

    appname = short_app_name + '.app'
    src_path = 'bin/' + short_app_name + '/Release/' + appname
    dest_path = '..'
    copy_file(src_path, dest_path)
    # rename app file
    tagname = os.environ["TRAVIS_TAG"]
    os.rename(appname, tagname + '.app')

def ci_deploy(): # TODO for fastlane

    if os.environ.get('TRAVIS_TAG'):
        tagname = os.environ["TRAVIS_TAG"]

        if "little-ninja" in tagname:
            project_path = "examples/little-ninja/"
        elif "little-robot-adventure" in tagname:
            project_path = "examples/little-robot-adventure/"
        elif "the-dragon-kid" in tagname:
            project_path = "examples/the-dragon-kid/"
        elif "4friends" in tagname:
            project_path = "examples/4friends/"

def project_copy_helper(platforms):
    global app_name
    config = json.loads(open(config_file_path + "/config.json").read())
    app_name = config['app_name']

    copy_resources()
    copy_cmake()
    copy_color_plugin()

    for platform in platforms:
        terminal_output('Copy files for platform: ' + platform)
        if platform is "android":
            setup_android(config)
        elif platform is "ios":
            setup_ios(config)
        elif platform is "mac":
            setup_mac(config)
        elif platform is "linux":
            setup_linux(config)

def main(argv):
    global config_file_path
    platform = []
    build_ci = None

    try:
      opts, args = getopt.getopt(argv,"n:crd",["config_file_path=", "clean", "travis", "appimage", "macapp", "deploy", "android", "ios", "linux", "mac"])
    except getopt.GetoptError:
      terminal_output("Wrong argument specified")
      sys.exit(2)

    for opt, arg in opts:
        terminal_output("build argument: " + opt)
        if opt in ("-r", "--travis"):
            build_ci = True
        elif opt in ("--android"):
            platform.append("android")
        elif opt in ("--ios"):
            platform.append("ios")
        elif opt in ("--linux"):
            platform.append("linux")
        elif opt in ("--mac"):
            platform.append("mac")
        elif opt in ("-n", "--config_file_path"):
            config_file_path = arg
        elif opt in ("--appimage"):
            ci_appimage()
            sys.exit(0)
        elif opt in ("--macapp"):
            ci_macimage()
            sys.exit(0)
        elif opt in ("-d", "--deploy"):
            ci_deploy() #todo
            sys.exit(0)
        elif opt in ("-c", "--clean"):
            clean_folders()
            sys.exit(0)

    if build_ci:
        ci_build(platform)
    elif config_file_path != "":
        project_copy_helper(platform)
    else :  
        terminal_output('Missing Arguments: config_file_path %s' % (config_file_path))

if __name__ == "__main__":    
    if len(sys.argv) < 2:
        terminal_output("please run with arguments")
        terminal_output("for cleaning: build.py -c")
        terminal_output("for creating project files: build.py -n <folder path> -p")
        terminal_output("and platform: -android -ios -linux -mac")
        sys.exit(0)

    main(sys.argv[1:])
