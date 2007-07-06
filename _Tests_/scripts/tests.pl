#!/usr/bin/perl -w
# ==============================
# Fichier:			tests.pl
# Projet:			Einstein
# Ecrit par:		Paul Guyot (pguyot@kallisys.net)
# 
# Créé le:			20/6/2003
# Tabulation:		4 espaces
# 
# Copyright:		© 2003 by Paul Guyot.
# 					Tous droits réservés pour tous pays.
# ===========
# $Id: tests.pl 126 2005-02-24 06:55:53Z paul $
# ===========

# Syntaxe:
# perl tests.pl <chemin vers Tests> [<test> | all]

use strict;	# I am a quiche eater.

# --------------------------------------------------------------------------- #
# Globales
# --------------------------------------------------------------------------- #

# Chemin vers le programme Tests.
my $TestsPath;

# Répertoire où se trouve tests.pl
my $ProgramPath;

# Test ou all.
my $Test;

# Argument ou undef.
my $Arg;

# Chemin vers le master ou undef si le test est all.
my $MasterFile;

# --------------------------------------------------------------------------- #
# Usage()
# --------------------------------------------------------------------------- #
sub Usage()
{
	printf STDERR "Syntaxe: perl tests.pl <chemin vers Tests> [<test> [arg] | all]\n";
	exit 2;
}

# --------------------------------------------------------------------------- #
# GetProgramPath()
# --------------------------------------------------------------------------- #
sub GetProgramPath()
{
	$ProgramPath = $0;
	if ($ProgramPath =~ m/^(.*)(\/[^\/]+)$/)
	{
		$ProgramPath = "$1/";
	} else {
		$ProgramPath = "./";
	}
}

# --------------------------------------------------------------------------- #
# CheckAndParseArguments()
# --------------------------------------------------------------------------- #
sub CheckAndParseArguments()
{
	if (($#ARGV != 1) && ($#ARGV != 2))
	{
		printf STDERR "Nombre incorrect d'arguments\n";
		Usage();
	}
	
	# Extraction du chemin vers Tests.
	$TestsPath = $ARGV[0];
	if (! -x "$TestsPath")
	{
		printf STDERR "Tests ($TestsPath) n'est pas exécutable\n";
		Usage();
	}
	
	$Test = $ARGV[1];

	if ($#ARGV == 2)
	{
		$Arg = $ARGV[2];
	}

	# Recherche du master-test-
	if ($Test ne "all")
	{
		$MasterFile = $ProgramPath . "master-test-" . $Test;
		if (defined($Arg))
		{
			$MasterFile .= "_" . $Arg;
		}
		
		if (! -e $MasterFile)
		{
			printf STDERR "Aucun fichier maître pour $Test ne fut trouvé\n";
			Usage();
		}
	}
}

# --------------------------------------------------------------------------- #
# Test( $Test, $Arg $MasterFile )
# --------------------------------------------------------------------------- #
sub Test( $ $ $ )
{
	my $Test = shift;
	my $Arg = shift;
	my $MasterFile = shift;
	
	open MASTER_FILE, "<$MasterFile";
	my @Master = <MASTER_FILE>;
	close MASTER_FILE;
	
	if (defined($Arg))
	{
		open TEST_RESULT, "'$TestsPath' $Test $Arg | grep -v Checksum 2>&1 |" or die "Impossible de lancer le test";
	} else {
		open TEST_RESULT, "'$TestsPath' $Test 2>&1 | grep -v Checksum |" or die "Impossible de lancer le test";
		$Arg = '';
	}
	my @TestResult = <TEST_RESULT>;
	close TEST_RESULT or $TestResult[$#TestResult + 1] = "Erreur lors de la fermeture de Tests.";
	
	# Comparaison.
	my $lineCount = $#TestResult;
	if (($lineCount == 0)
		&& ($TestResult[0] eq "Test non disponible pour cette plateforme\n"))
	{
		print "Test $Test $Arg : NON DISPONIBLE\n";
	} else {
		# Itération sur les deux.
		my $different = 0;
		if ($lineCount != $#Master)
		{
			$different = 1;
			
			if ($lineCount < $#Master)
			{
				$lineCount = $#Master;
			}
		}
		
		for (my $indexLine = 0; $indexLine <= $lineCount; $indexLine++)
		{
			my $TestLine;
			my $MasterLine;

			if ($indexLine <= $#TestResult)
			{
				$TestLine = $TestResult[$indexLine];
			} else {
				$TestLine = '';
			}
			if ($indexLine <= $#Master)
			{
				$MasterLine = $Master[$indexLine];
			} else {
				$MasterLine = '';
			}
			
			chomp $TestLine;
			chomp $MasterLine;
			if ($TestLine ne $MasterLine)
			{
				if (!($TestLine =~ m/^$MasterLine$/))
				{
					$different = 1;
					print "- $MasterLine\n";
					print "+ $TestLine\n";
				}
			}
		}
		
		if ($different)
		{
			print "Test $Test $Arg : ECHEC\n";
		} else {
			print "Test $Test $Arg : SUCCES\n";
		}
	}
}

# --------------------------------------------------------------------------- #
# main
# --------------------------------------------------------------------------- #
GetProgramPath();
CheckAndParseArguments();

# Si le test est all, on itère sur tous les master-test- du dossier.
if ($Test eq "all")
{
	# Itération sur les tous les master-test dans $ProgramPath
	opendir MASTER_DIR, $ProgramPath or die "Impossible d'ouvrir $ProgramPath";
	my @MasterDirFiles = readdir MASTER_DIR;
	closedir MASTER_DIR or die "Erreur à la fermeture de $ProgramPath";
	
	foreach my $MasterDirFile (@MasterDirFiles)
	{
		if ($MasterDirFile =~ m/^master-test-(.*)_(.*)$/)
		{
			Test( $1, $2, $ProgramPath . $MasterDirFile );
		} elsif ($MasterDirFile =~ m/^master-test-(.*)$/) {
			Test( $1, undef, $ProgramPath . $MasterDirFile );
		}
	}
} else {
	Test( $Test, $Arg, $MasterFile );
}

## =================================================================== ##
## The reason computer chips are so small is computers don't eat much. ##
## =================================================================== ##
