# ÉQUIPE 101 - Instructions et notes Importantes

# INF3995 -Code de base

Code de base utilisé pour démarrer le développement du projet.
Le code contient 04 modules séparés :

- Client : le site Web fait avec le cadriciel(_framework_)`Angular`.
- Serveur : le serveur dynamique bâti avec la librairie `Flask python`.
- Simulation : le code nécessaire au fonctionnement de la simulation avec `Argos` bati essentiellement en **C++**.
- Firmware : le code embarqué sur les drones physiques avec la libraire `Bicraze` bati essentiellement **C**

# Commandes npm

Lancer le client :
1 - Se déplacer dans le dossier client  cd client
2 - lancer Angual avec ng serve

Lancer le serveur :
1 - Se déplacer dans le dossier serveur  cd serveur
2 - Lancer Flask avec  flask run

Lancer la simulation :
1 - Se déplacer dans le dossier serveur  cd simulation
2 - Build l'image docker de la simlation  docker build -t simulation:latest
3 - Run un container avec la nouvelle image 
docker run -it \
    --env="DISPLAY" \
    --env="QT_X11_NO_MITSHM=1" \
    --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
    --publish 8080-8090:8080-8090  
    simulation:latest\ 

Compilation code Firmware : Se déplacer dans le dossier firmware\layer-team101 et exécuter : `make clean`, `make`, `make cload CLOAD_CMDS?="-w radio://0/80/2M/E7E7E7E71x"` avec **1x = 12** pour le drone 1 et **1x = A2** pour le drone 2 dans notre cas.

NOTE :  si une erreur de display s'affiche il suffit d'exécuter sur un terminal xhost +local:root
NOTE2 : pour changer le nombre de drones dans la simulation il faut modifier le ficher . argos à deux endroits **<params nb_drones = ""** et la place habituelle des nombre de drones 


# Commandes Lancement Docker

- Juste pour la simulation avec la commande ci-dessus.

# Standards de Programmation

- Client
  Nous avons utilisé Google typescript guide pour le code Angular,accesible sous ce [lien](https://google.github.io/styleguide/tsguide.html)

- Serveur
  Nous avons utilisé les standards de Programmation Python PEP-8 pour le backend Python, accesible sous ce [lien](https://peps.python.org/pep-0008/)

- FirmWare
  Nous avons utilisé le standard de programmation de Barr pour le code embarqué sur les drones physiques,accesible sous ce [lien](https://barrgroup.com/sites/default/files/barr_c_coding_standard_2018.pdf)

# Conventions de nommage et de langue

- Utilisation du ALL_CAPS pour les constantes.
- Utilisation du PascalCase pour les noms de types et les valeurs d'énumérations.
- Utilisation du camelCase pour les noms de fonctions, de propriétés et de variables.
- Utilisation du kebab-case pour les noms de balises des composants Angular.

# Git && Tests fonctionnels

Tout le code nécessaire à la compilation est présent sur la branche master ainsi que le rapport final.Vous y trouverez également le fichier de test associé à la racine du projet.

# Liens des Vidéos de démo

- Lien vers la vidéo du PDR [ici](https://drive.google.com/file/d/1Jslvjkoa23E0HuGLthGMArHOVoyk3umW/view?usp=sharing)
- Lien vers la video du CDR [ici](https://drive.google.com/drive/folders/1P2zckH_9DbvRNcGSk6RnBsbfW8lnfCUH)
- Lien vers les vidéos du RR [ici](https://drive.google.com/drive/folders/1-4kd3FBsBiAbpvFU6gjufqWbpebm2YGB?usp=share_link)
