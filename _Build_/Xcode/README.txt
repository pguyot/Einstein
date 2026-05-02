Build this from the command line:

  xcodebuild -scheme Einstein -configuration release clean build

  also try to add -exportPath "Einstein.app"
  
Test the app:

  open /Users/matt/Library/Developer/Xcode/DerivedData/Einstein-fgtapiubsgwdnkddffpdwzjpwaij/Build/Products/Release/Einstein.app

Zip the app and copy it the Sites directory (messagepad.org)

  mv /Users/matt/Library/Developer/Xcode/DerivedData/Einstein-fgtapiubsgwdnkddffpdwzjpwaij/Build/Products/Release/Einstein.app .
  zip -r -9 Einstein.MacOS.native.2020.4.13.zip Einstein.app
  cp Einstein.MacOS.native.2020.4.13.zip ~/Sites/messagepad.org/htdocs/Downloads/Einstein/MacOS/

To override the signing configuration, create a `LocalSigning.xcconfig` here and provide values for:

- DEVELOPMENT_TEAM: Apple team ID
- CODE_SIGN_STYLE: Manual or automatic
- CODE_SIGN_IDENTITY: '-' if not signing, otherwise 'Apple Development' or 'iPhone Developer'
- CODE_SIGN_ENTITLEMENTS: Entitlements file to be used

The local config is overriding the default Signing.xcconfig content, but it is not checked into Git.
