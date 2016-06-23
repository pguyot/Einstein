// ==============================
// Fichier:			TGPSoup.h
// Projet:			K
// 
// Cr�� le:			11/8/2003
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
/// \test	aucun test d�fini.
///
class TGPSoup
{
public:
	enum EStatisticsSelector {
		kBestFitness		= 0x0001,			///< Meilleure ad�quation
		kWorstFitness		= 0x0002,			///< Ad�quation la pire
		kAverageFitness		= 0x0004,			///< Moyenne des ad�quations
		kFitnesses			= kBestFitness | kWorstFitness | kAverageFitness,
												///< Toutes les ad�quations
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
	/// Constructeur � partir d'un algo de cr�ation, d'un langage et d'un
	/// type pour les arbres, ainsi que d'une taille pour la soupe.
	///
	/// \param inRandomGenerator	g�n�rateur al�atoire.
	/// \param inCAlgo				algorithme de cr�ation d'arbres.
	/// \param inLanguage			langage dont les arbres font partie.
	/// \param inSelection			m�thode de s�lection.
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
	/// Initialise la soupe � partir de 0 (cr�e les arbres al�atoirement)
	///
	void Init( void );

	///
	/// Vide la population (d�truit tous les arbres)
	///
	void Clear( void );

	///
	/// Lecture � partir d'un flux.
	/// Ajoute ces noeuds � la population.
	///
	/// \param inStream		d'o� lire la soupe.
	///
	virtual void AddFromStream( std::istream& inStream );

	///
	/// �crit les noeuds dans un flux.
	///
	/// \param outStream	flux o� �crire la soupe.
	///
	virtual void WriteToStream( std::ostream& outStream );

	///
	/// Construit la nouvelle population � partir de la population courante.
	/// Tous les taux sont entre 0 et 1 (ce sont en fait des probabilit�s).
	/// La somme doit �tre inf�rieure � 1, le cas par d�faut est la copie
	/// de l'individu directement.
	/// A priori cette m�thode n'a pas besoin d'�tre surcharg�e sauf pour
	/// introduire un calcul particulier de la g�n�ration suivante.
	///
	/// \param inCrossoverRate				taux de reproduction sexu�e.
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
	/// Le r�sultat peut �tre z�ro, un ou deux arbres fils.
	///
	/// \param inLeftTree	noeud parent gauche
	/// \param inRightTree	noeud parent droit
	/// \param outLeftTree	en sortie, premier fils ou \c nil si le croisement
	///						a �chou�.
	/// \param outRightTree	en sortie, deuxi�me fils ou \c nil.
	///
	virtual void Crossover(
						const TGPTree* inLeftTree,
						const TGPTree* inRightTree,
						TGPTree** outLeftTree,
						TGPTree** outRightTree );

	///
	/// Effectue une mutation d'un individu.
	///
	/// \param inTargetTree		noeud � muter
	/// \param outMutatedTree	noeud mut� (ou \c nil), (copie).
	///
	virtual void Mutate(
						const TGPTree* inTargetTree,
						TGPTree** outMutatedTree );

	///
	/// Effectue une modification d'architecture sur un individu.
	/// Par d�faut ne fait rien.
	///
	/// \param inTargetTree		noeud � modifier
	/// \param outAlteredTree	noeud modifi� (ou \c nil), (copie).
	///
	virtual void AlterArchitecture(
						const TGPTree* inTargetTree,
						TGPTree** outAlteredTree );

	///
	/// Ajoute des noeuds � la population.
	///
	/// \param inImportedTrees	noeuds � ajouter (non copi�s).
	///
	void Add( const std::vector<TGPTree*>& inImportedTrees );

	///
	/// Affiche des statistiques sur les arbres.
	///
	/// \param inOutStream	flux o� �crire les statistiques.
	/// \param inWhichStats	statistiques � afficher
	///
	virtual void PrintStatistics(
						std::ostream& inOutStream,
						EStatisticsSelector inWhichStats ) const;

protected:
	///
	/// Remplit la population pour atteindre une taille donn�e.
	/// Le remplissage est effectu� al�atoirement. Les anciens arbres
	/// sont conserv�s.
	///
	/// \param inPopSize	taille de la population souhait�e.
	///
	virtual void Fill( size_t inPopSize );

	///
	/// Accesseur sur le langage.
	///
	/// \return une r�f�rence sur le langage.
	///
	inline const TGPLanguage&	GetLanguage( void ) const
		{
			return mLanguage;
		}

	///
	/// Accesseur sur l'algorithme de cr�ation d'arbres.
	///
	/// \return une r�f�rence sur l'algorithme de cr�ation d'arbre.
	///
	inline const TGPCreationAlgorithm&	GetCreationAlgorithm( void ) const
		{
			return mCreationAlgo;
		}

	///
	/// Accesseur sur la population.
	///
	/// \return une r�f�rence sur la population.
	///
	inline std::vector<TGPTree*>&		GetPopulation( void )
		{
			return mPopulation;
		}

	///
	/// Accesseur sur la population.
	///
	/// \return une r�f�rence sur la population.
	///
	inline const std::vector<TGPTree*>&	GetPopulation( void ) const
		{
			return mPopulation;
		}

	///
	/// S�lecteur sur la population.
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
	/// R�initialise tous les noeuds avant l'�valuation.
	///
	virtual void ResetNodes( void );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPSoup( const TGPSoup& inCopy );

	///
	/// Op�rateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPSoup& operator = ( const TGPSoup& inCopy );

	///
	/// Cr�ation d'un arbre � partir d'une racine.
	/// Cette m�thode doit �tre surcharg�e par les sous-classes.
	/// Elle est appel�e par \c Fill et \c NextGeneration.
	///
	/// \param inRoot	racine de l'arbre
	/// \return un nouvel arbre avec cette racine.
	///
	virtual TGPTree*			BuildTreeFromRoot( const TGPNodeRef& inRoot ) const = 0;

	/// \name Variables
	TRandomGenerator&			mRandomGenerator;	///< G�n�rateur al�atoire.
	const TGPLanguage&			mLanguage;			///< Langage des arbres.
	const TGPCreationAlgorithm&	mCreationAlgo;		///< Algo de cr�ation d'arbres
	TGPSelection&				mSelection;			///< M�thode de s�lection.
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
