[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://github.com/chouilles/buglog/blob/main/LICENSE)


# Buglog

Buglog can be described as a simple semi-distributed chatting service. It is a transparent network which allows people to talk over the internet.


## Authors

- [@chouilles](https://www.github.com/chouilles)


## Features

- Transparent chatting.
- User run.
- Sentralized Database.

## Coming soon

- Direct Messaging.
- Customization.
- Tor compatibility
## Installation

Compile Buglog with gcc 
### Install gcc:
#### Debian/Ubuntu
```bash
sudo apt-get install build-essential
```
#### Arch
```bash
sudo pacman -S gcc
```

### Cloning from github
```bash
  git clone https://github.com/chouilles/buglog
  cd buglog
  ./build
```

## Tech

**Client and server**: C  
**Database**: C, mysql, nginx, curl, json  
**Protocols**: http, tcp

## Usage

**Connnecting**
``` bash
buglog -C <IP> <PORT>
```
**Searching**
```bash
buglog -s
```
*Search not in best conditions yet.* 

**Help**
```bash
buglog -h 
``` 
 
 ### Hosting 
 
 In order to host a chatroom to people outside your **local network**, you will need to portforward on your router. Portforwarding can be done through your routers admin page usually located on `192.168.1.1` WAN settings. [How to port forward](https://www.hellotech.com/guide/for/how-to-port-forward)

 You also would need to configure your *firewall* in order to let traffic through.
When everything is set up, you can start the chatserver using ```buglog -H <Public IP or domain> <PORT>```
## Appendix

Not finished. **Not safe to use :)**


