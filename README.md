[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/LZABp_05)

# Projet Compresseur Quadtree

## Groupe 4

### Authors

- [MUNAITPASOV Maksat](https://github.com/munmaks)
- [BENVENISTE Alexandre ](https://github.com/SuRiz4)

## Aperçu

Ce projet implémente un compresseur d'image basé sur un quadtree en C.  
Il compresse les images de format `pgm` en format `qtc`.  
Il peut aussi décompressé les images au format `qtc` en format `pgm`.  
Le niveau de compression peut-être ajuster à l'aide des différentes commandes disponibles.  
Si le filtrage est utilisé, alors l'image compressée perdra en qualité et sera irréversible lors de la décompression mais permet de réduire de manière significative la taille de l'image compressée.

## Utilisation

### Prérequis

- Assurez-vous d'avoir un compilateur C installé (par exemple, `gcc`).
- Assurez-vous d'avoir `make` installé.

### Construire le Projet

- Construisez le bibliothèque en utilisant `make lib/libqtc.so` :

```sh
make lib/libqtc.so
```

- Construisez le projet en utilisant `make` :

```sh
# avec bibliothèque qtc et main de l'utilisateur
make
```

- Construisez le projet en utilisant `make ansi` pour utilisé la norme ansi:

```sh
make ansi
```

### Exécuter le Projet

- Après avoir construit le projet, vous pouvez utilisé l'exécutable :
  ```sh
  ./bin/codec [OPTIONS...] {-c | -u} -i input.{pgm | qtc}
  ```
  - `input.{pgm | qtc}` : Fichier d'entré
  - `-c` : Compresser une image au format "pgm"
  - `-u` : Decompresser une image au format "qtc"
  - `-i` : Choisir le fichier d'entré à utiliser

Par defaut l'image compresser se trouve dans le dossier `QTC` avec le nom `out.qtc`  
Même chose pour l'image décompresser dans le dossier `PGM` avec le nom `out.pgm`

### Exemple

- Pour compresser une image `pgm` :

```sh
./bin/codec -c -i fichier_a_compresser.pgm
```

- Pour décompresser une image `qtc` :

```sh
./bin/codec -u -i fichier_a_decompresser.qtc
```

### Options

- `-o` : Choisir le nom du fichier de sorti, par défaut `out.{pgm | qtc}`:

```sh
./bin/codec -c -i fichier_a_compresser.pgm -o ficher_compresser.qtc

./bin/codec -u -i fichier_a_decrompresser.qtc -o ficher_decompresser.pgm
```

- `-v` : Active le mode verbose :

```sh
./bin/codec -c -i fichier_a_compresser.pgm -v
```

- `-a` : Applique un filtrage quadtree avec un facteur alpha spécifié, permet de choisir l'intensité de la compression.

  - α ≤1.0 → : Aucun ﬁltrage, donc aucun gain supplémentaire
  - α ≃1.5 → : Résultats intéressants, ﬁltrage moyen, gain correct
  - α ≥2.0 → : Fltrage excessif, image très abimée

```sh
./bin/codec -c -i fichier_a_compresser.pgm -a 1.4
```

- `-g` : Affiche la grille de segmentation en créeant une image `g_out.pgm`.  
   Peut-être utilisé lors de la compression et de la décompression

```sh
./bin/codec -c -i fichier_a_compresser.pgm -g
```

- `-h` : Affiche le message d'aide.

```sh
./bin/codec -h -v
Usage: ./bin/codec [OPTIONS...]
`Encode / Decode` PGM (Portable Gray Map) pictures depending on options used.

First of all you need to choose between `encodeur` or `decodeur`.
Than program needs correct file accoring to chosen mode.
        -h,     display help message usage and exit
        -v,     verbose mode
        -c,     chosen mode is `encodeur` expects `.pgm` file
        -u,     chosen mode is `decodeur` expects `.qtc` file
        -g,     segmentation grid
        -i,     input.{pgm | qtc}, input file depending from chosed mode
        -o,     output.{pgm | qtc}, output file depending from chosed mode
        -a,     `double` in [0.0, 2.0], filtering rate for `encodeur`
```

### Nettoyage

Pour nettoyer les fichiers de construction, exécutez :

```sh
make clean
```

### Répartition du travail

- Maksat :

  - Conversion du quadtree en fichier et inversement
  - Filtrage pour la compression
  - Options pour l'executable
  - doxyfile + les commentaires utiles

- Alexandre :

  - Conversion du fichier PGM en pixmap et inversement
  - Mode segmentation
  - Rapport du projet
