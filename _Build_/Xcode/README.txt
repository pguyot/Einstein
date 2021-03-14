
Build this from the command line:

  xcodebuild -scheme Einstein -configuration release clean build

  also try to add -exportPath "Einstein.app"
  
Test the app:

  open /Users/matt/Library/Developer/Xcode/DerivedData/Einstein-fgtapiubsgwdnkddffpdwzjpwaij/Build/Products/Release/Einstein.app

Zip the app and copy it the Sites directory (messagepad.org)

  mv /Users/matt/Library/Developer/Xcode/DerivedData/Einstein-fgtapiubsgwdnkddffpdwzjpwaij/Build/Products/Release/Einstein.app .
  zip -r -9 Einstein.MacOS.native.2020.4.13.zip Einstein.app
  cp Einstein.MacOS.native.2020.4.13.zip ~/Sites/messagepad.org/htdocs/Downloads/Einstein/MacOS/

