# teojpg
Tool to convert JPG image to Thomson MO/TO



# jpeg > Thomson MO/TO
Ce programme permet d'afficher des images jpeg après conversion sur les ordinateurs Thomson MO/TO.
Deux formats sont générés :
* Un programme basic lié à la configuration demandée (TO7, TO7-70, TO8, TO9, MO5, MO6) à lancer sur l'ordinateur.
* Un fichier [TO-SNAP](http://collection.thomson.free.fr/code/articles/prehisto_bulletin/page.php?XI=0&XJ=13) exploitable sur l'ordinateur à partir d'un loader basic (généré lui aussi) ou bien sur [Grafx](http://grafx2.chez.com/).

Le résultat est affiché dans une fenêtre graphique à l'aide de la librairie [SDL](https://www.libsdl.org/).

Par exemple, pour une configuration TO7 :

![L'original](/samples/KennyMcCormick_small.jpg)

S'affiche sur le PC :

![teojpg](/samples/kenny_res.png)

Et donne sur un TO7 :

![dcmoto](/samples/ken_emul.png)

# Le programme
Une version windows est disponible dans le répertoire release.
Il s'agit d'un programme en ligne de commande prenant une liste de paramètres.


	$ ./teojpg.exe -h
	Usage: teojpg [options] [[--] args]
	   or: teojpg [options]

	A brief description of what the program does and how it works.

		-h, --help                show this help message and exit

	Basic options
		-i, --input=<str>         Nom de l'image
		-m, --matrix=<int>        Tramage ordonné Matrice [2,3,4,8]
		-f, --floyd=<int>         Tramage Floyd Steinberg [1->10]
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

Pour l'instant, seul le mode Bitmap 40, 16/8 couleurs 320x200 commun à tout les Thomsons est géré.




