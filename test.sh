#!/bin/bash

# Vérification de l'option passée en paramètres lors de l'appel
# Appel adéquat en fonction de l'option 
# Execution de arm-none-eabi-readelf puis de notre programme
# Puis redirection des resultats dans 2 fichiers .txt, en uppercase pour éviter la sensibilité aux majuscules/minuscules
if [[ $1 = '-x' ]]; then
    arm-none-eabi-readelf $1 $2 $3 | tr [:lower:] [:upper:] >file_original.txt
    ./mainPrint $1 $2 $3 | tr [:lower:] [:upper:] >file_handmade.txt
else
    arm-none-eabi-readelf $1 $2 | tr [:lower:] [:upper:] >file_original.txt
    ./mainPrint $1 $2 | tr [:lower:] [:upper:] >file_handmade.txt
fi

# Certains prints en fonction des appels sont indésirables et faussent les résultats, donc on les enlève
if  [[ $1 = '-S' ]]; then
    # La première ligne du resultat de arm-none-eabi-readelf -S parasite les tests
    sed -i '1d' file_original.txt
    #sed -i '2d' file_handmade.txt
elif [[ $1 = '-r' ]]; then
    # Retirer les mots clés ENTRY et ENTRIES car ils faussent le résultat    
    sed -i 's/\<ENTRIES\>//g' file_original.txt
    sed -i 's/\<ENTRY\>//g' file_original.txt
    sed -i 's/\<ENTRIES\>//g' file_handmade.txt
    sed -i 's/\<ENTRY\>//g' file_handmade.txt
fi

FILE1=file_original.txt
FILE2=file_handmade.txt

# On va comparer les resultats obtenus en ignorant les différences de mise en forme (espaces, retours à la ligne)
# Si les résultats sont vides, cela veut dire que la lecture n'a pas été faite
# Les différences, s'il y en a, seront affichées dans un fichier texte log.txt
if diff <( tr -d ' \n' <$FILE1 ) <( tr -d ' \n' <$FILE2) >log.txt; then
    if [ -s $FILE1 ]; then
        echo 'Test validé: le resultat du programme est celui attendu'
    else
        echo "Test invalide: le fichier n'est pas au format elf"
    fi
else
    echo 'Test invalide: le resultat du programme est différent de la commande native'
fi

# note: la mise en forme doit être strictement identique à celle de la commande native, sinon le test n'est pas valide
