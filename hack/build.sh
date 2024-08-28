#!/usr/bin/env bash

CMAKE_BUILD_DIR="cmake-build-release"

cmake -G Ninja -S $(pwd) -B ${CMAKE_BUILD_DIR} \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_MAKE_PROGRAM=`which ninja` \
	-DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT:-'~/vcpkg'}/scripts/buildsystems/vcpkg.cmake

cmake --build ${CMAKE_BUILD_DIR} --target all -j `nproc`

cd ${CMAKE_BUILD_DIR}
APP_NAME=$(cd *.app ; echo $(basename $(pwd)))

cat >$APP_NAME/Contents/Info.plist<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleDevelopmentRegion</key>
	<string>English</string>
	<key>CFBundleExecutable</key>
	<string>hitokoto-desktop-ng</string>
	<key>CFBundleGetInfoString</key>
	<string></string>
	<key>CFBundleIconFile</key>
	<string></string>
	<key>CFBundleIdentifier</key>
	<string></string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleLongVersionString</key>
	<string></string>
	<key>CFBundleName</key>
	<string></string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleShortVersionString</key>
	<string></string>
	<key>CFBundleSignature</key>
	<string>????</string>
	<key>CFBundleVersion</key>
	<string></string>
	<key>CSResourcesFileMapped</key>
	<true/>
	<key>NSHumanReadableCopyright</key>
	<string></string>
	<key>LSUIElement</key>
	<string>1</string>
</dict>
</plist>
EOF

# 生成证书
# https://developer.apple.com/help/account/create-certificates/create-a-certificate-signing-request
# 不签名无法在开启了 SIP 的系统中运行
codesign -s "WireDolphin" $APP_NAME || true
codesign --display --verbose=4 $APP_NAME

csrutil status
macdeployqt6 $APP_NAME -dmg
cd ..

mv ${CMAKE_BUILD_DIR}/*.dmg .
rm -rf ${CMAKE_BUILD_DIR}
