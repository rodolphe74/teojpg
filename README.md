# teojpg
Tool to convert JPG image to Thomson MO/TO



# jpeg > Thomson MO/TO
Ce programme permet d'afficher des images jpeg après conversion sur les ordinateurs Thomson MO/TO.
Deux formats sont générés :
* Un programme basic lié à la configuration demandée (TO7, TO7-70, TO8, TO9, MO5, MO6) à lancer sur l'ordinateur.
* Un fichier [TO-SNAP](http://collection.thomson.free.fr/code/articles/prehisto_bulletin/page.php?XI=0&XJ=13) exploitable sur l'ordinateur à partir d'un loader basic (généré lui aussi) ou bien sur [Grafx](http://grafx2.chez.com/).

Sur PC, le résultat est affiché dans une fenêtre graphique à l'aide de la librairie [SDL](https://www.libsdl.org/).

Par exemple, pour une configuration TO7 :

![L'original](/samples/KennyMcCormick_small.jpg)

S'affiche sur le PC :

![teojpg](/samples/kenny_res.png)

Et donne sur un TO8 (avec un format TO7 320*200 8 couleurs) :

![dcmoto](/samples/ken_emul.png)

# Le programme
Une version Windows est disponible dans le répertoire release.
Il s'agit d'un programme en ligne de commande prenant une liste de paramètres.


	$ ./teojpg.exe -h
	Usage: teojpg [options] [[--] args]
	   or: teojpg [options]

	A brief description of what the program does and how it works.

		-h, --help                show this help message and exit

	Basic options
		-i, --input=<str>         Fichier image jpg
		-p, --palette=<str>       Fichier palette format hex
		-m, --matrix=<int>        Tramage ordonné Matrice [2,3,4,7,8]
		-f, --floyd=<int>         Tramage Floyd Steinberg [1->10]
		-k, --ostromoukhov        Tramage adaptif ostromoukof
		-g, --gamma=<flt>         Correction gamma [0.1->2.0]
		-b, --brightness=<flt>    Luminosité [-128->+128]
		-t, --contrast=<flt>      Contraste [0.1->2.0]
		-s, --saturation=<flt>    Saturation [0.1->2.0]
		-w, --wu                  Quantification Wu
		-o, --octree              Quantification Octree
		-c, --median              Quantification median-cut
		-z, --machine=<str>       Génération pour thomson [TO7, TO770, MO5, MO6, TO8, TO9]


Une fois le résultat affiché, il est possible de basculer entre le mode PC/Color Clash à l'aide de la barre d'espacement.
Les touches -/+ permettent de commuter la taille des pixels (doubles ou simples).

Pour l'instant, seuls les mode Bitmap 40, 16/8 couleurs 320x200 avec contraintes de proximité (2 couleurs par bloc de 8 pixels) commun à tous les Thomson et le mode 160x200 16 couleurs sans contraintes des Thomson MO6/TO8/TO9 sont gérés.

Lorsqu'une configuration de type MO6, TO8, TO9 est choisie, le programme utilise un algorithme de quantification de palette (par défaut [Wu](https://www.ece.mcmaster.ca/~xwu/cq.c)) pour réduire le nombre de couleurs à 16 parmis les 4096 disponibles sur ces machines. Le résultat affiché sur le PC est en général assez fidèle aux tons de l'image de départ. L'image affichée sur le Thomson va probablement différer de l'image source. Il est en effet difficile de faire une correspondance exacte entre une palette PC et une palette Thomson.

Il est possible de jouer sur le gamma, le contraste, la luminosité et la saturation de l'image source pour rectifier le décalage de palette.

Il est possible de reprendre le fichier TO-SNAP dans Grafx pour modifier la palette.

Je suis à l'écoute de toute proposition permettant de mieux faire correspondre les couleurs PC <> Thomson.

# Les arguments

| Argument       | Description         |
| :------------- | :----------         |
|-i              |Nom de l'image jpg                                   |
|-m              |Taille de la matrice pour le tramage ordonné<br>valeurs possibles: 2, 3, 4, 7, 8<br>Le nombre de tons (et de trames) augmente avec la taille de la matrice.<br>La matrice 7 est une matrice 15x7 spécifique au mode 160x200 (pixels rectangulaires). |
|-f              |Matrice pour le tramage diffusion d'erreurs<br>valeurs possibles: de 1 à 10<br>1-Floyd Steinberg<br>2-Jarvis<br>3-Zhigang<br>4-Shiau<br>5-Shiau 2<br>6-Stucki<br>7-Burkes<br>8-Sierra<br>9-Atkinson<br>10-Knuth|
|-k              |Tramage selon l'algorithme [Ostromoukhov](https://perso.liris.cnrs.fr/victor.ostromoukhov/publications/pdf/SIGGRAPH01_varcoeffED.pdf)           |
|-g              |Modification du gamma<br>valeurs : entre 0.1 et 2.0, 1.0 gamma d'origine           |
|-b              |Modification de la luminosité<br>valeurs : entre -128 et 128, 0 luminosité d'origine       |
|-t              |Modification du contraste<br>valeurs : entre 0.1 et 2.0, 1.0 contraste d'origine           |
|-s              |Modification de la saturation<br>valeurs : entre 0.1 et 2.0, 1.0 saturation d'origine           |
|-w              |Utilisation de la quantification [Xiaolin Wu](https://www.ece.mcmaster.ca/~xwu/cq.c)         |
|-o              |Utilisation de la quantification [Octree](https://fr.wikipedia.org/wiki/Octree)         |
|-c              |Utilisation de la quantification [Median Cut](https://en.wikipedia.org/wiki/Median_cut)         |
|-z              |Choix de la machine Thomson<br>valeurs possibles: TO7, TO770, TO8, TO9, MO5, MO6<br>La valeur choisie affecte le fichier basic résultant ainsi que l'utilisation d'une palette spécifique|

# Prochaines versions
- ~~Essayer d'ajouter la diffusion d'erreur OSTROMOUKHOV~~
- Gérer les autres modes graphiques Thomson TO8/TO9(+)/MO6, ~~320x200 4 couleurs~~ et ~~160x200 16 couleurs~~ sans contraintes.
- ~~Utilisation d'une palette prédéfinie (parce que les algos de quantifications ne sont pas toujours optimaux) au format [lospec.com](https://lospec.com/palette-list) hex~~.
- ~~Makefile Linux/BSD~~
- Nettoyage de code

(~~barré~~ = fait)

# Compilation
Sur windows, un simple ``make -f Makefile.win`` (ou ``mingw32-make.exe``) devrait suffire à partir du moment où [git bash](https://git-scm.com/download/win) et [mingw gcc](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/) sont installés. Les librairies utiles à la compilation sont incluses dans les sous répertoires. Git pour windows inclut une version de bash pour windows ainsi que la plupart des commandes Unix, ce qui est bien pratique pour unifier les commandes de compilation.

Sur Linux, installer au préalable le paquet SDL2 de développement et lancer un ``make -f Makefile.unx``.

# Les liens
- [UToPic](https://github.com/Samuel-DEVULDER/UToPiC)
- [Emulateur DCMOTO et DCFDUTIL](http://dcmoto.free.fr/emulateur/index.html)
- [Les scripts Thomson de Grafx](https://gitlab.com/GrafX2/grafX2/-/tree/master/share/grafx2/scripts/samples/picture/thomson)
- [Le format SNAP-TO](http://collection.thomson.free.fr/code/articles/prehisto_bulletin/page.php?XI=0&XJ=13)
- [libcaca](http://caca.zoy.org/wiki/libcaca/study/introduction)
- [Tramage adaptif Ostromoukhov](https://perso.liris.cnrs.fr/victor.ostromoukhov/publications/pdf/SIGGRAPH01_varcoeffED.pdf)
- [Tramage ordonné Yliluoma](https://bisqwit.iki.fi/story/howto/dither/jy/)
- [Quantification Wu (fast optimal color quantizer)](https://www.ece.mcmaster.ca/~xwu/)
- [Les palette du site lospec.com](https://lospec.com/palette-list)
- [Containers API](https://github.com/bkthomps/Containers)
- [libjpeg](https://sourceforge.net/projects/libjpeg/files/libjpeg/6b/)
- [argparse](https://github.com/cofyc/argparse)
- [SDL2-devel-2.0.14-mingw.tar.gz](https://www.libsdl.org/download-2.0.php)
- [mingw gcc 64 bits pour Windows](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/)


# Quelques exemples

| Mode graphique | Résolution  | Contraintes      |Couleurs      | Tramage           | Quantification/Palette | Résultat       |
| :------------- | :---------  | :----------      | :----------  | :----------    | :----------    | :----------    |
|      BM40      |   320x200   | 2 coul/8 pix   |16/4096       | Ostromoukhov    | Wu             |![](/samples/nimoy_ostro_to9.png)|
|      BM40      |   320x200   | 2 coul/8 pix   |16 (MO5)        | Atkinson        | Octree         |![](/samples/arcadia_atk.png)|
|      BM16      |   160x200   | -                |16/4096    | Régulier 15x7       | Octree         |![](/samples/actarus.png)|
|      BM40      |   320x200   | 2 coul/8 pix   |16/4096       | -               | Wu             |![](/samples/dualail.png)|
|      BM4       |   320x200   | -              |4/4096        | Régulier 8x8    | Palette kadabura4 |![](/samples/bm320.png)|
