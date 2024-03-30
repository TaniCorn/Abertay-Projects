# Networking project, extended upon CMP105. Using Host-Client architecture, lag compensation techniques, and SFML sockets

## Information

[Application EXE](https://drive.google.com/drive/u/0/folders/1hhOkbPP7QjcZ0mjbkYnkBx98RHkN8wJF)

[Demonstration Video](https://youtu.be/PcaBrdJNwsI)

This project was written for my CMP305 module in 3rd year Abertay 2022-2023, where I recieved an A.

What can you expect from this page?

- [Application, report, and code](#information)
- [CMP303-Network systems for game development.pptx](https://github.com/TaniCorn/Abertay-Projects/files/14812244/CMP303-Network.systems.for.game.development.pptx)

What can you expect from the code?

- A solid foundation for a network game to work on
- A single application for both the host and client
- An efficient and simple way of sending, recieving and processing data

What can you expect from the program?

- A networked top-down zombie survival shooter game
- Well functioning game even under loads of Ping – 200ms, Throttle – 30ms, 10%, Out of order – 10%

![04](https://user-images.githubusercontent.com/63819551/236672997-f734d20a-0b7f-479c-80c5-8449d173b01f.png)

## Project supporting document
### Project Design

When I first started this project I had a couple goals:
1. A single application for a networked game
2. Host-Client architecture of up to 4 players
3. A simple messaging system to relay commands
4. Host-Migration and easy joining

Unfortunately I couldn't get the host-migration and easy joining done in time. However I think my plan for them was spot on.

For the Host-Migration, we would give all other players IP to a second in command. This way not everyone needs to know about everyone, only 2 people do. If the host loses connection, the 2IC picks up as the host

For the Easy joining, I planned on having 1 single open port that would redirect incoming connection requests to a different port on the computer. That way there was no need to type in a different port for each player, just pass a single number to all your friends.


# Post

Unfortunately I've not had much of a chance to go back and clean this up, as I've currently doing my final year.

## Retrospect

- This was a really fun project to do, I love thinking about network architectures and it was fun being able to design this.

- I would really love to get some experience in an industry level network project because I'm fascinated in how they handle their networking solutions.




![05](https://user-images.githubusercontent.com/63819551/236673000-2be6f423-316d-485b-b410-b16b1f46e4e6.png)
![02](https://user-images.githubusercontent.com/63819551/236673014-366b24f4-7219-4c94-8e80-f4b7941f31a3.png)
![03](https://user-images.githubusercontent.com/63819551/236673020-d9c292e3-26b3-4457-b50c-431097f9daf8.png)
