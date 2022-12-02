<img src="https://user-images.githubusercontent.com/11777512/193417321-0d75b568-dd6f-413f-b07a-3bc9015a0106.png" width="350">
<img src="https://user-images.githubusercontent.com/11777512/204952506-e58b8ea8-5316-48fa-89ae-6744a0cc6eb4.png" width="350">

# DIY Drone Controller (Transmitter + Receiver)
The goal of this project is to create my very own drone RC Transmitter that is compatible with as many different flight controllers as possible. I have always wanted to build my own drone -- and for this project I sort of had to -- but DIY drones are all over the place and they pretty much come in a kit anymore, which takes away the challenge. On the other hand, DIY drone controllers, while they exist, are much less common. Plus they offer way more opportunities for customization.

So, for this project (at least mk. 1) I built a custom drone transmitter with an acrylic faceplate, some sweet leds, a power button, and dual joysticks with trim potentiometers. 

Check out the project on [PCBWay](https://www.pcbway.com/project/shareproject/DIY_Drone_Transmitter_6b195ee1.html)!

# RC Protocols
The hardest part of this project so far has been figuring out the best way to communicate from the RC Receiver to the flight controller.

Drones use two different communication protocols to operate, which are outlined very well by Oscar Liang [here](https://oscarliang.com/rc-protocols/). 

The way he puts it, the transmitter, or controller, sends a signal to the receiver mounted on the drone -- TX Protocols. Next, the receiver decodes that signal from the transmitter and sends it to the flight controller -- RX Protocols. In this project the TX protocol is easy, I'm using two Arduino Nano's to communicate via RF using NRF24L01 modules. Since I'm using Arduino Nano's and coding them myself, I make the TX protocol be whatever I want. 

Now, I'm not an expert at RC protocols in any way, so the version of "TX Protocol" that I've implemented for this project is far from perfect, but it works...

The hard part is getting the receiving Arduino to successfully communicate with the drone. As outlined by Oscar Liang, there are several RX protocols to choose from, but the Arduino Nano isn't capable of doing them all. Currently, I am using PPM to communicate over a single wire to my flight controller, but in the future I hope to convert the RX Protocol to SBUS over UART. PPM works well but not all flight controllers support PPM, as I found when trying to locate one myself... On the other hand, most flight controllers have multiple UART channels that support SBUS. 

# Thoughts On MultiWii
When I mentioned earlier that I've always wanted to build my own drone, well a few years back I did. Sadly, that drone never flew, but I'm convinced it would have! That drone was made literally from scratch using some perfboard, an Arduino Nano, a barometer chip, and gyro chip, and some good 'ol fashioned wires. Unfortunately, since it was running on an Arduino Nano, the flight control firmware of choice at the time was MultiWii. Not to say that anything is wrong with MultiWii, it's actually an amazing project. However, when I revisited that project, I was unable to the MultiWii configuration software up an running on my laptop, so I had to go another route. In the future, I would love to say I built my flight controller from scratch, but in the meantime, I'll have to make due with an off the shelf flight controller 😢

# Building One Yourself!
Full instructions can be found here: [https://www.modhobbyist.com/projects/26](https://www.modhobbyist.com/projects/26)

I'm terrible with documentation, so if I haven't completed all this yet, please ping me on the Discord server!
[https://discord.gg/R3tyHfKwaw](https://discord.gg/R3tyHfKwaw)
