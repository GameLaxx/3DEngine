#!/bin/bash

# Parcourt tous les dossiers à partir du répertoire courant
find . -name "*.o" -exec rm -rf {} +

echo "Tous les dossiers .o ont été supprimés."
