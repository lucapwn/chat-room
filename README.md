# Chat Room

The [Chat Room](https://github.com/lucapwn/chat-room) is a software developed in C language using sockets to create a chat room between a server and clients.

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

In the near future I will include support for the Windows operating system and macOS.

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

![](https://user-images.githubusercontent.com/58787069/216812354-3f4aa4f9-e8e6-4f15-9310-9c9b72f64c4a.PNG)

## Author

Developed by [Lucas Araújo](https://github.com/lucapwn).

## License

This software is [MIT](https://choosealicense.com/licenses/mit/) licensed.
