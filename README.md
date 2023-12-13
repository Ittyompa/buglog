[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://github.com/chouilles/buglog/blob/main/LICENSE)


# Buglog

Buglog can be described as a simple distributed chatting service. It is a transparent network which allows Linux users to talk over the internet only using the terminal.


## Authors

- [@chekuga](https://www.github.com/chekuga)


## Features

- Transparent chatting.
- User run.
- Sentralized Database.

## To be added-features

- Direct Messaging.
- Customization with .conf file.
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
  git clone https://github.com/chekuga/buglog
  cd buglog
  ./build
```

## Tech

**Client and server language**: C  
**Protocols**: tcp

## Usage

**Connnecting**
``` bash
buglog -C -ip <IP> -p <PORT>
```

**DM**
``` bash
/dm <nickname> <message>
```

**Help**
```bash
buglog -h 
``` 
 
 ### Hosting 
 
 In order to host a chatroom to people outside your **local network**, you will need to portforward on your router. Portforwarding can be done through your routers admin page usually located in WAN settings on `192.168.1.1`. [How to port forward](https://www.hellotech.com/guide/for/how-to-port-forward)

 You also would need to configure your *firewall* in order to let traffic through.
When everything is set up, you can start the chatserver using ```buglog -H -ip <IP> -p <PORT>```
## Appendix

Not finished. **Not safe to use :)** Code is not optimized

