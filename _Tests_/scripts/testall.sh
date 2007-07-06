#!/bin/bash
# ==============================
# Fichier:			testall.sh
# Projet:			Einstein
# Ecrit par:		Paul Guyot (pguyot@kallisys.net)
#
# Créé le:			2/5/2003
# Tabulation:		4 espaces
#
# Copyright:		© 2003 by Paul Guyot.
# 					Tous droits réservés pour tous pays.
# ===========
# $Id: testall.sh 14 2004-07-27 11:52:09Z paul $
# ===========

# Script pour appeler tous les tests non manuels.
for test in test-*.sh
do
	sh $test $*
done

## ================================================== ##
## Row, row, row your bits, gently down the stream... ##
## ================================================== ##
