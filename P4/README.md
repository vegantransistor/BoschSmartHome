# Set up a Man in the Middle Proxy to sniff TLS traffic

In this article you will learn how to set up a Man-in-the-Middle proxy between the Bosch Smart Home Controller and the Internet.  

**Precondition:** [root your controller](https://github.com/vegantransistor/BoschSmartHome/blob/main/P1/README.md)

## Set up dnsmasq

On your Linux PC, prepare the Ethernet connection, in this example we are using following configuration:
```
Interface: enp1s0
IP Address: 192.168.2.1
Netmask: 255.255.255.0
Gateway: 192.168.2.1
```

Install `dnsmasq` and configure it:

```
interface=enp1s0
dhcp-range=192.168.2.50,192.168.2.150,255.255.255.0,12h
```

## Set up SSH

Connect the Smart Home Controller to your PC with an Ethernet Cable. Connect the serial interface to your PC and start the SSH daemon on the Smarthome Controller via the serial interface:

```
systemctl start ssh
```
Find out the IP address of the device with `ip addr`:

```
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel qlen 1000
    link/ether 64:da:a0:03:42:39 brd ff:ff:ff:ff:ff:ff
    inet 169.254.152.138/16 brd 169.254.255.255 scope link eth0
       valid_lft forever preferred_lft forever
    inet 192.168.2.58/24 brd 192.168.2.255 scope global dynamic eth0
       valid_lft 43198sec preferred_lft 43198sec
    inet6 fe80::66da:a0ff:fe03:4239/64 scope link 
       valid_lft forever preferred_lft forever 
```
Then connect (password is your password from [1](https://github.com/vegantransistor/BoschSmartHome/blob/main/P1/README.md)):
```
ssh root@192.168.2.58
```

## Install the Man in the Middle Proxy

Install [mitmproxy](https://mitmproxy.org/) and check the mitm root CA in `~/.mitmproxy`. We will use `pem`, `der`, `crt` and java keystore `jks` formats.

## Replace the Root Certificates in the device

In order to be able to catch the TLS traffic we need to replace the root certificates in the device with the mitm root CA. Some (most) of them are located in `/data/etc/certificates/truststore/` and this directore is not protected by dm-veritiy. Replace all certificates with the mitm root CA certificate. 

For the java keystore file, you can add the mitm root CA certificate to the truststore with `keytool` (the password is `key4SH`):
```
keytool -importcert -keystore bosch-smarthome.jks -file mitmproxy-ca-cert.cer -alias mitm
```

## Extract the device certificate
Each Controller has an own certificate, we need it (and its associated private key) for the proxy connection. Go to `/etc/data/ecm/keys/` and copy `device_chain.pem` and `device_key_pair.pem`. Concatenate both files into one called `device_key_chain.pem`.

## Start mitmproxy

On the PC, start mitmproxy. First restart dnsmasq and setup ip forwarding and ip tables:
```
sudo systemctl restart dnsmasq
sudo sysctl -w net.ipv4.ip_forward=1
sudo sysctl -w net.ipv6.conf.all.forwarding=1
sudo sysctl -w net.ipv4.conf.all.send_redirects=0
sudo iptables -t nat -A PREROUTING -i enp1s0 -p tcp --dport 80 -j REDIRECT --to-port 8080
sudo iptables -t nat -A PREROUTING -i enp1s0 -p tcp --dport 443 -j REDIRECT --to-port 8080
sudo iptables -t nat -A PREROUTING -i enp1s0 -p tcp --dport 8883 -j REDIRECT --to-port 8080
sudo ip6tables -t nat -A PREROUTING -i enp1s0 -p tcp --dport 80 -j REDIRECT --to-port 8080
sudo ip6tables -t nat -A PREROUTING -i enp1s0 -p tcp --dport 443 -j REDIRECT --to-port 8080
sudo ip6tables -t nat -A PREROUTING -i enp1s0 -p tcp --dport 8883 -j REDIRECT --to-port 8080
```

Then start the proxy:
```
mitmproxy --rawtcp --set websocket=false --mode transparent --ssl-insecure --set confdir=[your path for mitm root CA] --set tls_version_client_max=TLS1_2 --set tls_version_server_max=TLS1_2 --set client_certs=[your path for device certificate]/device_key_chain.pem
```

Normally you shall see the (decrypted) traffic in the mitmproxy application and most of the http requests shall be answered with `200`.

Note that if you export the environmental variable `sslkeyfile` and import it in wireshark (Edit > Preferences > Protocols > TLS > Pre-Master secret log file name), the decrypted traffic is available in wireshark too.





