// ==============================
// Fichier:			TGPNodeLeaf.h
// Projet:			K
//
// Créé le:			25/2/2003
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
// The Original Code is TGPNodeLeaf.h.
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
// $Id: TGPNodeLeaf.h,v 1.3 2004/11/23 15:11:30 paul Exp $
// ===========

#ifndef _TGPNODELEAF_H
#define _TGPNODELEAF_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNode.h>

// K
#include <K/GP/Data/TGPType.h>

///
/// Classe pour un noeud qui est une feuille.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TGPNodeLeaf
	:
		public TGPNode
{
public:
	///
	/// Constructeur par défaut.
	///
	TGPNodeLeaf( void );

	///
	/// Récupération du type d'un paramètre.
	/// Lance une exception car ce noeud est une feuille et n'a donc pas
	/// de sous-noeud.
	///
	/// \param	inParamIndex		indice du paramètre dont on veut le type.
	/// \throws range_error s'il n'y pas un tel noeud.
	///
	virtual const TGPType&	GetParamType( KUInt32 inParamIndex ) const;
};

#endif
		// _TGPNODELEAF_H

// ====================== //
// That does not compute. //
// ====================== //
