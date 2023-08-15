# Set up mitmproxy to sniff TLS traffic between Bosch Smart Home App and Cloud

In this post you will learn how to set up [mitmproxy](https://mitmproxy.org/) to intercept TLS traffic between the Bosch Smart Home App and the Bosch Smart Home Cloud. Warning: this setup only works in a remote setting involving the cloud, i.e. app and SHC are **not** in the same network.


## Prerequisites

 * Android emulator with a device running (I'm using Android 11, API30, newer versions shall work too). Use a device w/o playstore to be able to root it.
 * mitmproxy installed
 
## Modify the Bosch Smart Home App

Download the last Bosch Smart Home App (apk file), e.g. boschsh.apk
  
Create a directory and copy the boschsh.apk in this directory: 
```
  mkdir APKUNZIP
  cd APKUNZIP
  cp ../boschsh.apk .
```
  
Unzip the apk file and then remove it:
```
  unzip boschsh.apk
  rm boschsh.apk
```

Go to the META-INF directory and delete following files (do not delete this directory completely as stated in many tutorials!):
```
  ANDROIDA.RSA  
  ANDROIDA.SF  
  MANIFEST.MF
```

Patch the keystores, these are located in the `res/` directory with the extensions `.bks` (bouncycastle format).
```
  cd res/
  find . -name '*.bks'
```

Try to list certificates from one bks file (donwload the provider from here: https://www.bouncycastle.org/latest_releases.html):
```
  keytool -list -keystore [name].bks -provider "org.bouncycastle.jce.provider.BouncyCastleProvider" -providerPath "bcprov-jdk18on-176.jar" -storetype bks
```

Oh, we need the "secret". Open the original apk file in `jadx` and search for "truststore" (case insensitive) in the code. The "secret" is located in the package `com.bosch.sh.connector.certificate.server` (but that's not really a secret). Try again `keytool -list` with this secret, this shall work.

For **each** bks files, add your own mitmproxy CA certificate:
```
  keytool -import -keystore [name].bks -file [path]/mitmproxy-ca-cert.pem -provider "org.bouncycastle.jce.provider.BouncyCastleProvider" -providerPath "[path]/bcprov-jdk18on-176.jar" -storetype bks
```

Go back and re-zip the content (warning: `resources.arsc` has to be uncompressed!):
```
  cd ..
  zip -r -n "resources.arsc" boschmod.apk *
```

Align:
```
  zipalign -p -f -v 4 boschmod.apk boschmodaligned.apk
```

Sign the apk with your own key (a rooted android emulator accepts it):

Create a key store:
```
  keytool -genkey -alias apkkey -keyalg RSA -keystore apkkeystore.jks -keysize 2048
```

Sign the modified apk with apksigner:
```
  apksigner sign --ks apkkeystore.jks --in boschmodaligned.apk --out boschmodsigned.apk
```

Install the `boschmodsigned.apk` in android emulator and start, this shall work!

## Run mitmproxy

First SHC and App have to be paired in the same network. Then run the App in a remote context (remote access has to be enabled). 
Within Android emulator, enable proxy (extended controls > settings > proxy > manual proxy configuration with pc IP and port=8081). 
Run mitmproxy on the pc running Android emulator:

```
mitmproxy --mode regular@8081 --ssl-insecure --set confdir=MITMCERT/ --rawtcp --set websocket=false
```

Now the traffic between app and cloud can be decrypted!
