// ==============================
// Fichier:			TGPNodeDo.h
// Projet:			K
// 
// Cr�� le:			3/9/2003
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
// The Original Code is TGPNodeDo.h.
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
// $Id: TGPNodeDo.h,v 1.7 2004/11/23 15:11:31 paul Exp $
// ===========

#ifndef _TGPNODEDO_H
#define _TGPNODEDO_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNode.h>

///
/// Classe pour un noeud repr�sentant l'ex�cution de plusieurs sous-arbre dans l'ordre.
/// Ce noeud retourne la valeur du dernier sous-arbre.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.7 $
///
/// \test	aucun test d�fini.
///
class TGPNodeDo
	:
		public TGPNode
{
public:
	///
	/// Constructeur � partir d'un type pour les sous-noeuds et d'une arit�.
	///
	/// \param inType	type des sous-arbres (et de ce noeud).
	/// \param inArity	arit� du noeud.
	///
	TGPNodeDo( const TGPType& inType, KUInt32 inArity );

	///
	/// R�cup�ration du type de ce noeud.
	///
	/// \return le type de ce noeud.
	///
	virtual const TGPType&	GetType( void ) const;

	///
	/// R�cup�ration du type d'un param�tre.
	/// Retourne le type intrins�que, i.e. ne demande pas au
	/// sous-noeud.
	///
	/// \param	inParamIndex		indice du param�tre dont on veut le type.
	/// \throws range_error s'il n'y pas un tel noeud.
	///
	virtual const TGPType&	GetParamType( KUInt32 inParamIndex ) const;

	///
	/// R�cup�re le nom du noeud.
	/// Ce nom doit �tre unique et permettre de recr�er le noeud (sans
	/// les sous-noeuds).
	///
	/// \return	une r�f�rence sur la cha�ne qui identifie le noeud.
	///
	virtual TStringRef		GetLispName( void ) const;

private:
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
							KUIntPtr* ioCookie );

	/// \name Variables
	const TGPType&		mType;		///< Type des sous-branches.
	TStringRef			mLispName;	///< Nom du noeud.
};

#endif
		// _TGPNODEDO_H

// ========================================= //
// Remember, UNIX spelled backwards is XINU. //
//                 -- Mt.                    //
// ========================================= //
