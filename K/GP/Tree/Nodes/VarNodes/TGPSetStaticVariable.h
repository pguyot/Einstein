// ==============================
// Fichier:			TGPSetStaticVariable.h
// Projet:			K
// 
// Cr�� le:			14/8/2003
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
// The Original Code is TGPSetStaticVariable.h.
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
// $Id: TGPSetStaticVariable.h,v 1.7 2004/11/23 15:11:31 paul Exp $
// ===========

#ifndef _TGPSETSTATICVARIABLE_H
#define _TGPSETSTATICVARIABLE_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNode.h>

// ISO C++
#include <string>

///
/// Classe pour un noeud qui change la valeur d'une variable statique.
/// Par variable statique on entend ici une variable simple dont la valeur
/// n'est pas d�termin�e dynamiquement mais par deux noeuds, Set et Get.
/// La valeur est initialis�e � chaque fois que Reset est appel� sur le
/// contexte.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.7 $
///
/// \test	aucun test d�fini.
///
class TGPSetStaticVariable
	:
		public TGPNode
{
public:
	///
	/// Constructeur � partir d'un nom de variable, d'un type de variable et d'un
	/// type pour le noeud.
	/// Ce noeud retourne la valeur de la variable, mais il peut �tre utile de d�finir
	/// son type comme void. Par d�faut, c'est le m�me type que la variable.
	///
	/// \param inVarName	nom de la variable (d�termine le stockage de la variable).
	/// \param inVarType	type de la variable.
	/// \param inNodeType	type du noeud.
	///
	TGPSetStaticVariable(
					const std::string& inVarName,
					const TGPType& inVarType,
					const TGPType& inNodeType );

	///
	/// Constructeur � partir d'un nom de variable et d'un type de variable.
	/// C'est le m�me constructeur que le pr�c�dent sauf que le type du noeud
	/// est d�fini comme �tant celui de la variable.
	///
	/// \param inVarName	nom de la variable (d�termine le stockage de la variable).
	/// \param inVarType	type de la variable.
	///
	TGPSetStaticVariable(
					const std::string& inVarName,
					const TGPType& inVarType );

	///
	/// R�cup�ration du type de ce noeud.
	///
	/// \return le type de ce noeud (kBoolean).
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

protected:
	///
	/// Constructeur par copie.
	///
	/// \param inCopy		objet � copier
	///
	TGPSetStaticVariable( const TGPSetStaticVariable& inCopy );

	///
	/// Accesseur sur le nom.
	///
	/// \return une r�f�rence sur le nom.
	///
	inline const std::string&	GetVarName( void ) const
		{
			return mVarName;
		}

private:
	///
	/// D�termine si ce noeud est constant.
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
							KUIntPtr* ioCookie );

	/// \name Constantes
	static const char*	kLispFormat;	///< Format du nom du noeud.

	/// \name Variables
	const std::string		mVarName;	///< Nom de la variable.
	const TGPType&			mVarType;	///< Type de la variable.
	const TGPType&			mNodeType;	///< Type du noeud.
	TStringRef				mLispName;	///< Nom du noeud.
};

#endif
		// _TGPSETSTATICVARIABLE_H

// ================================================ //
// An elephant is a mouse with an operating system. //
// ================================================ //
