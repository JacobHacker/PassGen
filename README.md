Note:
I created this program to test out Qt functionality (specifically the ui 
builder), and because I wanted to play with random password generation.

Requirements:
Qt 5

Algorithms:
* Random.org:
* Mersenne Twister
* rand()

Random.org:
From their website:
"RANDOM.ORG offers true random numbers to anyone on the Internet. The 
randomness comes from atmospheric noise, which for many purposes is better 
than the pseudo-random number algorithms typically used in computer programs."

The connection is sent over HTTPS, to protect from from Man in the Middle
(MitM) attacks and network sniffers.

