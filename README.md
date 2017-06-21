git pull origin master  //Pull les données du serveur gitlab

git checkout master //Aller sur le master pour préparer la création d'une autre branche

git checkout -b "feature-name"  //Création d'une branche en local

... do some work ...

git add .   //Ajouter les fichier et dossiers à sauvegarder

git commit -m "+-*! a good message explaining the content of the commit"    //Commit en local, penser a faire des petits commits

git checkout master

git pull origin master

git checkout feature-name

git merge master

git push origin feature-name    //On push la branche sur le serveur gitlab

Créer une pull request sur gitlab

Se faire approuver la branche par les autres développeurs et modifier le code en fonction des commentaires

Merger la branche sur gitlab 