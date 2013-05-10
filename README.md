Note:
I created this program to test out Qt functionality (specifically the ui 
builder), and because I wanted to play with random password generation (and 
randomness in general).

Requirements:
Qt 4.7
Boost 1.46

Recommendations:
QtCreator (coders)
QtDesigner (ui designers)
QtLinguist (translators)

Algorithms:

* Random.org:
* Mersenne Twister
* rand()

Random.org:
From their website:
"RANDOM.ORG offers true random numbers to anyone on the Internet. The 
randomness comes from atmospheric noise, which for many purposes is better 
than the pseudo-random number algorithms typically used in computer programs."

The connection is sent over HTTPS, so we are protected from Man in the Middle
(MitM) attacks and network sniffers.

TODO:
* While the program is fairly solid (at around 200 LoC how could it not be?)
there needs to be more error checking, specifically during the HTTPS requests.

* Test portability, this _should_ work on nearly every OS out there, since
it's using Qt and Boost as it's Framework, but it would be interesting if it
could be ported to other devices, such as the iPhone, Android, ...