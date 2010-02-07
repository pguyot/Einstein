// ==============================
// Fichier:			TGPLanguage.h
// Projet:			K
//
// Créé le:			11/2/2003
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
// The Original Code is TGPLanguage.h.
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
// $Id: TGPLanguage.h,v 1.7 2005/02/14 20:55:03 pguyot Exp $
// ===========

#ifndef _TGPLANGUAGE_H
#define _TGPLANGUAGE_H

#include <K/Defines/KDefinitions.h>

// ISO C++
#include <map>
#include <vector>
#include <string>

// ANSI C
#include <stddef.h>

// K
#include <K/GP/Data/TGPType.h>
#include <K/GP/Tree/Nodes/TGPNodeRef.h>

// Pré-déclarations
class TGPNode;
class TGPSoup;
class TGPTree;
class TRandomGenerator;

///
/// Classe pour un langage, i.e. un ensemble de noeuds.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TGPLanguage
{
public:
	///
	/// Constructeur par défaut.
	///
	TGPLanguage( void );

	///
	/// Destructeur.
	/// Supprime tous les noeuds du langage.
	///
	virtual ~TGPLanguage( void );
	
	///
	/// Sous-ensembles de noeuds (tous, terminaux, non terminaux)
	///
	enum ENodeSet {
		kAll,		///< Tous les symboles
		kNodes,		///< Symboles non terminaux.
		kLeaves		///< Feuilles.
	};

	///
	/// Récupère un noeud choisi aléatoirement d'un type donné.
	///
	/// \param	inRandomGenerator	générateur aléatoire pour choisir le noeud.
	/// \param	inType				type du noeud à choisir.
	/// \param	inNodeSet			ensemble où choisir le noeud.
	/// \return le noeud choisi (c'est un clone du prototype du langage)
	/// \throws runtime_error s'il n'y a pas de noeud de ce type.
	///
	TGPNode*	GetRandomNode(
						TRandomGenerator& inRandomGenerator,
						const TGPType& inType,
						ENodeSet inNodeSet = kAll ) const;

	///
	/// Détermine le nombre de noeuds d'un type donné.
	///
	/// \param	inType		type des noeuds à compter
	/// \param	inNodeSet	ensemble où compter les noeuds.
	/// \return le nombre de noeuds de ce type dans cet ensemble.
	///
	size_t		CountNodes(
						const TGPType& inType,
						ENodeSet inNodeSet = kAll ) const;

	///
	/// Méthode récursive pour la conversion d'un arbre depuis le Lisp.
	/// Cette méthode convertit tout entre la parenthèse ouvrante (le premier
	/// caractère du flux) jusqu'à la parenthèse fermante associée.
	/// Elle utilise la méthode GetLispName des noeuds. Les noeuds de
	/// l'arbre doivent déjà être présents dans le langage.
	///
	/// \param	inStream	flux d'entrée.
	/// \throws runtime_error si une erreur de syntaxe est survenue.
	/// \return un sous-arbre correspondant à ce qui a été lu sur le flux.
	///
	TGPNodeRef	BuildNodeFromStreamAsLisp( std::istream& inStream ) const;

	///
	/// Ajoute une constante dans le langage.
	/// La constante est transformée en noeud (mais l'ensemble des constantes
	/// est aussi conservé).
	///
	/// \param inValue	valeur de la constante à ajouter.
	/// \return \c true si la constante fut ajoutée, \c false si elle était déjà là.
	///
	Boolean		AddConstantValue( TGPDataRef& inValue );

	///
	/// Ajoute une constante dans le langage.
	/// La constante est transformée en noeud (mais l'ensemble des constantes
	/// est aussi conservé).
	///
	/// \param inType	type de la constante.
	/// \param inValue	valeur de la constante à ajouter.
	/// \return \c true si la constante fut ajoutée, \c false si elle était déjà là.
	///
	Boolean		AddConstantValue( const TGPType& inType, TGPDataRef& inValue );

	///
	/// Accesseur sur les constantes d'un type donné.
	///
	/// \param inType	type considéré.
	/// \return une référence sur les constantes du type \c inType.
	///
	inline const std::vector<TGPDataRef>&	GetConstants( const TGPType& inType ) const
		{
			return (((TGPLanguage*) this)->mConstants[ &inType ]);
		}

	///
	/// Ajoute un noeud (ou une feuille) dans la liste
	///
	/// \param inNode	noeud à ajouter. N'est pas copié et sera
	///					libéré par le destructeur.
	/// \return \c true si le noeud fut ajouté, \c false s'il était déjà là.
	///
	Boolean		AddNode( TGPNode* inNode );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPLanguage( const TGPLanguage& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPLanguage& operator = ( const TGPLanguage& inCopy );

	///
	/// Compte les noeuds d'un type donné.
	///
	/// \param inVector		vecteur où compter les noeuds.
	/// \param inType		type des noeuds à compter.
	/// \return le nombre de noeuds de ce type.
	///
	static size_t CountNodesOfType(
						const std::vector<TGPNode*> inVector,
						const TGPType& inType );

	///
	/// Récupère le i-ème noeud d'un type donné.
	///
	/// \param inVector		vecteur avec les noeuds.
	/// \param inType		type du noeud cherché
	/// \param inNodeIndex	indice du noeud à récupérer.
	/// \return le i-ème noeud du type \c inType ou \c nil s'il n'y en
	///			a pas
	///
	static TGPNode* GetIthNodeOfType(
						const std::vector<TGPNode*> inVector,
						const TGPType& inType,
						size_t inNodeIndex );

	///
	/// Récupère le noeud d'un nom donné.
	///
	/// \param inVector		vecteur avec les noeuds.
	/// \param inName		nom du noeud cherché
	/// \return le noeud du nom donné.
	///
	static TGPNode* GetNodeFromName(
						const std::vector<TGPNode*> inVector,
						const std::string& inName );

	///
	/// Récupère le noeud d'un nom donné.
	///
	/// \param inVector		vecteur avec les noeuds.
	/// \param inName		nom du noeud cherché
	/// \return le noeud du nom donné.
	///
	static TGPNode* GetNodeFromName(
						const std::vector<TGPNode*> inVector,
						TStringRef inName );

	///
	/// Avance le flux jusqu'au premier caractère non blanc.
	///
	/// \param inStream		flux considéré
	/// \return le premier caractère non blanc (toujours dans le flux)
	///
	static char	SkipBlanks( std::istream& inStream );

	///
	/// Récupère le prochain symbole sans blanc ou parenthèse.
	///
	/// \param inStream		flux considéré
	/// \return le prochain symbole sans blanc ou parenthèse (éventuellement
	///			la chaîne vite)
	///
	static std::string	NextToken( std::istream& inStream );

	/// \name Variables
	std::vector<TGPNode*>					mNodes;		///< Liste des noeuds.
	std::vector<TGPNode*>					mLeaves;	///< Liste des feuilles.
	std::map< const TGPType*, std::vector< TGPDataRef > >
											mConstants;	///< Liste des constantes.
};

#endif
		// _TGPLANGUAGE_H

// =============================================================================== //
// If you ever want to have a lot of fun, I recommend that you go off and program  //
// an imbedded system.  The salient characteristic of an imbedded system is that   //
// it cannot be allowed to get into a state from which only direct intervention    //
// will suffice to remove it.  An imbedded system can't permanently trust anything //
// it hears from the outside world.  It must sniff around, adapt, consider, sniff  //
// around, and adapt again.  I'm not talking about ordinary modular programming    //
// carefulness here.  No.  Programming an imbedded system calls for undiluted      //
// raging maniacal paranoia.  For example, our ethernet front ends need to know    //
// what network number they are on so that they can address and route PUPs         //
// properly.  How do you find out what your network number is?  Easy, you ask a    //
// gateway.  Gateways are required by definition to know their correct network     //
// numbers.  Once you've got your network number, you start using it and before    //
// you can blink you've got it wired into fifteen different sockets spread all     //
// over creation.  Now what happens when the panic-stricken operator realizes he   //
// was running the wrong version of the gateway which was giving out the wrong     //
// network number?  Never supposed to happen.  Tough.  Supposing that your         //
// software discovers that the gateway is now giving out a different network       //
// number than before, what's it supposed to do about it?  This is not discussed   //
// in the protocol document.  Never supposed to happen.  Tough.  I think you       //
// get my drift.                                                                   //
// =============================================================================== //
