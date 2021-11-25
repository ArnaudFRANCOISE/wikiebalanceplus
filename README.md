# Wiki de EBalance Plus
## _Description_
Le wiki de EBlancePlus repertorie tous les tutoriels necessaire à la compréhension du projet. De plus il contient la définition de certaines notions techniques

## Installation

- Installer un serveur local comme [wampserver](https://www.wampserver.com/en/download-wampserver-64bits/)
- Cloner ou Télécharger le repo
- Placer le fichier ```mediawiki-1.37.0``` dans ```C:\wamp64\www\```
- Executer **wampserver.exe**
- Dans votre navigateur allez à l'adresse: http://localhost/mediawiki-1.37.0/index.php
- Parcourez le wiki !


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
- Déplacer la $wgSecretKey dans une ENV variable:
    ```$wgSecretKey = getenv("SECRET_KEY");```

- Push le projet sur l'hebergeur


## License

Junia
