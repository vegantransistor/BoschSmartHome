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

Connect the Smart Home Controller to your PC with an Ethernet Cable. Connect the serial interface to your PC and start the SSH daemon on the Smarthome Controller:

```
systemctl start ssh
```
Then SSH the device. First find out the IP address of the device with `ip addr`:

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
Then connect:
```
ssh root@192.168.2.58
```

## 


