#define _CRT_SECURE_NO_WARNINGS
#include "One_Header_v5.h"
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "Rando.h"
#include <limits.h>

//#define MAX_NUM_PLAYERS 24
#ifdef AUTOMATE_PLAY
#define NDEBUG
#endif // AUTOMATE_PLAY


// INDIVIDUAL TESTS
//#define TEST_BUILD_A_DECK 108
//#define TEST_DESTROY_A_DECK 108
//#define TEST_PRINT_A_CARD
//#define TEST_START_GAME
#define TEST_MULTIPLE_PLAYERS 4

int main(void)
{
	// VARIABLES
	int i = 0;
	int numTries = 0;

#ifdef TEST_BUILD_A_DECK
	oneCard * badTestTemp = NULL;
	oneCard * badTest0 = build_a_new_deck();
	assert(badTest0);

	badTestTemp = badTest0;
	while (NULL != badTestTemp && numTries < STOP_LOOPING)
	{
		if (NULL != badTestTemp)
		{
			numTries++;
			badTestTemp = badTestTemp->nextCard;
		}
	}
	assert(TEST_BUILD_A_DECK == numTries);

#ifdef TEST_DESTROY_A_DECK
	numTries = destroy_a_deck(badTest0);
	assert(TEST_DESTROY_A_DECK == numTries);
#endif // TEST_DESTROY_A_DECK
#endif // TEST_BUILD_A_DECK

#ifdef TEST_PRINT_A_CARD
	oneCard * pacTest0Deck = build_a_new_deck();
	assert(pacTest0Deck);
	oneCard * pacTest0Discard = NULL;
	oneCard * pacTest0Hand = deal_a_hand(&pacTest0Deck);
	assert(pacTest0Hand);

	print_one_card(pacTest0Hand);
	print_players_hand(pacTest0Hand);

	// Free it
	destroy_a_deck(&pacTest0Deck);
	destroy_a_deck(&pacTest0Discard);
	destroy_a_deck(&pacTest0Hand);
#endif // TEST_PRINT_A_CARD

#ifdef TEST_START_GAME
	unsigned int numOfPlayers = 0;
	oneCard * TSGdrawPile = build_a_new_deck();
	oneCard * TSGdiscardPile = NULL;
	oneCard ** TSGplayersHands = NULL;
	oneCard * TSGtmpCards = NULL;				// Temporary cards
	oneCard * TSGtmpDeck = NULL;				// Temporary deck
	const oneCard * TSGblankCard = NULL;		// Used as a placeholder to print the blank tops of cards
	int gameOver = 0;							// bool value representing continuation of the game
	int TSGtmpRetVal = 0;						// Temporarily holds int return values

//	clear_stdin();
	printf("How many players?\t");
	scanf("%u", &numOfPlayers);
	clear_stdin();
	assert(numOfPlayers > 1 && numOfPlayers <= MAX_NUM_PLAYERS);

	if (numOfPlayers > 1 && numOfPlayers <= MAX_NUM_PLAYERS)
	{
		// Verify there are enough cards
		//while ((numOfPlayers * 7) > (count_the_cards(TSGdrawPile) - 1))
		//{
		//	TSGtmpCards = build_a_new_deck();							// Get another deck of cards
		//	assert(TSGtmpCards);

		//	if (NULL != TSGtmpCards)									// Add that new deck to the shoe
		//	{
		//		TSGtmpDeck = link_a_card(TSGdrawPile, TSGtmpCards);
		//		assert(TSGtmpDeck == TSGtmpCards);						// Link a card places the new card/deck on top
		//		TSGtmpCards = NULL;										// This variable is now part of TSGtmpDeck
		//		TSGdrawPile = TSGtmpDeck;								// This variable is now part of TSGdrawPile
		//	}
		//}

		// Store the pointers to player hands
		// Index[0] == user;
		TSGplayersHands = (oneCard**)calloc(numOfPlayers, sizeof(oneCard*));
		assert(NULL != TSGplayersHands);

		// Deal the cards to the players
//		if (NULL != TSGplayersHands)
//		{
//			for (i = 0; i < numOfPlayers; i++)
//			{
//				(*(TSGplayersHands + i)) = deal_a_hand(&TSGdrawPile);
////				assert(7 == count_the_cards((*(TSGplayersHands + i))));
//			}
//
////			assert(numOfPlayers >= 16 || ((108 - (numOfPlayers * 7)) == count_the_cards(TSGdrawPile)));
//		}

		// Flip a card into the discard pile
		//TSGtmpCards = move_one_card(&TSGdrawPile, &TSGdiscardPile, 0);
		//assert(TSGtmpCards == TSGdiscardPile);

		// Print the status of the game
//		print_the_game(TSGplayersHands, TSGdiscardPile);		// Let the menu print the game

		//// Print the draw pile and discard pile
		//puts("Draw Pile");
		//print_one_card(TSGblankCard);
		//puts("Discard Pile");
		//print_one_card(TSGdiscardPile);

		//// Print the player's hand
		//puts("Your Hand");
		//print_players_hand(*(TSGplayersHands));

		// Setup the game
		TSGtmpRetVal = setup_game(&TSGdrawPile, &TSGdiscardPile, TSGplayersHands, numOfPlayers);
		assert(0 == TSGtmpRetVal);

/*
		///////////////
		// DEBUGGING //
		///////////////
		(*TSGplayersHands)->type = TYPE_WILD;
*/

		// Begin the game
		while (0 == gameOver)
		{
			TSGtmpRetVal = player_menu(TSGplayersHands, &TSGdrawPile, &TSGdiscardPile);
			assert(0 == TSGtmpRetVal || 1 == TSGtmpRetVal);

			//if (NULL == *TSGplayersHands)
			//{
			//	puts("YOU WON!");
			//}

			if (0 != TSGtmpRetVal)		// Quit playing on any error or user choice
			{
				gameOver = 1;
			}
		}
	}
	else
	{
		puts("That is an unreasonable number of players.");
	}

/*
	////////////////////
	// DEBUGGING INFO //
	////////////////////
	int runningCardCount = 0;
	int currentTmpCount = 0;
	puts("Here are the 'deck' stats");
	//// Count the player's hands
	for (i = 0; i < numOfPlayers; i++)
	{
		currentTmpCount = count_the_cards((*(TSGplayersHands + i)));
		assert(currentTmpCount >= 0 || NULL == (*(TSGplayersHands + i)));
		runningCardCount += currentTmpCount;
		printf("Player Hand #%d:  \t%d cards\n", i, currentTmpCount);
	}
	//// Count the draw pile
	currentTmpCount = count_the_cards(TSGdrawPile);
	assert(currentTmpCount >= 0);
	runningCardCount += currentTmpCount;
	printf("Draw Pile:\t%d cards\n", currentTmpCount);
	//// Count the discard pile
	currentTmpCount = count_the_cards(TSGdiscardPile);
	assert(currentTmpCount >= 0);
	runningCardCount += currentTmpCount;
	printf("Discard Pile:\t%d cards\n", currentTmpCount);
	//// Print the total count of cards
	printf("There are %d total cards in this shoe.\n", runningCardCount);
	if (0 != (runningCardCount % 108))
	{
		puts("Somehow, there were some spare cards somewhere?!");
	}
	////////////////////
	////////////////////
	////////////////////
*/

	// Free it all
	destroy_a_deck(&TSGdrawPile);
	destroy_a_deck(&TSGdiscardPile);
	for (i = 0; i < numOfPlayers; i++)
	{
		destroy_a_deck((*(TSGplayersHands + i)));
	}
	free(TSGplayersHands);
	TSGplayersHands = NULL;

#endif // TEST_START_GAME

#ifdef TEST_MULTIPLE_PLAYERS

	// Game Decks
	oneCard * TMPdrawPile = NULL;
	oneCard * TMPdiscardPile = NULL;
	oneCard * TMPtempDeck1 = NULL;
	oneCard * TMPtempDeck2 = NULL;

	// Players
#ifdef AUTOMATE_PLAY
	onePlayer player1 = { NULL, 1, NULL, 0, 0, &AI_algorithm_points, NULL };		// AI
#else
	onePlayer player1 = { NULL, 1, NULL, 0, 0, &player_menu, NULL };				// User
#endif // AUTOMATE_PLAY
	onePlayer player2 = { &player1, 2, NULL, 0, 0, &AI_algorithm_points, NULL };	// AI
	onePlayer player3 = { &player2, 3, NULL, 0, 0, &AI_algorithm_random, NULL };	// AI
	onePlayer player4 = { &player3, 4, NULL, 0, 0, &AI_algorithm_chaotic, NULL };	// AI
	onePlayer player5 = { &player4, 5, NULL, 0, 0, &AI_algorithm_aggressive, NULL };	// AI
	player1.prevPlayer = &player5;
	player1.nextPlayer = &player2;
	player2.nextPlayer = &player3;
	player3.nextPlayer = &player4;
	player4.nextPlayer = &player5;
	player5.nextPlayer = &player1;
	onePlayer * currentPlayer = NULL;		// Pointer to the player taking a turn
	onePlayer * playerOne = NULL;			// Pointer to the player that started this hand

	// Misc Variables
	int TMPtmpRetVal1 = 0;
	int TMPtmpRetVal2 = 0;
	int directionOfPlay = 1;
	int continuePlaying = 1;
	char userInput = 0;

	// Randomize the first player
	TMPtmpRetVal1 = rando_me(0, 3);			// Hard coded
	assert(TMPtmpRetVal1 >= 0 && TMPtmpRetVal1 <= 3);
	i = TMPtmpRetVal1;

	// Set starting point for first player
	playerOne = &player1;

	if (TMPtmpRetVal1 >= 0 && TMPtmpRetVal1 <= 3)
	{
		while (i > 0)
		{
			playerOne = playerOne->nextPlayer;
			i--;
		}
	}

	while (1 == continuePlaying)
	{
		// Setup the game
		currentPlayer = playerOne;
		directionOfPlay = PLAY_CLOCKWISE;
		TMPtmpRetVal1 = setup_game(&TMPdrawPile, &TMPdiscardPile, currentPlayer);
		assert(0 == TMPtmpRetVal1);

		// Start the game
		TMPtmpRetVal1 = 0;

		printf("Player #%d will begin.\n", currentPlayer->playerNumber);
		PAUSE_OUTPUT(NUM_SECONDS_TO_PAUSE);

		while (0 == TMPtmpRetVal1)
		{
			TMPtmpRetVal1 = currentPlayer->algorithm(&(currentPlayer->currentHand), &TMPdrawPile, &TMPdiscardPile, currentPlayer, directionOfPlay);

			if (1 == TMPtmpRetVal1 || NULL == currentPlayer->currentHand)
			{
				printf("Player #%d won!\n", currentPlayer->playerNumber);
				TMPtmpRetVal1 = 1;

				// Score the game
				TMPtmpRetVal2 = score_players_hands(currentPlayer);
				assert(0 == TMPtmpRetVal2 || 1 == TMPtmpRetVal2);

				if (0 == TMPtmpRetVal2)
				{
					puts("Would you like to continue playing?\n(Y or N)\n");
#ifdef AUTOMATE_PLAY
					PAUSE_OUTPUT(NUM_SECONDS_TO_PAUSE * 5);
					userInput = 'Y';
#else
					userInput = getchar();
					userInput = toupper(userInput);
					clear_stdin();
#endif // AUTOMATE_PLAY

					if ('Y' != userInput)
					{
						continuePlaying = 0;
					}
				}
				else if (1 == TMPtmpRetVal2)
				{
					continuePlaying = 0;		// Someone is about to overflow
				}
			}
			else if (0 == TMPtmpRetVal1)		// No winner yet
			{
				if (NULL != TMPdiscardPile)	// Safety check
				{
					if (TYPE_REVERSE == TMPdiscardPile->type && 0 == TMPdiscardPile->processed)
					{
						// Swap direction of play
						if (PLAY_CLOCKWISE == directionOfPlay)
						{
							directionOfPlay = PLAY_COUNTER_CLOCKWISE;
							puts("Play is now counter clockwise.");
							PAUSE_OUTPUT(NUM_SECONDS_TO_PAUSE);
						}
						else
						{
							directionOfPlay = PLAY_CLOCKWISE;
							puts("Play is now clockwise.");
							PAUSE_OUTPUT(NUM_SECONDS_TO_PAUSE);
						}

						// Set reverse to processed
						TMPdiscardPile->processed = 1;
					}

					// Verify the current player's card counts
					if (currentPlayer->currentCardCount != count_the_cards(currentPlayer->currentHand))
					{
						printf("Player #%d reports having %d cards but should actually have %d cards",
							currentPlayer->playerNumber, 
							currentPlayer->currentCardCount, 
							count_the_cards(currentPlayer->currentHand)
						);
					}
					assert(currentPlayer->currentCardCount == count_the_cards(currentPlayer->currentHand));

					if (PLAY_CLOCKWISE == directionOfPlay)
					{
						currentPlayer = currentPlayer->nextPlayer;
					}
					else if (PLAY_COUNTER_CLOCKWISE == directionOfPlay)
					{
						currentPlayer = currentPlayer->prevPlayer;
					}
					else
					{
						puts("Something went wrong with the direction of play.");
						continuePlaying = 0;
						break;
					}
				}
				else
				{
					puts("Somehow, the discard pile is empty.");
					continuePlaying = 0;
					break;
				}
			}
			else
			{
				printf("Game ended with error code:\t%d\n", TMPtmpRetVal1);
				continuePlaying = 0;
			}
		}

		if (1 == continuePlaying)
		{
			// Reset game
			TMPtmpRetVal1 = reset_game(&TMPdrawPile, &TMPdiscardPile, &player1);
			assert(0 == TMPtmpRetVal1);

			if (0 != TMPtmpRetVal1)
			{
				puts("Reset failed!");
			}
			else
			{
//				printf("Total points in the deck:\t%d\n", hand_value(TMPdrawPile));
			}

			// Pass the deck to the next player
			if (NULL != playerOne)
			{
				playerOne = playerOne->nextPlayer;
				assert(NULL != playerOne);
			}
		}
	}

	// Free it all
	//// Free the game decks
	destroy_a_deck(&TMPdrawPile);
	destroy_a_deck(&TMPdiscardPile);

	//// Free the player hands
	if (NULL != currentPlayer)
	{
		// Find player1
		while (currentPlayer != &player1)
		{
			currentPlayer = currentPlayer->nextPlayer;
		}

		// Start freeing player hands
		do 
		{
			if (NULL != currentPlayer->currentHand)
			{
				destroy_a_deck(&(currentPlayer->currentHand));
			}

			currentPlayer = currentPlayer->nextPlayer;
		} while (currentPlayer != &player1 && NULL != currentPlayer);
	}

#endif // TEST_MULTIPLE_PLAYERS


	return 0;
}
