// ==============================
// Fichier:			TGPEvaluationContext.h
// Projet:			K
//
// Cr�� le:			25/2/2003
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
/// Classe pour g�rer le contexte durant une �valuation.
/// Le contexte comprend les valeurs des variables et probablement
/// d'autres choses encore.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
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
	/// Constructeur par d�faut.
	///
	TGPEvaluationContext( void );

	///
	/// Destructeur.
	///
	virtual ~TGPEvaluationContext( void );

	///
	/// R�initialise le contexte d'�valuation (pour un autre arbre).
	/// Par d�faut, vide la bo�te des biscuits, le cache et r�initialise
	/// le compteur des �tapes.
	/// Cette m�thode doit donc �tre appel�e si elle est surcharg�e.
	///
	virtual void Reset( void );
	
	///
	/// Ajoute un �l�ment au cache. Celui-ci sera supprim�
	/// lorsque la m�thode Reset sera appel�e.
	///
	/// \param inItem	�l�ment � ajouter au cache.
	///
	inline void AddToCache( TCacheItem* inItem )
		{
			mCachePool.push_back( inItem );
		}
	
	///
	/// Supprime un �l�ment du cache. Celui-ci ne sera plus supprim�
	/// lorsque la m�thode Reset sera appel�e.
	///
	/// \param inItem	�l�ment � supprimer du cache.
	///
	inline void RemoveFromCache( TCacheItem* inItem )
		{
			mCachePool.remove( inItem );
		}

	///
	/// D�termine si un biscuit a �t� d�fini.
	///
	/// \param inCookieName	nom du biscuit
	/// \return \c true si le biscuit existe, \c false sinon.
	///
	inline Boolean		HasCookie( std::string inCookieName )
		{
			return (mCookieJar.count( inCookieName ) > 0);
		}

	///
	/// Accesseur sur un biscuit. Cr�e le biscuit si n�cessaire.
	///
	/// \param inCookieName	nom du biscuit.
	/// \return une r�f�rence sur un biscuit.
	///
	inline TGPDataRef	GetCookie( std::string inCookieName )
		{
			return mCookieJar[ inCookieName ];
		}

	///
	/// S�lecteur sur un biscuit. Cr�e le biscuit si n�cessaire.
	///
	/// \param inCookieName		nom du biscuit.
	/// \param inCookieValue	valeur du biscuit.
	///
	inline void		SetCookie( std::string inCookieName, TGPDataRef& inCookieValue )
		{
			mCookieJar[ inCookieName ] = inCookieValue;
		}

	///
	/// D�termine si on continue l'�valuation (permet d'interrompre en cas
	/// d'erreur par exemple).
	///
	/// \return \c true si on continue d'�valuer, \c false sinon.
	///
	inline Boolean	ContinueEvaluation( void )
		{
			return mContinue;
		}

	///
	/// Arr�te l'�valuation.
	///
	inline void	StopEvaluation( void )
		{
			mContinue = false;
		}

	///
	/// Reprend l'�valuation.
	///
	inline void	ResumeEvaluation( void )
		{
			mContinue = true;
		}

protected:
	///
	/// Accesseur sur la bo�te des biscuits.
	///
	/// \return une r�f�rence sur la bo�te de biscuits.
	///
	inline std::map< std::string, TGPDataRef >&	GetCookieJar( void )
		{
			return mCookieJar;
		}

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPEvaluationContext( const TGPEvaluationContext& inCopy );

	///
	/// Op�rateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPEvaluationContext& operator = ( const TGPEvaluationContext& inCopy );
	
	/// \name Variables
	std::map< std::string, TGPDataRef >	
									mCookieJar;		///< Bo�te avec les
													///<  biscuits
	std::list< TCacheItem* >		mCachePool;		///< Ensemble d'�l�ment
													///<  pour le cache.
	Boolean							mContinue;		///< Si on continue l'�valuation.
};

#endif
		// _TGPEVALUATIONCONTEXT_H

// ========================================= //
// An algorithm must be seen to be believed. //
//                 -- D.E. Knuth             //
// ========================================= //
