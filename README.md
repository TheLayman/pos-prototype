# POS-Prototype
```sh
Group 10, CS349-2017.
```

[GitHub]

As the name suggests, this is a prototype of a basic POS terminal aka Point of Sale terminal.

  - Compile server.c
	```sh
	gcc -o server -lm server.c
	```
  - Compile client.c
	```sh
	gcc -o client client.c
	```
  - Arguments for server executable <executable> <port_no>
	```sh
	./server 9999
	```
  - Arguments for client executable <executable> <server_ipv4_address> <port_no>
	```sh
	./client 127.0.0.1 9999
	```
  - Follow the Instruction in the stdout of Client Program to transact. :)

Tips to Hack:

  - "database.txt" contains the products and their respective prices.
  - Follow the Comments and contact us for any doubts or bugs.

# Contributors
- Pavan Karthik Boddeda
- Sujay Lakkimsetti
- George *Veneel* Dogga


#
#
### With Love,

# *TheLaymen*

### Contact US

### Email: [Pavan] or [George]
[George]: <mailto:georgeveneeldogga@gmail.com>
[Pavan]: <mailto:pavankarthikboddeda@gmail.com>
[GitHub]: https://github.com/TheLayman/pos-prototype
