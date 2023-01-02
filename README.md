# Chat Room

Chat room developed in C language using sockets.

![Badge](https://img.shields.io/static/v1?label=license&message=MIT&color=1E90FF)
![Badge](https://img.shields.io/static/v1?label=build&message=passing&color=00d110)

## Content

  - [About](#about)
  - [Support](#support)
  - [Running](#running)
  - [Screenshot](#screenshot)
  - [Author](#author)
  - [License](#license)

## About

The [Chat Room](https://github.com/lucapwn/chat-room) software is a CLI (Command Line Interface) system that allows communication between users through a server.

The system is still under development and other features will be included later.

## Support

The software is currently only available for the GNU/Linux operating system.

In the near future, I will include support for the Windows operating system.

## Running

Compile the software via the Makefile:

~~~console
foo@bar:~$ make run
~~~

Run the server by entering a port:

~~~console
foo@bar:~$ ./server 80
~~~

And create as many clients as you want to talk to:

~~~console
foo@bar:~$ ./client 127.0.0.1 80
~~~

## Screenshot

The image below illustrates the software running and the server communicating with two clients.

![](https://lh3.googleusercontent.com/u/1/drive-viewer/AFDK6gOSMGhAxuyTlltznMSWgAG-woMgk28-0HAxLAVirI_aMMiYCMIolVdbgZznlTpeNQzCsBoz-ECfqYxqSqIIPhftCwIf8g=w1920-h947)

## Author

Developed by [Lucas Araújo](https://github.com/lucapwn).

## License

This software is [MIT](https://choosealicense.com/licenses/mit/) licensed.
