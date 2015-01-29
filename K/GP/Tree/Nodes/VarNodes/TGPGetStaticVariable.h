// ==============================
// Fichier:			TGPGetStaticVariable.h
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
// The Original Code is TGPGetStaticVariable.h.
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
// $Id: TGPGetStaticVariable.h,v 1.8 2005/02/14 20:55:05 pguyot Exp $
// ===========

#ifndef _TGPGETSTATICVARIABLE_H
#define _TGPGETSTATICVARIABLE_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNode.h>

// ISO C++
#include <string>

// K
#include <K/GP/Data/TGPType.h>

///
/// Classe pour un noeud qui r�cup�re la valeur d'une variable statique.
/// Par variable statique on entend ici une variable simple dont la valeur
/// n'est pas d�termin�e dynamiquement mais par deux noeuds, Set et Get.
/// La valeur est initialis�e � chaque fois que Reset est appel� sur le
/// contexte.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.8 $
///
/// \test	aucun test d�fini.
///
class TGPGetStaticVariable
	:
		public TGPNode
{
public:
	///
	/// Constructeur � partir d'un nom, d'un type et d'une valeur par d�faut.
	///
	/// \param inVarName		nom de la variable
	/// \param inType			type de la variable (et donc du noeud)
	/// \param inDefaultValue	valeur par d�faut de la variable (avant que Set ne soit
	///							appel�).
	///
	TGPGetStaticVariable(
					const std::string& inVarName,
					const TGPType& inType,
					TGPDataRef& inDefaultValue );

	///
	/// Constructeur � partir d'un nom et d'une valeur par d�faut. Le type est
	/// d�duit de la valeur par d�faut.
	///
	/// \param inVarName		nom de la variable
	/// \param inDefaultValue	valeur par d�faut de la variable (avant que Set ne soit
	///							appel�).
	///
	TGPGetStaticVariable(
					const std::string& inVarName,
					TGPDataRef& inDefaultValue );

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
	TGPGetStaticVariable( const TGPGetStaticVariable& inCopy );
	
	///
	/// Accesseur sur le nom.
	///
	/// \return une r�f�rence sur le nom.
	///
	inline const std::string&	GetVarName( void ) const
		{
			return mVarName;
		}

	///
	/// Accesseur sur la valeur par d�faut.
	///
	/// \return la valeur par d�faut.
	///
	inline const TGPDataRef&	GetDefaultValue( void ) const
		{
			return mDefaultValue;
		}

	///
	/// Accesseur sur la valeur par d�faut.
	///
	/// \return la valeur par d�faut.
	///
	inline TGPDataRef	GetDefaultValue( void )
		{
			return mDefaultValue;
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
	const std::string		mVarName;		///< Nom de la variable.
	const TGPType&			mType;			///< Type de la variable.
	TGPDataRef				mDefaultValue;	///< Valeur par d�faut (avant Set)
	TStringRef				mLispName;		///< Nom du noeud.
};

#endif
		// _TGPGETSTATICVARIABLE_H

// ============================================================== //
// Real Programmers think better when playing Adventure or Rogue. //
// ============================================================== //
