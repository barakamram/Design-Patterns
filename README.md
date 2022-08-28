# Authors:

Barak Amram

Liroy Melamed

## How to run:

### Write the command "make all"

* Questions1-3 (queue.h, active_object.h, active_object.c, main1.c, server.h, server.c, client.c): 
  * run ./server 
  * for each client run ./client localhost (./client 127.0.0.1)

* Question 4 (guard.cpp): 
  * run ./guard

* Question 5 (singleton.cpp): 
  * run ./singleton

* Question 6 (reactor.h, pollserver.c, selectclient.c): 
  * run ./pollserver 
  * for each client run ./selectclient localhost (./selectclient 127.0.0.1)

### You can compile separately for each question:
* Questions 1-3: write "make server" and "make client".
* Question 4: write "make guard".
* Question 5: write "make singleton".
* Questions 6: write "make pollserver" and "make selectclient".

