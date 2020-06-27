// ==============================
// Fichier:         TSplashScreenInfo.h
// Projet:          (Bibliothèque)
// Ecrit par:       Paul Guyot (pguyot@kallisys.net)
//
// Créé le:         2/8/2004
// Tabulation:      4 espaces
//
// Copyright:       © 2004 by Paul Guyot.
//                  Tous droits réservés pour tous pays.
// ===========
// $Id: TSplashScreenInfo.h 107 2005-01-05 10:20:39Z paul $
// ===========

#ifndef _TSPLASHSCREENINFO_H
#define _TSPLASHSCREENINFO_H

#include <K/Defines/KDefinitions.h>

///
/// Classe pour .
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 107 $
///
/// \test   aucun test défini.
///
class TSplashScreenInfo
{
public:
    /// \name Constantes publiques

    /// \name Constructeurs et destructeur

    ///
    /// Constructeur par défaut.
    ///
    TSplashScreenInfo( void );

    ///
    /// Destructeur.
    ///
    virtual ~TSplashScreenInfo( void );

private:
    ///
    /// Constructeur par copie volontairement indisponible.
    ///
    /// \param inCopy       objet à copier
    ///
    TSplashScreenInfo( const TSplashScreenInfo& inCopy );

    ///
    /// Opérateur d'assignation volontairement indisponible.
    ///
    /// \param inCopy       objet à copier
    ///
    TSplashScreenInfo& operator = ( const TSplashScreenInfo& inCopy );

    /// \name Variables

};

#endif
        // _TSPLASHSCREENINFO_H

// ============================================================================ //
// Dear Emily:                                                                  //
//         How can I choose what groups to post in?                             //
//                 -- Confused                                                  //
//                                                                              //
// Dear Confused:                                                               //
//         Pick as many as you can, so that you get the widest audience.  After //
// all, the net exists to give you an audience.  Ignore those who suggest you   //
// should only use groups where you think the article is highly appropriate.    //
// Pick all groups where anybody might even be slightly interested.             //
//         Always make sure followups go to all the groups.  In the rare event  //
// that you post a followup which contains something original, make sure you    //
// expand the list of groups.  Never include a "Followup-to:" line in the       //
// header, since some people might miss part of the valuable discussion in      //
// the fringe groups.                                                           //
//                 -- Emily Postnews Answers Your Questions on Netiquette       //
// ============================================================================ //
