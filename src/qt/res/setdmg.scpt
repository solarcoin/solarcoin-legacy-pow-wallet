tell application "Finder"
    tell disk "SolarCoin-Qt"
       open
       set current view of container window to icon view
       set toolbar visible of container window to false
       set statusbar visible of container window to false
       set the bounds of container window to {400, 100, 1000, 550}
       set theViewOptions to the icon view options of container window
       set arrangement of theViewOptions to not arranged
       set icon size of theViewOptions to 72
       set background picture of theViewOptions to file ".background:solarcoin.png"
       #make new alias file at container window to POSIX file "/Applications" with properties {name:"Applications"}
       set position of item "SolarCoin-Qt.app" of container window to {135, 345}
       set position of item "Applications" of container window to {470, 345}
       update without registering applications
       delay 5
       eject
    end tell
end tell
