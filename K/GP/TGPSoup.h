// ==============================
// Fichier:			TGPSoup.h
// Projet:			K
// 
// Créé le:			11/8/2003
// Tabulation:		4 espaces
// 
// ***** BEGIN LICENSE BLOCK *****
// Version: MPL 1.1
//
// The contents of this file are subject to the Mozilla Public License Version
// 1.1 (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
// http://www.mozilla.org/MPL/
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the
// License.
//
// The Original Code is TGPSoup.h.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2003-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TGPSoup.h,v 1.9 2004/11/24 18:23:16 pguyot Exp $
// ===========

#ifndef _TGPSOUP_H
#define _TGPSOUP_H

#include <K/Defines/KDefinitions.h>

// ISO C++
#include <vector>
#include <iostream>

// K
#include <K/GP/Creation/TGPCreationAlgorithm.h>
#include <K/GP/Data/TGPType.h>
#include <K/GP/Language/TGPLanguage.h>
#include <K/GP/Selection/TGPSelection.h>
#include <K/GP/Tree/TGPTree.h>

///
/// Classe pour une population de programmes.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.9 $
///
/// \test	aucun test défini.
///
class TGPSoup
{
public:
	enum EStatisticsSelector {
		kBestFitness		= 0x0001,			///< Meilleure adéquation
		kWorstFitness		= 0x0002,			///< Adéquation la pire
		kAverageFitness		= 0x0004,			///< Moyenne des adéquations
		kFitnesses			= kBestFitness | kWorstFitness | kAverageFitness,
												///< Toutes les adéquations
		kBestTreeAsDot		= 0x0008,			///< Meilleur arbre pour dot
		kWorstTreeAsDot		= 0x0010,			///< Pire arbre pour dot
		kTreesAsDot			= kBestTreeAsDot | kWorstTreeAsDot,
												///< Les deux arbres pour dot
		kBestTreeAsLisp		= 0x0020,			///< Meilleur arbre en lisp.
		kWorstTreeAsLisp	= 0x0040,			///< Pire arbre en lisp.
		kTreesAsLisp		= kBestTreeAsLisp | kWorstTreeAsLisp
												///< Les deux arbres en lisp
	};
	
	///
	/// Constructeur à partir d'un algo de création, d'un langage et d'un
	/// type pour les arbres, ainsi que d'une taille pour la soupe.
	///
	/// \param inRandomGenerator	générateur aléatoire.
	/// \param inCAlgo				algorithme de création d'arbres.
	/// \param inLanguage			langage dont les arbres font partie.
	/// \param inSelection			méthode de sélection.
	/// \param inTreeType			type des arbres.
	/// \param inSoupSize			taille de la soupe
	///
	TGPSoup(
			TRandomGenerator& inRandomGenerator,
			const TGPCreationAlgorithm& inCAlgo,
			const TGPLanguage& inLanguage,
			TGPSelection& inSelection,
			const TGPType& inTreeType,
			size_t inSoupSize,
			KUInt32 inMaxDepth );
	
	///
	/// Destructeur.
	///
	virtual ~TGPSoup( void );

	///
	/// Initialise la soupe à partir de 0 (crée les arbres aléatoirement)
	///
	void Init( void );

	///
	/// Vide la population (détruit tous les arbres)
	///
	void Clear( void );

	///
	/// Lecture à partir d'un flux.
	/// Ajoute ces noeuds à la population.
	///
	/// \param inStream		d'où lire la soupe.
	///
	virtual void AddFromStream( std::istream& inStream );

	///
	/// Écrit les noeuds dans un flux.
	///
	/// \param outStream	flux où écrire la soupe.
	///
	virtual void WriteToStream( std::ostream& outStream );

	///
	/// Construit la nouvelle population à partir de la population courante.
	/// Tous les taux sont entre 0 et 1 (ce sont en fait des probabilités).
	/// La somme doit être inférieure à 1, le cas par défaut est la copie
	/// de l'individu directement.
	/// A priori cette méthode n'a pas besoin d'être surchargée sauf pour
	/// introduire un calcul particulier de la génération suivante.
	///
	/// \param inCrossoverRate				taux de reproduction sexuée.
	/// \param inMutationRate				taux de mutation.
	/// \param inArchitectureAlteringRate	taux de modification
	///											d'architecture.
	///
	virtual void NextGeneration(
						double inCrossoverRate,
						double inMutationRate,
						double inArchitectureAlteringRate );

	///
	/// Effectue un croisement entre deux individus de la population.
	/// Le résultat peut être zéro, un ou deux arbres fils.
	///
	/// \param inLeftTree	noeud parent gauche
	/// \param inRightTree	noeud parent droit
	/// \param outLeftTree	en sortie, premier fils ou \c nil si le croisement
	///						a échoué.
	/// \param outRightTree	en sortie, deuxième fils ou \c nil.
	///
	virtual void Crossover(
						const TGPTree* inLeftTree,
						const TGPTree* inRightTree,
						TGPTree** outLeftTree,
						TGPTree** outRightTree );

	///
	/// Effectue une mutation d'un individu.
	///
	/// \param inTargetTree		noeud à muter
	/// \param outMutatedTree	noeud muté (ou \c nil), (copie).
	///
	virtual void Mutate(
						const TGPTree* inTargetTree,
						TGPTree** outMutatedTree );

	///
	/// Effectue une modification d'architecture sur un individu.
	/// Par défaut ne fait rien.
	///
	/// \param inTargetTree		noeud à modifier
	/// \param outAlteredTree	noeud modifié (ou \c nil), (copie).
	///
	virtual void AlterArchitecture(
						const TGPTree* inTargetTree,
						TGPTree** outAlteredTree );

	///
	/// Ajoute des noeuds à la population.
	///
	/// \param inImportedTrees	noeuds à ajouter (non copiés).
	///
	void Add( const std::vector<TGPTree*>& inImportedTrees );

	///
	/// Affiche des statistiques sur les arbres.
	///
	/// \param inOutStream	flux où écrire les statistiques.
	/// \param inWhichStats	statistiques à afficher
	///
	virtual void PrintStatistics(
						std::ostream& inOutStream,
						EStatisticsSelector inWhichStats ) const;

protected:
	///
	/// Remplit la population pour atteindre une taille donnée.
	/// Le remplissage est effectué aléatoirement. Les anciens arbres
	/// sont conservés.
	///
	/// \param inPopSize	taille de la population souhaitée.
	///
	virtual void Fill( size_t inPopSize );

	///
	/// Accesseur sur le langage.
	///
	/// \return une référence sur le langage.
	///
	inline const TGPLanguage&	GetLanguage( void ) const
		{
			return mLanguage;
		}

	///
	/// Accesseur sur l'algorithme de création d'arbres.
	///
	/// \return une référence sur l'algorithme de création d'arbre.
	///
	inline const TGPCreationAlgorithm&	GetCreationAlgorithm( void ) const
		{
			return mCreationAlgo;
		}

	///
	/// Accesseur sur la population.
	///
	/// \return une référence sur la population.
	///
	inline std::vector<TGPTree*>&		GetPopulation( void )
		{
			return mPopulation;
		}

	///
	/// Accesseur sur la population.
	///
	/// \return une référence sur la population.
	///
	inline const std::vector<TGPTree*>&	GetPopulation( void ) const
		{
			return mPopulation;
		}

	///
	/// Sélecteur sur la population.
	///
	/// \param inPopulation		nouvelle population.
	///
	inline void	SetPopulation( std::vector<TGPTree*>& inPopulation )
		{
			mPopulation = inPopulation;
		}

	///
	/// Accesseur sur le type des arbres.
	///
	/// \return le type des arbres.
	///
	inline const TGPType&	GetTreeType( void ) const
		{
			return mTreeType;
		}

	///
	/// Réinitialise tous les noeuds avant l'évaluation.
	///
	virtual void ResetNodes( void );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPSoup( const TGPSoup& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPSoup& operator = ( const TGPSoup& inCopy );

	///
	/// Création d'un arbre à partir d'une racine.
	/// Cette méthode doit être surchargée par les sous-classes.
	/// Elle est appelée par \c Fill et \c NextGeneration.
	///
	/// \param inRoot	racine de l'arbre
	/// \return un nouvel arbre avec cette racine.
	///
	virtual TGPTree*			BuildTreeFromRoot( const TGPNodeRef& inRoot ) const = 0;

	/// \name Variables
	TRandomGenerator&			mRandomGenerator;	///< Générateur aléatoire.
	const TGPLanguage&			mLanguage;			///< Langage des arbres.
	const TGPCreationAlgorithm&	mCreationAlgo;		///< Algo de création d'arbres
	TGPSelection&				mSelection;			///< Méthode de sélection.
	std::vector<TGPTree*>		mPopulation;		///< Population.
	const TGPType&				mTreeType;			///< Type des arbres.
	size_t						mSoupSize;			///< Taille de la soupe.
	KUInt32						mMaxDepth;			///< Profondeur maximale des arbres.
};

#endif
		// _TGPSOUP_H

// ====================================================================== //
// Everybody needs a little love sometime; stop hacking and fall in love! //
// ====================================================================== //
