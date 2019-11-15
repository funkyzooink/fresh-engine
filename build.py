#!/usr/bin/env python

import getopt
import json
import os
import shutil
import subprocess
import sys

def terminal_output(text):
    print text

def copy_resources(config_file_path, android_platform):
    src = config_file_path + '/Resources' #todo
    dest = 'Resources'
    copy_files(src, dest)

    #android specific
    src = config_file_path + 'android/' + android_platform + '/res/mipmap-hdpi' #todo
    dest = 'proj.android/app/res/mipmap-hdpi'
    copy_files(src, dest)

    src = config_file_path + 'android/' + android_platform + '/res/mipmap-mdpi' #todo
    dest = 'proj.android/app/res/mipmap-mdpi'
    copy_files(src, dest)

    src = config_file_path + 'android/' + android_platform + '/res/mipmap-xhdpi' #todo
    dest = 'proj.android/app/res/mipmap-xhdpi'
    copy_files(src, dest)

    src = config_file_path + 'android/' + android_platform + '/res/mipmap-xxhdpi' #todo
    dest = 'proj.android/app/res/mipmap-xxhdpi'
    copy_files(src, dest)

    src = config_file_path + 'android/' + android_platform + '/res/mipmap-xxxhdpi' #todo
    dest = 'proj.android/app/res/mipmap-xxxhdpi'
    copy_files(src, dest)

    #ios specific
    src = config_file_path + '/ios/Images.xcassets' #todo
    dest = 'proj.ios_mac/ios/Images.xcassets'
    copy_files(src, dest)

def copy_templates():
    # iOS
    src = 'templates/proj.ios_mac'
    dest = 'proj.ios_mac'
    if os.path.isdir(dest): 
        terminal_output('Workspace not clean')
        sys.exit(2)

    copy_files(src, dest)

    # Android
    src = 'templates/proj.android'
    dest = 'proj.android'
    if os.path.isdir(dest): 
        terminal_output('Workspace not clean')
        sys.exit(2)
    copy_files(src, dest)

    # Linux
    src = 'templates/proj.linux'
    dest = 'proj.linux'
    if os.path.isdir(dest): 
        terminal_output('Workspace not clean')
        sys.exit(2)
    copy_files(src, dest)

def prepare_templates(app_name, android_bundle_id, ios_bundle_id, version_name):
    src = 'templates/proj.ios_mac'
    dest = 'proj.ios_mac'
    set_ios_values(dest, app_name, ios_bundle_id) # TODO version number

    src = 'templates/proj.android'
    dest = 'proj.android'
    set_android_values(dest, app_name, android_bundle_id, version_name)

    # rename android folders
    app_name = app_name.replace(" ", "")
    src = 'proj.android/app/jni/hellocpp'
    dest = 'proj.android/app/jni/' + app_name.lower()
    os.rename(src, dest)

    # cmake
    src = 'templates/CMakeLists.txt'
    dest = 'CMakeLists.txt'
    shutil.copyfile(src, dest)
    replace_in_file(dest, app_name.lower(), 'hellocpp')

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

def set_android_values(path, app_name, bundle_id, version_name):
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

    mkpath = path + '/app/jni/Android.mk'
    replace_in_file(mkpath, app_name.lower(), 'hellocpp')

def set_ios_values(path, app_name, bundle_id):
    srcfolder = path + '/HelloCpp.xcodeproj'
    destfolder = path + '/' + app_name + '.xcodeproj'
    filepath = srcfolder + '/project.pbxproj'

    # App name
    replace_in_file(filepath, app_name, 'HelloCpp')
    os.rename(srcfolder, destfolder)
    terminal_output('App name set to: %s in folder: %s ' % (app_name, destfolder))

    # bundle id
    infoplistpath = path + '/ios/Info.plist'
    replace_in_file(infoplistpath, bundle_id, 'org.cocos2dx.hellocpp')
    terminal_output('ios App bundleid set to: %s in folder: %s ' % (bundle_id, infoplistpath))

# TODO mac build, remove?
    #infoplistpath = path + '/mac/Info.plist'
    #replace_in_file(infoplistpath, bundle_id, 'org.cocos2dx.hellocpp')
    #terminal_output('mac App bundleid set to: %s in folder: %s ' % (bundle_id, infoplistpath))

def replace_in_file(filepath, new_value, old_value):
    with open (filepath, 'r') as file:
        filedata = file.read()
        filedata = filedata.replace(old_value, new_value)
    with open (filepath, 'w') as file:
        file.write(filedata)

def build(platform, build_type):
    terminal_output('building platform as %s %s: ' % (platform, build_type))
    subprocess.call(["cocos2d//tools//cocos2d-console//bin//cocos", "compile",  "-p", platform, "-m", build_type , "-j", "16"])

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
def ci_build():
    project_path = "examples/little-ninja/"

    if os.environ.get('TRAVIS_TAG'):
        tagname = os.environ["TRAVIS_TAG"]

        if "little-ninja" in tagname:
            project_path = "examples/little-ninja/"
        elif "the-dragon-kid" in tagname:
            project_path = "examples/the-dragon-kid/"
        elif "4friends" in tagname:
            project_path = "examples/4friends/"

    project_copy_helper(project_path, 'play')

def project_copy_helper(config_file_path, android_platform):
    config = json.loads(open(config_file_path + "/config.json").read())
    app_name = config['app_name']
    bundle_id_name = android_platform +  '_bundle_id'
    android_bundle_id = config[bundle_id_name]
    ios_bundle_id = config['ios_bundle_id']
    version_name = config['version_name']
    copy_templates()
    prepare_templates(app_name, android_bundle_id, ios_bundle_id, version_name)
    copy_resources(config_file_path, android_platform)

def main(argv):
    platform = ''
    build_type = 'release'

    config_file_path = ''

    try:
      opts, args = getopt.getopt(argv,"p:a:m:n:tcr",["platform=", "android-platform=", "build_type=", "config_file_path=", "template", "clean", "travis"])
    except getopt.GetoptError:
      terminal_output("Wrong argument specified")
      sys.exit(2)

    android_platform = "play" #todo can be removed when underground will be removed

    for opt, arg in opts:
        if opt in ("-r", "--travis"):
            ci_build()
            sys.exit(0)
        elif opt in ("-p", "--platform"):
            platform = arg
        elif opt in ("-a", "--android-platform"):
            android_platform = arg
        elif opt in ("-m", "--build_type"):
            build_type = arg
        elif opt in ("-n", "--config_file_path"):
            config_file_path = arg
        elif opt in ("-t", "--template"):
            copy_templates()
            sys.exit(0)
        elif opt in ("-c", "--clean"):
            clean_folders()
            sys.exit(0)
          

    if platform != "":           
        build(platform, build_type)
    elif config_file_path != "":
        project_copy_helper(config_file_path, android_platform)
    else :  
        terminal_output('Missing Arguments: platform: %s, build_type %s, config_file_path %s' % (platform, build_type, config_file_path))

if __name__ == "__main__":    
    if len(sys.argv) < 2:
        terminal_output("please run with arguments")
        terminal_output("for cleaning: build.py -c")
        terminal_output("for copying empty templates: build.py -t")
        terminal_output("for compiling: build.py -p <iOS|Android> -m <release|debug>")
        terminal_output("for creating project files: build.py -n <folder path>")
        sys.exit(0)

    main(sys.argv[1:])