#!/bin/bash
# ==============================
# Fichier:			common.sh
# Projet:			Einstein
# Ecrit par:		Paul Guyot (pguyot@kallisys.net)
#
# Créé le:			2/5/2003
# Tabulation:		4 espaces
#
# Copyright:		© 2003 by Paul Guyot.
# 					Tous droits réservés pour tous pays.
# ===========
# $Id: common.sh 14 2004-07-27 11:52:09Z paul $
# ===========

if [ $# -ne 1 ]; then
	echo "Nombre invalide d'arguments."
	echo "Syntaxe: $0 <chemin vers Tests>"
	exit 1
fi

LC_CTYPE=
LANG=

# Détermination de la racine de Einstein à partir de PWD.
# Comme ce script est appelé par . common.sh dans les différents scripts des
# tests, on peut supposer que PWD est le dossier des scripts.
EINSTEIN="$PWD/../../"

# Exécutable pour faire les tests.
TESTSPATH="$1"

# Répertoire pour les fichiers temporaires.
TMPDIR=/tmp

# Fichier maître
MASTERFILE=$TMPDIR/einstein-tests-$$-master

# Fichier de sortie
OUTFILE=$TMPDIR/einstein-tests-$$-out

# Test désactivé.
NOTAVAILFILE=$TMPDIR/einstein-tests-$$-notavail

# Nettoyage des fichiers précédents (pour les échecs précédents).
rm -f $TMPDIR/einstein-tests-*

cat > $NOTAVAILFILE << ENDNOTAVAIL
Test non disponible pour cette plateforme
ENDNOTAVAIL

# Méthode pour comparer.
compare() {
	if [ `diff -u $NOTAVAILFILE $OUTFILE | wc -l` -ne "0" ]
	then
		if [ `diff -u $MASTERFILE $OUTFILE | tee /dev/stderr | wc -l` -ne "0" ]
		then
			echo "$0 : ECHEC"
			exit 1
		else
			echo "$0 : SUCCES"
		fi
	else
		echo "$0 : NON DISPONIBLE"
	fi
}

## ====================================================================== ##
## Prof:    So the American government went to IBM to come up with a data ##
##          encryption standard and they came up with ...                 ##
## Student: EBCDIC!"                                                      ##
## ====================================================================== ##
