// ==============================
// Fichier:			UProcessorTests.h
// Projet:			Einstein
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			27/7/2004
// Tabulation:		4 espaces
// 
// Copyright:		© 2004 by Paul Guyot.
// 					Tous droits réservés pour tous pays.
// ===========
// $Id: UProcessorTests.h 147 2005-09-29 20:17:58Z paul $
// ===========

#ifndef _UPROCESSORTESTS_H
#define _UPROCESSORTESTS_H

#include <K/Defines/KDefinitions.h>

///
/// Class for tests relative to the processor.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class UProcessorTests
{
public:
	///
	/// Execute an instruction and print the registers afterwards.
	/// The processor is set in boot state.
	///
	/// \param inHexWord	instruction (as hexa) to execute.
	///
	static void ExecuteInstruction( const char* inHexWord );

	///
	/// Execute an instruction and print the registers afterwards.
	/// The processor is set in some random (actually fixed) state.
	///
	/// \param inHexWord	instruction (as hexa) to execute.
	///
	static void ExecuteInstructionState1( const char* inHexWord );

	///
	/// Execute an instruction and print the registers afterwards.
	/// The processor is set in some random (actually fixed) state.
	///
	/// \param inHexWord	instruction (as hexa) to execute.
	///
	static void ExecuteInstructionState2( const char* inHexWord );

	///
	/// Execute two instructions and print the registers afterwards.
	///
	/// \param inHexWord	instructions (as hexa) to execute.
	///
	static void ExecuteTwoInstructions( const char* inHexWords );

	///
	/// Step into the ROM (found at ../../_Data_/717006)
	///
	/// \param inCount  number of steps to perform.
	///
	static void Step( const char* inCount );
};

#endif
		// _UPROCESSORTESTS_H

// =============================================================================== //
// Norbert Weiner was the subject of many dotty professor stories.  Weiner was, in //
// fact, very absent minded.  The following story is told about him: when they     //
// moved from Cambridge to Newton his wife, knowing that he would be absolutely    //
// useless on the move, packed him off to MIT while she directed the move.  Since  //
// she was certain that he would forget that they had moved and where they had     //
// moved to, she wrote down the new address on a piece of paper, and gave it to    //
// him.  Naturally, in the course of the day, an insight occurred to him.  He      //
// reached in his pocket, found a piece of paper on which he furiously scribbled   //
// some notes, thought it over, decided there was a fallacy in his idea, and       //
// threw the piece of paper away.  At the end of the day he went home (to the      //
// old address in Cambridge, of course).  When he got there he realized that they  //
// had moved, that he had no idea where they had moved to, and that the piece of   //
// paper with the address was long gone.  Fortunately inspiration struck.  There   //
// was a young girl on the street and he conceived the idea of asking her where    //
// he had moved to, saying, "Excuse me, perhaps you know me.  I'm Norbert Weiner   //
// and we've just moved.  Would you know where we've moved to?"  To which the      //
// young girl replied, "Yes, Daddy, Mommy thought you would forget."               //
//         The capper to the story is that I asked his daughter (the girl in the   //
// story) about the truth of the story, many years later.  She said that it wasn't //
// quite true -- that he never forgot who his children were!  The rest of it,      //
// however, was pretty close to what actually happened...                          //
//                 -- Richard Harter                                               //
// =============================================================================== //
