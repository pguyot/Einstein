// ==============================
// Fichier:			TGPEvaluationContext.h
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
// The Original Code is TGPEvaluationContext.h.
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
// $Id: TGPEvaluationContext.h,v 1.6 2005/02/14 20:55:00 pguyot Exp $
// ===========

#ifndef _TGPEVALUATIONCONTEXT_H
#define _TGPEVALUATIONCONTEXT_H

#include <K/Defines/KDefinitions.h>

// ISO C++
#include <map>
#include <list>

// K GP
#include <K/GP/Data/TGPDataRef.h>

///
/// Classe pour gérer le contexte durant une évaluation.
/// Le contexte comprend les valeurs des variables et probablement
/// d'autres choses encore.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TGPEvaluationContext
{
public:
	class TCacheItem
	{
	public:
		TCacheItem( void ) {};
		virtual ~TCacheItem( void ) {};
	};
	
	///
	/// Constructeur par défaut.
	///
	TGPEvaluationContext( void );

	///
	/// Destructeur.
	///
	virtual ~TGPEvaluationContext( void );

	///
	/// Réinitialise le contexte d'évaluation (pour un autre arbre).
	/// Par défaut, vide la boîte des biscuits, le cache et réinitialise
	/// le compteur des étapes.
	/// Cette méthode doit donc être appelée si elle est surchargée.
	///
	virtual void Reset( void );
	
	///
	/// Ajoute un élément au cache. Celui-ci sera supprimé
	/// lorsque la méthode Reset sera appelée.
	///
	/// \param inItem	élément à ajouter au cache.
	///
	inline void AddToCache( TCacheItem* inItem )
		{
			mCachePool.push_back( inItem );
		}
	
	///
	/// Supprime un élément du cache. Celui-ci ne sera plus supprimé
	/// lorsque la méthode Reset sera appelée.
	///
	/// \param inItem	élément à supprimer du cache.
	///
	inline void RemoveFromCache( TCacheItem* inItem )
		{
			mCachePool.remove( inItem );
		}

	///
	/// Détermine si un biscuit a été défini.
	///
	/// \param inCookieName	nom du biscuit
	/// \return \c true si le biscuit existe, \c false sinon.
	///
	inline Boolean		HasCookie( std::string inCookieName )
		{
			return (mCookieJar.count( inCookieName ) > 0);
		}

	///
	/// Accesseur sur un biscuit. Crée le biscuit si nécessaire.
	///
	/// \param inCookieName	nom du biscuit.
	/// \return une référence sur un biscuit.
	///
	inline TGPDataRef	GetCookie( std::string inCookieName )
		{
			return mCookieJar[ inCookieName ];
		}

	///
	/// Sélecteur sur un biscuit. Crée le biscuit si nécessaire.
	///
	/// \param inCookieName		nom du biscuit.
	/// \param inCookieValue	valeur du biscuit.
	///
	inline void		SetCookie( std::string inCookieName, TGPDataRef& inCookieValue )
		{
			mCookieJar[ inCookieName ] = inCookieValue;
		}

	///
	/// Détermine si on continue l'évaluation (permet d'interrompre en cas
	/// d'erreur par exemple).
	///
	/// \return \c true si on continue d'évaluer, \c false sinon.
	///
	inline Boolean	ContinueEvaluation( void )
		{
			return mContinue;
		}

	///
	/// Arrête l'évaluation.
	///
	inline void	StopEvaluation( void )
		{
			mContinue = false;
		}

	///
	/// Reprend l'évaluation.
	///
	inline void	ResumeEvaluation( void )
		{
			mContinue = true;
		}

protected:
	///
	/// Accesseur sur la boîte des biscuits.
	///
	/// \return une référence sur la boîte de biscuits.
	///
	inline std::map< std::string, TGPDataRef >&	GetCookieJar( void )
		{
			return mCookieJar;
		}

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPEvaluationContext( const TGPEvaluationContext& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPEvaluationContext& operator = ( const TGPEvaluationContext& inCopy );
	
	/// \name Variables
	std::map< std::string, TGPDataRef >	
									mCookieJar;		///< Boîte avec les
													///<  biscuits
	std::list< TCacheItem* >		mCachePool;		///< Ensemble d'élément
													///<  pour le cache.
	Boolean							mContinue;		///< Si on continue l'évaluation.
};

#endif
		// _TGPEVALUATIONCONTEXT_H

// ========================================= //
// An algorithm must be seen to be believed. //
//                 -- D.E. Knuth             //
// ========================================= //
