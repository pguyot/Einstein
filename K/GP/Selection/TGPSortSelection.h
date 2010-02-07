// ==============================
// Fichier:			TGPSortSelection.h
// Projet:			K
// 
// Créé le:			6/11/2003
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
// The Original Code is TGPSortSelection.h.
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
// $Id: TGPSortSelection.h,v 1.2 2004/11/23 15:11:30 paul Exp $
// ===========

#ifndef _TGPSORTSELECTION_H
#define _TGPSORTSELECTION_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Selection/TGPSelection.h>

///
/// Classe pour une méthode de sélection avec tri.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.2 $
///
/// \test	aucun test défini.
///
class TGPSortSelection
	:
		public TGPSelection
{
public:
	///
	/// Méthode appelée au début du processus de sélection.
	/// Trie la population par coefficient d'adéquation.
	///
	/// \param inPopulation		référence sur la population
	///
	virtual void NewGeneration( const std::vector<TGPTree*>& inPopulation );

protected:
	///
	/// Accesseur sur la population triée par coefficient d'adéqtion.
	///
	/// \return une copie de la population, triée.
	///
	inline const std::vector<TGPTree*>	GetSortedPopulation( void ) const
		{
			return mSortedPopulation;
		}
	
private:
	/// \name Variables
	std::vector<TGPTree*>		mSortedPopulation;	///< Population (triée)
};

#endif
		// _TGPSORTSELECTION_H

// ============================================================================== //
//         The programmers of old were mysterious and profound.  We cannot fathom //
// their thoughts, so all we do is describe their appearance.                     //
//         Aware, like a fox crossing the water.  Alert, like a general on the    //
// battlefield.  Kind, like a hostess greeting her guests. Simple, like uncarved  //
// blocks of wood.  Opaque, like black pools in darkened caves.                   //
//         Who can tell the secrets of their hearts and minds?                    //
//         The answer exists only in the Tao.                                     //
//                 -- Geoffrey James, "The Tao of Programming"                    //
// ============================================================================== //
