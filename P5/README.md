# Set up mitmproxy to sniff TLS traffic of the Bosch Smart Home App

In this post you will learn how to set up [mitmproxy](https://mitmproxy.org/) to intercept TLS traffic between the Bosch Smart Home App and the Bosch Smart Home Controller to sniff th traffic. 
Three steps are needed: 
1. modify the app by injecting our own CA certificate to the trusted key store
2. extract the SHC device certificate and key
3. run mitmproxy

## Modifying the Bosch Smart Home App

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

Go to META-INF directory and delete following files (do not delete this directory completely as stated in many tutorials!):
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

For each bks files, add your own mitmproxy CA certificate:
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

Install the apk in android emulator and start, this shall work!

## Extract the SHC device certificate and key

Since the app is communicating with the SHC device with mTLS, mitmproxy also needs the device certificate and key (client certificate).

Having a SHC rooted with `SSH` in place, extract (`SCP`)following files:
```
/data/etc/keys/device_chain.pem
/data/etc/keys/device_key_pair.pem
```

Concatenate both in one file (key and certificate chain, needed by mitmproxy):
```
cp device_key_pair.pem device_key_chain.pem
cat device_chain.pem >> device_key_chain.pem
```

## Run mitmproxy

Run mitmproxy in regular mode (change the pathes to your certificates):

```
mitmproxy --mode regular@8081 --ssl-insecure --set confdir=MITMCERT/ --set tls_version_client_max=TLS1_2 --set tls_version_server_max=TLS1_2 --set client_certs=CERT/device_key_chain.pem
```

Start the Bosch Smart Home app on Android emulator and set up the Smart Home Controller (the controller has to be in the same network as your PC). Put the mac address and key manually. Normally, the SHC won't be found by the app. Put its IP address manually `Enter IP address` and then it will be found. As soon as the configuration is done, we can redirect tre Android emulator traffic to the mitmproxy: in the `Extended Controls`, `settings`, tab `proxy`, switch to `manual proxy configuration` with the IP address of the PC running mitmproxy and port `8081`.


Then start the app on Android emulator. Go to the extended controls, settings, proxy tab. 

