// ==============================
// Fichier:			TGPNodeBoolean.h
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
// The Original Code is TGPNodeBoolean.h.
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
// $Id: TGPNodeBoolean.h,v 1.3 2004/11/23 15:11:30 paul Exp $
// ===========

#ifndef _TGPNODEBOOLEAN_H
#define _TGPNODEBOOLEAN_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNode.h>

// K
#include <K/GP/Data/TGPType.h>

///
/// Classe de base pour un noeud opérant sur des booléens.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TGPNodeBoolean
	:
		public TGPNode
{
public:
	///
	/// Constructeur à partir de l'arité.
	///
	/// \param	inArity		arité du noeud.
	///
	TGPNodeBoolean( KUInt32 inArity );

	///
	/// Récupération du type de ce noeud.
	///
	/// \return le type de ce noeud.
	///
	virtual const TGPType&	GetType( void ) const;

	///
	/// Récupération du type d'un paramètre.
	/// Retourne le type intrinsèque, i.e. ne demande pas au
	/// sous-noeud.
	///
	/// \param	inParamIndex		indice du paramètre dont on veut le type.
	/// \throws range_error s'il n'y pas un tel noeud.
	///
	virtual const TGPType&	GetParamType( KUInt32 inParamIndex ) const;
};

#endif
		// _TGPNODEBOOLEAN_H

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
