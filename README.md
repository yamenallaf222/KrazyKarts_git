# KrazyKarts_git
Racing Game Over the Internet



The game was about creating components for a networked car racing game and making the network code a removable and replaceable element without any dependency on it in the game's core code. 
In other words, if this element is removed, the game functions as a single-player game without any problems. In this project, a simple physics system for the car was implemented instead of using ready-made code in the engine to have more control over the data transmitted over the network.
The car's movement component was also made a separate component that can be removed and replaced. 
One of the notable achievements in this project is the Unreal Engine Replication System, which is the network system that exchanges data between the server and clients. 
Delayed updates were handled very flexibly, so even if the network delay reached a full second, the perceived image of other players on one player's screen would be very smooth.
The Hermite Cubic Interpolation technique was used to predict the object's movement between server updates as close to the original as possible . Many other issues were addressed, such as the server modifying the player's car position which creates  sense of delay for the player.
Since the server has control over the object's positions in the game and the server itself suffers from delays and infrequent updates, it tries to update the player's position on their screen to a previous point that is outdated to the current location of the player's end.
This issue was addressed by not allowing the server to modify the player's position directly but instead simulating the point where the player arrived at the server to verify the correctness of their steps and to have a timestamp for each step taken by the player,
which is then sent to the server.


the images below demonstrate the infrequent updates on the other players cars as we can see the collision rectangle of the car is ahead of the car actual body as we wait to have two points to interpolate between 

![Screenshot 2023-11-11 011244](https://github.com/yamenallaf222/KrazyKarts_git/assets/128222208/0ccc7f23-2bd4-4ab4-9d3d-ca0c67eb2953)

Here is a youtube video demonstrating the interpolation and replication
https://youtu.be/E1nSRAOYew0?si=3UtdzuiO2e01HbSp

