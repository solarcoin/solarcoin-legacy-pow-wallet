mkdir "$2/SolarCoin-Qt"
mkdir "$2/SolarCoin-Qt/.background"
cp -R "$2/SolarCoin-Qt.app" "$2/SolarCoin-Qt/SolarCoin-Qt.app"
cp "$1/src/qt/res/images/dmg_bg.png" "$2/SolarCoin-Qt/.background/solarcoin.png"
ln -s /Applications/ "$2/SolarCoin-Qt/Applications"

hdiutil create -srcfolder "$2/SolarCoin-Qt" -volname "SolarCoin-Qt" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size 100m "$2/pack.temp.dmg"
device=$(hdiutil attach -readwrite -noverify -noautoopen "$2/pack.temp.dmg" | egrep '^/dev/' | sed 1q | awk '{print $1}')
sleep 15
osascript "$1/src/qt/res/setdmg.scpt"
#chmod -Rf go-w /Volumes/SolarCoin-Qt
#sync
#sync
#hdiutil detach ${device}
hdiutil convert "$2/pack.temp.dmg" -format UDZO -imagekey zlib-level=9 -o "$2/SolarCoin-Qt"
rm -f "$2/pack.temp.dmg"
