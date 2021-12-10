
# Wiki de EBalance Plus
## _Description_
Le wiki de EBlancePlus repertorie tous les tutoriels necessaire à la compréhension du projet. De plus il contient la définition de certaines notions techniques

## Installation  en local

- Installer un serveur local comme [wampserver](https://www.wampserver.com/en/download-wampserver-64bits/)
- Cloner ou Télécharger le repo
- Placer le dossier ```mediawiki-1.37.0``` dans ```C:\wamp64\www\```
- Executer **wampserver.exe**
- Dans votre navigateur allez à l'adresse: http://localhost/phpmyadmin/index.php
- créez une nouvelle table ```my_wiki``` puis importer le fichier ```my_wiki.sql``` du repo (si votre root necessite un mot de passe, vous devrez alors l'indiquer dans le ```LocalSettings.php``` (cf. partie deploiement plus bas))
- Dans votre navigateur allez à l'adresse: http://localhost/mediawiki-1.37.0/index.php
- Parcourez le wiki !


## Utilisation
Le wiki est consultable publiquement, cependant uniquement les éditeurs connecter pourront modifier les pages.
Pour ce faire, il faut vous procurer les identifiants au près de votre manager et cliquer sur connecter. 
![alt text](https://github.com/ArnaudFRANCOISE/StackOverflow_tags_prediction/blob/fea39a8f4c38a7e9539591b9d3c7a66f1c254376/RNN_model/Accuracy.png?raw=true)
 Ceci étant fait vous verrez alors une option modifier qui va apparaître. 
 Pour créer une nouvelle page il suffit tout simplement de la rechercher puis mediawiki vous proposera de l'éditer.

## Deploiement

- cloner le repo 
```git clone https://github.com/YOURUSERNAME/mediawiki```
- Installer [composer](https://getcomposer.org/download/)
- Dans le repertoire local executer : ```composer update```
- Créer une base de donnée et modifier les paramètre ```## Database settings``` dans le ```LocalSettings.php```:
  ```
    $url = parse_url(getenv("CLEARDB_DATABASE_URL"));
    $wgDBtype = "mysql";
    $wgDBserver = $url["host"];
    $wgDBname = substr($url["path"], 1);
    $wgDBuser = $url["user"];
    $wgDBpassword = $url["pass"];
    ```
- Déplacer la $wgSecretKey et la ```$wgUpgradeKey``` dans une ENV variable:
    ```
    $wgSecretKey = getenv("SECRET_KEY");
    $wgUpgradeKey = getenv("UPGRADE_KEY");
     ```

- Push le projet sur l'hebergeur


## License

Junia

