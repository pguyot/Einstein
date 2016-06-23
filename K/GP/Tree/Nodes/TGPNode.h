// ==============================
// Fichier:			TGPNode.h
// Projet:			K
//
// Cr�� le:			16/1/2003
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
// The Original Code is TGPNode.h.
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

#ifndef _TGPNODE_H
#define _TGPNODE_H

#include <K/Defines/KDefinitions.h>

// ISO C++
#include <vector>
#include <functional>

// K
#include <K/GP/Data/TGPDataRef.h>
#include <K/GP/Data/TGPType.h>
#include <K/Unicode/TStringRef.h>

// Pr�-d�clarations
class TGPEvaluationContext;
class TGPNodeRef;

///
/// Classe pour un noeud dans un programme de programmation g�n�tique.
/// Un noeud peut-�tre une primitive ou une feuille
/// (suivant l'arit�)
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
///
class TGPNode
{
public:
	///
	/// Destructeur.
	///
	virtual ~TGPNode( void );

	///
	/// Accesseur sur l'arit�.
	///
	inline KUInt32			GetArity( void ) const
		{
			return mArity;
		}
	
	///
	/// R�cup�ration du type de ce noeud.
	///
	/// \return le type de ce noeud.
	///
	virtual const TGPType&	GetType( void ) const = 0;

	///
	/// R�cup�ration du type d'un param�tre.
	/// Retourne le type intrins�que, i.e. ne demande pas au
	/// sous-noeud.
	///
	/// \param	inParamIndex		indice du param�tre dont on veut le type.
	/// \throws range_error s'il n'y pas un tel noeud.
	///
	virtual const TGPType&	GetParamType( KUInt32 inParamIndex ) const = 0;

	///
	/// R�cup�re le nom du noeud.
	/// Ce nom doit �tre unique et permettre de recr�er le noeud (sans
	/// les sous-noeuds).
	///
	/// \return	une r�f�rence sur la cha�ne qui identifie le noeud.
	///
	virtual TStringRef		GetLispName( void ) const = 0;

protected:
	///
	/// Constructeur par copie.
	/// Vide la liste des sous-noeuds.
	/// Ce constructeur doit �tre appel� via le constructeur par
	/// copie des classes concr�tes.
	///
	/// \param inCopy		objet � copier
	///
	TGPNode( const TGPNode& inCopy );

	///
	/// Constructeur � partir de l'arit�.
	///
	/// \param	inArity		arit� du noeud.
	///
	TGPNode( KUInt32 inArity );

private:
	friend class TGPNodeRef;

	///
	/// Op�rateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPNode& operator = ( const TGPNode& inCopy );

	///
	/// D�termine si ce noeud est constant (au sens o� si les sous-noeuds
	/// sont constants, son �valuation retournera le m�me r�sultat et le contexte
	/// ne sera pas modifi�).
	///
	/// Par d�faut r�pond \c true.
	/// M�thode appel�e par Invalidate si les sous-noeuds
	/// sont constants.
	///
	/// \return \c true si ce noeud est constant, \c false sinon.
	///
	virtual Boolean			IsConst( void ) const;
	
	///
	/// �valuation.
	///
	/// Cette m�thode est appel�e plusieurs fois par l'interpr�teur.
	/// Elle retourne un sous-noeud � �valuer pour l'interpr�teur, afin de
	/// calculer son propre r�sultat, ou \c nil lorsque l'�valuation est termin�e.
	/// Dans ce cas, le r�sultat doit �tre renseign� dans \c ioResult.
	///
	/// \param inNodeRef	r�f�rence sur le noeud �valu�.
	/// \param ioResult		en entr�e, r�sultat de l'�valuation du sous-noeud
	///						demand�.
	///						en sortie, r�sultat de l'�valuation de ce noeud.
	/// \param ioStorage	entrep�t pour des donn�es (laiss� � la discr�tion
	///						du noeud)
	/// \param inContext	contexte pour l'�valuation.
	/// \param ioCookie		biscuit (mis � z�ro par l'interpr�teur au d�but de
	///						l'�valuation de ce noeud).	
	/// \return un sous-noeud dont la m�thode veut le r�sultat, \c nil
	///			lorsque l'�valuation est termin�e.
	///
	virtual TGPNodeRef*	Evaluate(
							TGPNodeRef* inNodeRef,
							TGPDataRef* ioResult,
							std::vector<TGPDataRef>& ioStorage,
							TGPEvaluationContext* inContext,
							KUIntPtr* ioCookie ) = 0;

	/// \name Variables	
	KUInt32				mArity;			///< Arit� du noeud, i.e. nombre de
										///< sous-noeuds.	
};

#ifdef __STL_BEGIN_NAMESPACE
__STL_BEGIN_NAMESPACE
#else
namespace std
{
#endif

///
/// Comparateur pour la classe TGPNode.
/// Utilise GetLispName et ignore les sous-noeuds.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
///
template<>
struct less<TGPNode*>
{
	///
	/// Comparaison des noeuds.
	///
	/// \param x	premier noeud � comparer
	/// \param y	deuxi�me noeud � comparer
	/// \return \c true si x < y
	///
	bool operator()(const TGPNode* x, const TGPNode* y);
};

#ifdef __STL_END_NAMESPACE
__STL_END_NAMESPACE
#else
}
#endif

#endif
		// _TGPNODE_H

// ====================================== //
// Frankly, Scarlett, I don't have a fix. //
//                 -- Rhett Buggler       //
// ====================================== //
