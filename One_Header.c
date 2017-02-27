#define _CRT_SECURE_NO_WARNINGS
#include "One_Header_v5.h"
#include <stdlib.h>
#include <assert.h>
#include "Rando.h"
#include <limits.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_TRIES 10
#ifdef AUTOMATE_PLAY
#define NDEBUG
#endif // AUTOMATE_PLAY

// Purpose:
//		Builds a doubly linked list of "One" cards
// Returns:
//		Pointer to head node on success
//		NULL on failure
// Notes:
//		This deck is "shuffled"
oneCard * build_a_new_deck(void)
{
	oneCard * ndReturnValue = NULL;
	oneCard * tempCard = NULL;
	int numCards = 0;
	int numColors = 0;
	int numTypes = 0;
	int i = 0;

	// Allocate the deck
	ndReturnValue = allocate_a_card();
	
	if (NULL != ndReturnValue)
	{
		numCards++;

		while (numCards < 108)
		{
			tempCard  = allocate_a_card();
			assert(tempCard);
			ndReturnValue = link_a_card(ndReturnValue, tempCard);
			assert(ndReturnValue);
			numCards++;
		}
	}

	tempCard = ndReturnValue;

	// Define the deck
	//// COLORS
	for (numColors = COLOR_RED; numColors <= COLOR_BLUE; numColors++)
	{
		////// 0
		if (NULL != tempCard)
		{
			tempCard->color = numColors;
			tempCard->type = '0';
			tempCard->processed = 0;
			tempCard = tempCard->nextCard;
		}
		else
		{
			ndReturnValue = NULL;
			break;
		}
		
		////// Double...
		for (i = 0; i < 2; i++)
		{
			////////  1 - 9
			for (numTypes = '1'; numTypes <= '9'; numTypes++)
			{
				if (NULL != tempCard)
				{
					tempCard->color = numColors;
					tempCard->type = numTypes;
					tempCard->processed = 0;
					tempCard = tempCard->nextCard;
				}
				else
				{
					ndReturnValue = NULL;
					break;
				}
			}

			//////// Reverse
			if (NULL != tempCard)
			{
				tempCard->color = numColors;
				tempCard->type = TYPE_REVERSE;
				tempCard->processed = 0;
				tempCard = tempCard->nextCard;
			}

			//////// Skip
			if (NULL != tempCard)
			{
				tempCard->color = numColors;
				tempCard->type = TYPE_SKIP;
				tempCard->processed = 0;
				tempCard = tempCard->nextCard;
			}

			//////// Draw 2
			if (NULL != tempCard)
			{
				tempCard->color = numColors;
				tempCard->type = TYPE_DRAW_TWO;
				tempCard->processed = 0;
				tempCard = tempCard->nextCard;
			}
		}
	}

	//// 4 x...
	for (i = 0; i < 4; i++)
	{
		////// Wild cards
		if (NULL != tempCard)
		{
			tempCard->color = COLOR_BLACK;
			tempCard->type = TYPE_WILD;
			tempCard->processed = 0;
			tempCard = tempCard->nextCard;
		}

		////// Wild Draw Fours
		if (NULL != tempCard)
		{
			tempCard->color = COLOR_BLACK;
			tempCard->type = TYPE_WILD_DRAW_FOUR;
			tempCard->processed = 0;
			tempCard = tempCard->nextCard;
		}
	}

	// Shuffle the new deck
	tempCard = shuffle_a_deck(ndReturnValue);
	assert(NULL != tempCard);
	if (NULL != tempCard)
	{
		ndReturnValue = tempCard;
	}

	return ndReturnValue;
}


// Purpose:
//		Free all the dynamically allocated memory for all the cards in the linked list
// Returns:
//		Number of cards freed on success
//		0 on failure
//		-1 on NULL pointer
//		-2 on unspecified error
int destroy_a_deck(oneCard ** oldDeck)
{
	int dadReturnValue = -1;
	oneCard * tempCard = NULL;

	if (NULL != oldDeck)
	{
		if (NULL != *oldDeck)
		{
			dadReturnValue = 0;

			/* This refactoring was attempting to resolve a crash bug */
			//tempCard = *oldDeck;

			// Find the end
			//while (NULL != tempCard->nextCard)
			//{
			//	tempCard = tempCard->nextCard;
			//}
			tempCard = find_the_last_card(*oldDeck);
			assert(NULL != tempCard);

			if (NULL != tempCard)
			{
				// Back it up one
				if (NULL != tempCard->prevCard)			// Checks for a deck with only one card
				{
					tempCard = tempCard->prevCard;
				}

				// Begin freeing from the end
				while (NULL != tempCard->nextCard)
				{
					if (0 != destroy_a_card(&(tempCard->nextCard)))
					{
						perror("Something went wrong deallocating this card.\n");
					}
					else
					{
						if (NULL != tempCard->prevCard)
						{
							tempCard = tempCard->prevCard;
						}
					}
					dadReturnValue++;
				}
			}
			destroy_a_card(oldDeck);
			dadReturnValue++;
		}
	}

	return dadReturnValue;
}


oneCard * allocate_a_card(void)
{
	oneCard * aacReturnValue = NULL;
	int aacNumTries = 0;

	while (NULL == aacReturnValue && aacNumTries < STOP_LOOPING)
	{
		aacReturnValue = (oneCard *)calloc(1, sizeof(oneCard));
		aacNumTries++;
	}

	return aacReturnValue;
}


// Purpose:
//		Free dynamically allocated memory and zero the data
// Returns:
//		0 on success
//		-1 on failure
int destroy_a_card(oneCard ** oldCard)
{
	int dacReturnValue = -1;

	if (NULL != oldCard)
	{
		if (NULL != (*oldCard))
		{
			// Zero the data
			(*oldCard)->color = 0;
			(*oldCard)->nextCard = NULL;
			(*oldCard)->prevCard = NULL;
			(*oldCard)->type = 0;
			(*oldCard)->processed = 0;

			// Free the memory
			free((*oldCard));

			// Zero the pointer
			if (NULL != (*oldCard))
			{
				(*oldCard) = NULL;
			}

			// Set the return value
			dacReturnValue = 0;
		}
	}

	return dacReturnValue;
}


// Purpose:
//		Add a card to a deck
// Returns:
//		Head node on success
//		NULL on NULL pointer or error
//		cardToLink if existingDeck is NULL
// Note:
//		cardToLink is 'placed on top' of existingDeck
//		This function will work for a single oneCard
//		This function also works for a deck of oneCards by:
//			Finding the bottom of cardToLink...
//			...and linking it to the top of existingDeck
oneCard * link_a_card(oneCard * existingDeck, oneCard * cardToLink)
{
	oneCard * lacReturnValue = NULL;
	oneCard * currentCard = NULL;

	if (NULL != existingDeck && NULL != cardToLink)
	{
		lacReturnValue = find_the_first_card(cardToLink);
		assert(NULL != lacReturnValue);

		if (NULL != lacReturnValue)
		{
			// Check if cardToLink is a deck or single card
			if (NULL == lacReturnValue->nextCard)		// Single card
			{
				lacReturnValue->nextCard = existingDeck;
				existingDeck->prevCard = cardToLink;
			}
			else										// cardToLink is actually a deck
			{
				currentCard = (oneCard*)find_the_last_card(cardToLink);
				if (NULL != currentCard)
				{
					currentCard->nextCard = (struct oneCard*)existingDeck;
					existingDeck->prevCard = (struct oneCard*)currentCard;
				}
			}
		}
	}
	else if (NULL == existingDeck && NULL != cardToLink)
	{
		lacReturnValue = cardToLink;
	}

	return lacReturnValue;
}


// Purpose:
//		Remove a card from the deck
// Returns:
//		Pointer to the new head node
//		NULL on error
oneCard * remove_a_card(oneCard * cardToRemove)
{
	oneCard * racReturnValue = NULL;
	oneCard * tempCard = NULL;

	if (NULL != cardToRemove)
	{
		// Connect the previousCard to the nextCard
		tempCard = (oneCard*)cardToRemove->prevCard;
		if (NULL != tempCard)
		{
			tempCard->nextCard = cardToRemove->nextCard;
		}
		tempCard = (oneCard*)cardToRemove->nextCard;
		if (NULL != tempCard)
		{
			tempCard->prevCard = cardToRemove->prevCard;
		}

		// NULL the pointer variables in cardToRemove
		cardToRemove->nextCard = NULL;
		cardToRemove->prevCard = NULL;

		// Find the head node
		tempCard = find_the_first_card(tempCard);

		racReturnValue = tempCard;
	}

	return racReturnValue;
}


// Purpose:
//		Add a card into a deck
// Returns:
//		Pointer to the new destination head node (see: cardToAdd)
//		NULL on error
// Note:
//		cardToAdd goes "on top" of the destination (see: new head node)
//		This function handles all the prevCard and nextCard linkages
oneCard * add_a_card(oneCard * cardToAdd, oneCard * destination)
{
	oneCard * aacReturnValue = NULL;

	if (NULL != cardToAdd)// && NULL != destination) // destination check isn't necessary because destination might be emtpy
	{
		if (NULL == cardToAdd->nextCard && NULL == cardToAdd->prevCard)
		{
			aacReturnValue = cardToAdd;
			aacReturnValue->nextCard = destination;
			if (NULL != destination)
			{
				destination->prevCard = aacReturnValue;
			}
		}	// else, cardToAdd was already part of another linked list
	}

	return aacReturnValue;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// CONSIDER REMOVING THIS FUNCTION ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Purpose:
//		Randomize one card out of the linked list starting at stackOfCards and remove it from the deck
// Returns:
//		Pointer to a card struct
//		NULL on error
// Note:
//		The original pointer to stackOfCards may be modified if the head node is drawn
oneCard * draw_a_card(oneCard ** stackOfCards)
{
	oneCard * dacReturnValue = NULL;
	oneCard * tempCard = NULL;
	int numOfCards = 0;
	int randoSelection = 0;

	if (NULL != stackOfCards)
	{
		if (NULL != *stackOfCards)
		{
			// Find the head node
			dacReturnValue = find_the_first_card(*stackOfCards);

			// Count the cards
			tempCard = dacReturnValue;
			while (NULL != tempCard)
			{
				numOfCards++;
				tempCard = (oneCard*)tempCard->nextCard;
			}
			tempCard = dacReturnValue;		// Reset

			// Randomize a card
			if (numOfCards > 1)
			{
				randoSelection = rando_me(1, numOfCards);
			}
			else
			{
				randoSelection = 1;
			}

			// Find that card
			if (randoSelection >= 1 && randoSelection <= numOfCards)
			{
				numOfCards = 1;

				while (randoSelection != numOfCards)
				{
					if (NULL != tempCard)
					{
						tempCard = (oneCard*)tempCard->nextCard;
						numOfCards++;
					}
				}

				dacReturnValue = tempCard;
			}

			// Remove that card
			if (dacReturnValue == *stackOfCards)
			{
				(*stackOfCards) = (oneCard*)(*stackOfCards)->nextCard;
			}
			if ((*stackOfCards) != remove_a_card(dacReturnValue))
			{
				perror("Something has gone wrong with card removal!\n");
			}
		}
	}

	return dacReturnValue;
}


// Purpose:
//		Find the head node in this double linked list
// Returns:
//		NULL on error
//		Pointer to the head node on success
oneCard * find_the_first_card(oneCard * anyCard)
{
	oneCard * ftfcReturnValue = NULL;

	if (NULL != anyCard)
	{
		ftfcReturnValue = anyCard;

		while (NULL != ftfcReturnValue->prevCard)
		{
			ftfcReturnValue = (oneCard*)ftfcReturnValue->prevCard;
		}
	}

	return ftfcReturnValue;
}


// Purpose:
//		Find the tail node in this double linked list
// Returns:
//		NULL on error
//		Pointer to the head node on success
oneCard * find_the_last_card(oneCard * anyCard)
{
	oneCard * ftlcReturnValue = NULL;

	if (NULL != anyCard)
	{
		ftlcReturnValue = anyCard;

		while (NULL != ftlcReturnValue->nextCard)
		{
			ftlcReturnValue = (oneCard*)ftlcReturnValue->nextCard;
		}
	}

	return ftlcReturnValue;
}


// Purpose:
//		Deal a hand of 7 cards
// Returns:
//		Return a pointer to the hand
//		NULL on error
// Note:
//		The original pointer to the drawPile may be modified if the head node happens to be drawn
oneCard * deal_a_hand(oneCard ** drawPile)
{
	oneCard * dahReturnValue = NULL;
	oneCard * tempCard = NULL;
	int numCardsInHand = 0;
	int i = 0;

	if (NULL != drawPile)
	{
		if (NULL != *drawPile)
		{
			if (7 <= count_the_cards(*drawPile))
			{
				dahReturnValue = draw_a_card(drawPile);
				assert(dahReturnValue);
				numCardsInHand++;

//				for (i = 0; i < 25; i++)		// DEBUGGING
//				for (i = 0; i < 0; i++)		// DEBUGGING
//				for (i = 0; i < 2; i++)		// DEBUGGING
				for (i = 0; i < 6; i++)
				{
					tempCard = draw_a_card(drawPile);
					assert(tempCard);
					dahReturnValue = link_a_card(dahReturnValue, tempCard);
					assert(dahReturnValue);
					numCardsInHand++;
				}
			}
		}
	}

	return dahReturnValue;
}


// Purpose:
//		Count the number of nodes in a oneCard struct
// Returns:
//		Number of cards on success
//		-1 on NULL
//		0 on error
int count_the_cards(oneCard * anyCard)
{
	int ctcReturnValue = -1;
	oneCard * tempCard = NULL;

	if (NULL != anyCard)
	{
		ctcReturnValue = 0;

		// Find the head node
		tempCard = find_the_first_card(anyCard);
		assert(tempCard);

		// Count the cards
		while (NULL != tempCard)
		{
			ctcReturnValue++;
			tempCard = (oneCard*)tempCard->nextCard;
		}
	}

	return ctcReturnValue;
}


// Purpose:
//		Print the cards in a player's doubly linked list
// Returns:
//		None
void print_players_hand(oneCard * anyCard)
{
	// Variables
	int numOfCards = 0;
	char color = 'X';
	oneCard * currentCard = anyCard;
	int i = 0;

	if (NULL != anyCard)
	{
		// 0. Find the first card
		currentCard = find_the_first_card(anyCard);
		assert(currentCard);

		// 1. Count the cards in hand
		numOfCards = count_the_cards(anyCard);

		// 2. Loop through the cards
		//// 2.a. Print row 1
		for (i = 0; i < numOfCards; i++)
		{
			printf("%c%c%c%c%c ", BORDER_UPPER_LEFT, BORDER_HORIZONTAL, BORDER_HORIZONTAL, BORDER_HORIZONTAL, BORDER_UPPER_RIGHT);
		}
		putchar(10);

		//// 2.b. Print row 2
		for (i = 0; i < numOfCards; i++)
		{
			printf("%c%c%c%c%c ", BORDER_VERTICAL, BORDER_SPACE, BORDER_SPACE, BORDER_SPACE, BORDER_VERTICAL);
		}
		putchar(10);

		//// 2.c. Print row 3
		for (i = 0; i < numOfCards; i++)
		{
			if (NULL != currentCard)
			{
				////// 2.c.i. Determine color
				switch (currentCard->color)
				{
				case COLOR_BLACK:
					color = ' ';
					break;
				case COLOR_BLUE:
					color = 'B';
					break;
				case COLOR_GREEN:
					color = 'G';
					break;
				case COLOR_RED:
					color = 'R';
					break;
				case COLOR_YELLOW:
					color = 'Y';
					break;
				default:
					color = 'X';
					break;
				}
			}

			printf("%c%c%c%c%c ", BORDER_VERTICAL, BORDER_SPACE, color, BORDER_SPACE, BORDER_VERTICAL);
			color = 'X'; // Reset to default
			currentCard = (oneCard*)currentCard->nextCard;
		}
		putchar(10);
		// Reset pointer variable to first card
		currentCard = find_the_first_card(anyCard);
		assert(currentCard);

		////// Print row 4
		for (i = 0; i < numOfCards; i++)
		{
			if (NULL != currentCard)
			{
				printf("%c%c%c%c%c ", BORDER_VERTICAL, BORDER_SPACE, currentCard->type, BORDER_SPACE, BORDER_VERTICAL);
			}
			currentCard = (oneCard*)currentCard->nextCard;
		}
		putchar(10);
		// Reset pointer variable to first card
		currentCard = find_the_first_card(anyCard);
		assert(currentCard);

		////// Print row 5
		for (i = 0; i < numOfCards; i++)
		{
			printf("%c%c%c%c%c ", BORDER_VERTICAL, BORDER_SPACE, BORDER_SPACE, BORDER_SPACE, BORDER_VERTICAL);
		}
		putchar(10);

		////// Print row 6
		for (i = 0; i < numOfCards; i++)
		{
			printf("%c%c%c%c%c ", BORDER_LOWER_LEFT, BORDER_HORIZONTAL, BORDER_HORIZONTAL, BORDER_HORIZONTAL, BORDER_LOWER_RIGHT);
		}
		putchar(10);

		////// Print selection row
		for (i = 1; i <= numOfCards; i++)
		{
			printf("  %1d   ", i);
		}
		putchar(10);
	}

	return;
}


// Purpose:
//		Print one card
// Returns:
//		None
void print_one_card(oneCard * oneCard)
{
	char color = ' ';
	char type = 0;

	if (NULL != oneCard)
	{
		switch (oneCard->color)
		{
			case COLOR_BLACK:
				color = ' ';
				break;
			case COLOR_BLUE:
				color = 'B';
				break;
			case COLOR_GREEN:
				color = 'G';
				break;
			case COLOR_RED:
				color = 'R';
				break;
			case COLOR_YELLOW:
				color = 'Y';
				break;
			default:
				color = 'X';
				break;
		}

		type = oneCard->type;
	}

	printf("%c%c%c%c%c\n", BORDER_UPPER_LEFT, BORDER_HORIZONTAL, BORDER_HORIZONTAL, BORDER_HORIZONTAL, BORDER_UPPER_RIGHT);
	printf("%c%c%c%c%c\n", BORDER_VERTICAL, BORDER_SPACE, BORDER_SPACE, BORDER_SPACE, BORDER_VERTICAL);
	printf("%c%c%c%c%c\n", BORDER_VERTICAL, BORDER_SPACE, color, BORDER_SPACE, BORDER_VERTICAL);
	printf("%c%c%c%c%c\n", BORDER_VERTICAL, BORDER_SPACE, type, BORDER_SPACE, BORDER_VERTICAL);
	printf("%c%c%c%c%c\n", BORDER_VERTICAL, BORDER_SPACE, BORDER_SPACE, BORDER_SPACE, BORDER_VERTICAL);
	printf("%c%c%c%c%c\n", BORDER_LOWER_LEFT, BORDER_HORIZONTAL, BORDER_HORIZONTAL, BORDER_HORIZONTAL, BORDER_LOWER_RIGHT);

	return;
}


// Purpose:
//		Move one card
// Arguments:
//		oldLocation - a pointer to the heade node of a linked list of oneCard nodes which represents the "source"
//		newLocation - a pointer to the head node of a linked list of oneCard nodes which represents the "destination"
//		cardNum - the "source" node number to move into the top of the "destination" counting the head node as 0
// Returns:
//		Pointer to the card that was moved
//		NULL on failure
// Notes:
//		This function can be used to:
//			Play a card
//			Draw a card
//			Flip the first card into the discard pile
//		This function handles all the prevCard and nextCard connections
//		This function places the "cardNum" node from oldLocation "on top" of the newLocation (see: new head node)
//		VERY IMPORTANT:  The "cardNum" node is the new head node of newLocation on success
oneCard * move_one_card(oneCard ** oldLocation, oneCard ** newLocation, int cardNum)
{
	oneCard * mocReturnValue = NULL;
	oneCard * tempCard = NULL;
	oneCard * currentNode = NULL;
	int numOfOldLocationCards = 0;		// Necessary to calculate if a moved card creates an empty deck

	int i = 0;

	if (NULL != oldLocation && NULL != newLocation && 0 <= cardNum)
	{
		if (NULL != *oldLocation) // && NULL != newLocation)	// Should not be necessary because the "destination" might be empty
		{
			// Find the top of the deck
			currentNode = find_the_first_card(*oldLocation);	// Should not be necessary

			// Count the cards in the deck
			numOfOldLocationCards = count_the_cards(*oldLocation);

			// Find the pointer to cardNum
			while (NULL != currentNode && i < cardNum)
			{
				currentNode = currentNode->nextCard;
				i++;
			}

			// Set return value
			mocReturnValue = currentNode;

			// Remove cardNum from oldLocation
			tempCard = remove_a_card(currentNode);
			// ...and modify the new source head node
			if (NULL != tempCard || 1 == numOfOldLocationCards)		// Setting *oldLocation to NULL is appropriate if the hand is emptied
			{
				*oldLocation = tempCard;
			}

			// Add cardNum to newLocation
			tempCard = add_a_card(currentNode, *newLocation);
			// ...and modify the new destination head node
			if (NULL != tempCard)
			{
				*newLocation = tempCard;
			}

			//// DEBUGGING
			//if (TYPE_WILD == (*newLocation)->type || TYPE_WILD_DRAW_FOUR == (*newLocation)->type)
			//{
			//	puts("DEBUGGING WILD CARDS!");	// DEBUGGING
			//}
		}
	}

	return mocReturnValue;
}


// Purpose:
//		This function glues together the game play for the user
// Arguments:
// Returns:
//		2 to quit
//		1 if player won
//		0 on success
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		This function:
//			Takes user input
//			Validates that input
//			Appropriately modifies the other decks (see: linked lists)
//			May modify the "head node" pointers for any of these linked lists (see: decks)
int player_menu(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer, int directionOfPlay)
{
	int pmReturnValue = -2;
	int passingUnlocked = 0;	// bool indication a card has been drawn, thus unlocking the "Pass" player option
	char playerChoice = 0;		// The menu selection the user wants to take
	int playerCardChoice = 0;	// The card number the user wants to play
	int validChoice = 0;		// bool indication user has made a valid menu decision
	int numOfCardsInHand = 0;
	int numTries = 0;
	oneCard * currentCard = NULL;	// Current card being operated on
//	oneCard * someOtherCard = NULL;	// Some other card being operated on
	char wildCardColor = 0;			// Stores players choice of color for a Wild card
	int wildCardValue = 0;			// Stores the conversion of the placers Wild card color choice as an int MACRO
	int tempRetVal = 0;				// Used to temporarily store int return values

	// Input Validation
	if (
		NULL != playerHand && 
		NULL != drawPile && 
		NULL != discardPile && 
		NULL != currentPlayer && 
		(PLAY_CLOCKWISE == directionOfPlay || PLAY_COUNTER_CLOCKWISE == directionOfPlay)
		)
	{
		tempRetVal = can_player_play(playerHand, drawPile, discardPile, currentPlayer);
		assert(0 == tempRetVal || 1 == tempRetVal);

		if (1 != tempRetVal)
		{
			pmReturnValue = tempRetVal;
		}
		else
		{
			if (NULL != *drawPile && NULL != *discardPile)
			{
				// Count the player's cards
				numOfCardsInHand = count_the_cards(*playerHand);
				assert(numOfCardsInHand > 0 || NULL == *playerHand);

				// Check for winner
				if (-1 == numOfCardsInHand)
				{
//					printf("\n\nYOU WON!\n\n");
					pmReturnValue = 1;
					return pmReturnValue;
				}

				// Print the game
				print_the_game(*playerHand, *discardPile, currentPlayer, directionOfPlay);

				// Check for "One"
				if (1 == numOfCardsInHand)
				{
					printf("ONE!\n");
				}

				while (0 == validChoice && numTries <= MAX_TRIES)
				{
					//// DEBUGGING
					//if (TYPE_WILD == (*discardPile)->type || TYPE_WILD_DRAW_FOUR == (*discardPile)->type)
					//{
					//	puts("DEBUGGING WILD CARDS!");	// DEBUGGING
					//}

					// Print the menu
					printf("What would you like to do?\n");
					printf("\t(P)lay a card\n");
					if (0 == passingUnlocked)
					{
						printf("\t(D)raw a card\n");
					}
					if (1 == passingUnlocked)
					{
						printf("\t(S)kip\n");
					}
					printf("\t(R)eprint your hand\n");
					printf("\t(Q)uit playing\n");

					// Scan the user's input
	//				clear_stdin();
					playerChoice = getchar();
					clear_stdin();
					playerChoice = toupper(playerChoice);

					// Validate user input
					switch (playerChoice)
					{
					case ('D'):			// Draw
						if (0 == passingUnlocked)
						{
							// Take the top card from the draw pile and add it to the player hand
							currentCard = move_one_card(drawPile, playerHand, 0);
							assert(NULL != currentCard);

							if (NULL != currentCard)
							{
								// Update number of cards in hand (currentPlayer)
								currentPlayer->currentCardCount = count_the_cards(currentPlayer->currentHand);
								assert(currentPlayer->currentCardCount > 0);

								if (currentPlayer->currentCardCount < 1)
								{
									pmReturnValue = -2;
									break;
								}

								// Print the game!
								print_the_game(*playerHand, *discardPile, currentPlayer, directionOfPlay);
							}
							else
							{
								pmReturnValue = -2;
								break;
							}

							passingUnlocked = 1;	// Now that they've drawn a card, they can pass
							validChoice = 0;	// Technically, drawing is valid but we want the menu to reprint
						}
						else
						{
							print_the_game(*playerHand, *discardPile, currentPlayer, directionOfPlay);
							printf("Bad input.\n");
							numTries++;

							if (numTries <= MAX_TRIES)
							{
								printf("Try again.\n");
							}
						}
						break;
					case ('S'):			// Skip
						if (1 == passingUnlocked)
						{
							pmReturnValue = 0;
							validChoice = 1;
						}
						else
						{
							print_the_game(*playerHand, *discardPile, currentPlayer, directionOfPlay);
							printf("Bad input.\n");
							numTries++;

							if (numTries <= MAX_TRIES)
							{
								printf("Try again.\n");
							}
						}
						break;
					case ('R'):			// Reprint player's cards
						print_the_game(*playerHand, *discardPile, currentPlayer, directionOfPlay);
						validChoice = 0;			// Technically valid, but we want the menu of options to print again
						break;
					case ('Q'):			// Quit
						pmReturnValue = 1;
						validChoice = 1;
						break;
					case ('P'):				// Play
											// Further user promptps
						printf("Which card would you like to play?\n");
						// Continue 
						scanf("%d", &playerCardChoice);
						clear_stdin();
//						numTries++;
					// Unlisted Shortcuts
					case('1'):
					case('2'):
					case('3'):
					case('4'):
					case('5'):
					case('6'):
					case('7'):
					case('8'):
					case('9'):
						if ('1' <= playerChoice && '9' >= playerChoice)
						{
							playerCardChoice = (int)playerChoice % 48;
						}

						if (playerCardChoice > 0 && playerCardChoice <= numOfCardsInHand)
						{
							// Check if this is a valid play
							//// Find the card
							currentCard = find_this_card(playerCardChoice - 1, *playerHand);
							assert(NULL != currentCard);

							//// Verify the card is a legal play
							if (1 != is_this_play_valid(currentCard, *discardPile))
							{
								validChoice = 0;
								numTries++;
								print_the_game(*playerHand, *discardPile, currentPlayer, directionOfPlay);
								printf("Illegal play.\n");

								if (numTries <= MAX_TRIES)
								{
									printf("Try again.\n");
								}
								break;
							}
							else
							{
								if (COLOR_BLACK == currentCard->color)
								{
									printf("What color do you choose for the Wild card?  ");
									wildCardColor = getchar();
									clear_stdin();
									wildCardValue = get_val_from_color(wildCardColor);

									//// Verify the color choice is a legal play
									if (COLOR_RED <= wildCardValue && COLOR_BLUE >= wildCardValue)
									{
										currentCard->color = wildCardValue;
									}
									else
									{
										validChoice = 0;
										numTries++;
										print_the_game(*playerHand, *discardPile, currentPlayer, directionOfPlay);
										printf("Illegal play.\n");

										if (numTries <= MAX_TRIES)
										{
											printf("Try again.\n");
										}
										break;
									}
								}
							}

							validChoice = 1;
							pmReturnValue = 0;
							// Actually play the card (remove from hand, add to discard)
							currentCard = move_one_card(playerHand, discardPile, playerCardChoice - 1);
							assert(NULL != currentCard);

							if (NULL != currentCard)
							{
								// Update number of cards in hand (currentPlayer)
								currentPlayer->currentCardCount = count_the_cards(currentPlayer->currentHand);
								assert(currentPlayer->currentCardCount > 0);

								if (currentPlayer->currentCardCount < 1)
								{
									pmReturnValue = -2;
									break;
								}

								// Print the game!
								print_the_game(*playerHand, *discardPile, currentPlayer, directionOfPlay);
							}
							else
							{
								pmReturnValue = -2;
								break;
							}
						}
						else
						{
							print_the_game(*playerHand, *discardPile, currentPlayer, directionOfPlay);
							printf("Bad input.\n");
							numTries++;

							if (numTries <= MAX_TRIES)
							{
								printf("Try again.\n");
							}
						}
						break;
					default:
						print_the_game(*playerHand, *discardPile, currentPlayer, directionOfPlay);
						printf("Bad input.\n");
						numTries++;

						if (numTries <= MAX_TRIES)
						{
							printf("Try again.\n");
						}
						break;
					}
				}
			}
			else
			{
				pmReturnValue = -1;
			}
		}
	}
	else
	{
		pmReturnValue = -1;
	}

	return pmReturnValue;
}


// Purpose:
//		Remove any pesky characters lurking around on the stdin stream
void clear_stdin(void)
{
	char read = 0;
	while (EOF != read && '\n' != read)
	{
		read = getchar();
//		putchar(read); // DEBUGGING
	}

	return;
}


// Purpose:
//		One function to print the current status of the game
// Notes:
//		Prints the player status, discard pile, and player's hand
void print_the_game(oneCard * playerHand, oneCard * discardPile, onePlayer * currentPlayer, int directionOfPlay)
{
	onePlayer * thisPlayer = currentPlayer;
	onePlayer * nextPlayer = NULL;

	// Clear the screen
	CLEAR_SCREEN;

	// Print the draw pile and discard pile
	//puts("Draw Pile");
	//print_one_card(NULL);

	// Print the player status
	if (NULL != thisPlayer)
	{
		puts("Opponent Status");

		thisPlayer = thisPlayer->nextPlayer; // Start with the next player

		while (thisPlayer != currentPlayer && NULL != thisPlayer)
		{
			printf("\tPlayer #%d has %d cards.\n", thisPlayer->playerNumber, thisPlayer->currentCardCount);
			thisPlayer = thisPlayer->nextPlayer;
		}

		if (PLAY_CLOCKWISE == directionOfPlay || PLAY_COUNTER_CLOCKWISE == directionOfPlay)
		{
			printf("\tDirection of play is currently ");

			if (PLAY_CLOCKWISE == directionOfPlay)
			{
				nextPlayer = thisPlayer->nextPlayer;
				printf("clockwise.\n");
			}
			else if (PLAY_COUNTER_CLOCKWISE == directionOfPlay)
			{
				nextPlayer = thisPlayer->prevPlayer;
				printf("counter clockwise.\n");
			}
			printf("\tThe next player is Player #%d.\n", nextPlayer->playerNumber);
		}
	}

	// Print the discard pile
	puts("Discard Pile");
	print_one_card(discardPile);

	// Print the player's hand
	puts("Your Hand");
	print_players_hand(playerHand);

	return;
}


// Purpose:
//		This function determines whether or not a play is valid
// Arguments:
// Returns:
//		1 if TRUE
//		0 if FALSE
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		This function does not make any change to the card or the discard pile
int is_this_play_valid(oneCard * intendedPlay, oneCard * discardPile)
{
	int itpvReturnValue = -1;	// Return value
	oneCard * tmpHand = NULL;	// Used to 'rewind' the player's hand to count Wild cards
//	int cardCount = 0;			// Used to count the number of Wild cards

	if (NULL != intendedPlay && NULL != discardPile)
	{
		itpvReturnValue = -2;

		// Check 1:  Is it the same color?
		if (intendedPlay->color == discardPile->color)
		{
			itpvReturnValue = 1;
		}
		// Check 2:  Is it the same type? 
		else if (intendedPlay->type == discardPile->type)
		{
			itpvReturnValue = 1;
		}
		// Check 3:  Is it a Wild card?
		else if (intendedPlay->type == TYPE_WILD)
		{
			itpvReturnValue = 1;
		}
		// Check 4:  Is it a Wild Draw 4?
		else if (intendedPlay->type == TYPE_WILD_DRAW_FOUR)
		{
			// Does the player hold any Wild cards or valid color choices?
			tmpHand = find_the_first_card(intendedPlay);
			assert(NULL != tmpHand);

			if (NULL != tmpHand)
			{
				// Default response
				itpvReturnValue = 1;

				
				while (NULL != tmpHand)
				{
					// Check for Wild cards
					if (TYPE_WILD == tmpHand->type)
					{
						itpvReturnValue = 0;
						break;
					}

					// Check for a valid color choice
					if (discardPile->color == tmpHand->color)
					{
						itpvReturnValue = 0;
						break;
					}

					tmpHand = tmpHand->nextCard;
				}
			}
			else
			{
				itpvReturnValue = -2;
			}
		}
		else
		{
			itpvReturnValue = 0;
		}
	}

	return itpvReturnValue;
}


// Purpose:
//		Find this card number in this deck
// Returns:
//		Pointer to the card
//		NULL on error
// Notes:
//		Start counting at 0
oneCard * find_this_card(int cardNum, oneCard * haystackDeck)
{
	oneCard * ftcReturnValue = NULL;
	int numOfCardsInDeck = 0;
	int currentCardNum = 0;

	if (NULL != haystackDeck && cardNum >= 0)
	{
		numOfCardsInDeck = count_the_cards(haystackDeck);
		assert(numOfCardsInDeck > 0);

		if (cardNum <= (numOfCardsInDeck - 1))
		{
			ftcReturnValue = find_the_first_card(haystackDeck);
			assert(NULL != ftcReturnValue);

			if (NULL != ftcReturnValue)
			{
				while (currentCardNum != cardNum)
				{
					ftcReturnValue = ftcReturnValue->nextCard;
					currentCardNum++;
				}
			}
		}
	}

	return ftcReturnValue;
}


// Purpose:
//		Find the position number of this card in this deck
// Returns:
//		Position number, starting at 0, on success
//		-1 on NULL
//		-2 if not in the deck
//		-3 on unspecified error
// Notes:
//		Start counting at 0
int find_this_position(oneCard * singleCard, oneCard * haystackDeck)
{
	int ftpReturnValue = -1;
	oneCard * currentCard = NULL;

	if (NULL != singleCard && NULL != haystackDeck)
	{
		currentCard = find_the_first_card(haystackDeck);
		assert(NULL != currentCard);

		if (NULL != currentCard)
		{
			ftpReturnValue = 0;

			while (currentCard != singleCard && NULL != currentCard)
			{
				currentCard = currentCard->nextCard;
				ftpReturnValue++;
			}

			if (NULL == currentCard)		// Didn't find it
			{
				ftpReturnValue = -2;
			}
		}
		else
		{
			ftpReturnValue = -3;
		}
	}

	return ftpReturnValue;
}


// Purpose:
//		Randomize the order of a deck of cards
// Returns:
//		Pointer to the new head node
//		NULL on error
// Notes:
//		The head node will likely change when the deck is shuffled
oneCard * shuffle_a_deck(oneCard * newDeck)
{
	oneCard * sadReturnValue = NULL;
	oneCard * currentCard = NULL;
	int numOfCards = 0;
	int cardToMove = 0;

	if (NULL != newDeck)
	{
		// Count the deck
		numOfCards = count_the_cards(newDeck) - 1;

		// Move one card at a time
		while (numOfCards >= 0)
		{
			if (numOfCards > 0)
			{
				cardToMove = rando_me(0, numOfCards);
			}
			else
			{
				cardToMove = 0;
			}
			assert(cardToMove >= 0 && cardToMove <= numOfCards);

			if (cardToMove >= 0 && cardToMove <= numOfCards)
			{
				currentCard = move_one_card(&newDeck, &sadReturnValue, cardToMove);
				assert(NULL != currentCard);

				if (NULL != currentCard)
				{
					numOfCards--;

					// Reset any Wild cards to the proper color
					if (TYPE_WILD == currentCard->type || TYPE_WILD_DRAW_FOUR == currentCard->type)
					{
						currentCard->color = COLOR_BLACK;
					}

					// Reset the processed member to 0
					currentCard->processed = 0;
				}
				else
				{
					break;
				}
			}
			else
			{
				puts("Error!");		// DEBUGGING
			}
		}
	}

	return sadReturnValue;
}


// Purpose:
//		Convert a color char to color macro value
// Returns:
//		COLOR_RED on 'R'
//		COLOR_YELLOW on 'Y'
//		COLOR_GREEN on 'G'
//		COLOR_BLUE on 'B'
//		COLOR_BLACK on 'W'
//		0 on failure
// Notes:
//		Char values for colors are case insensitive
int get_val_from_color(char colorLetter)
{
	int gvfcReturnValue = 0;
	char caseInsensitiveColorLetter = toupper(colorLetter);

	switch (caseInsensitiveColorLetter)
	{
	case ('R'):
		gvfcReturnValue = COLOR_RED;
		break;
	case('Y'):
		gvfcReturnValue = COLOR_YELLOW;
		break;
	case('G'):
		gvfcReturnValue = COLOR_GREEN;
		break;
	case('B'):
		gvfcReturnValue = COLOR_BLUE;
		break;
	case('W'):
		gvfcReturnValue = COLOR_BLACK;
		break;
	default:
		gvfcReturnValue = 0;
		break;
	}

	return gvfcReturnValue;
}


// Purpose:
//		Sets up the entire game prior to the start
// Returns:
//		0 on success
//		-1 on NULL pointer
//		-2 on unrealistic number of players
//		-3 on unspecified error
//	Notes:
//		This function:
//			Builds a deck of appropriate size
//			Shuffles the deck
//			Deals cards to each of the players
//			Flips a card onto the discard pile (and prompts the user for a color if it was Wild)
int setup_game(oneCard ** drawPile, oneCard ** discardPile, onePlayer * playerNode)
{
	int sgReturnValue = -3;			// Current return value
	oneCard * tmpDeck = NULL;		// Current deck of oneCards to operate on
	oneCard * tmpRetVal = NULL;		// Holds return value for the purposes of validation
	onePlayer * currentPlayer = NULL;	// Current player node being operated on
	int i = 0;						// Iterating variable
	char wildCardColor = 0;			// Holds a char representation of a color
	int wildCardValue = 0;			// Holds an int representation of a color
	int numTries = 0;				// Used to test the upper limit of retries (see: while loops)
	int numOfPlayers = 0;			// Counts the number of onePlayer nodes in the circular double linked list

	if (NULL == drawPile || NULL == discardPile || NULL == playerNode)
	{
		sgReturnValue = -1;
	}
	else
	{
		// Builds a deck of appropriate size
		/* NOT ANY MORE... REFACTORED TO PLAY MULTIPLE GAMES */
		//// Is something already there?  If so, tear it down and start fresh.
		//if (NULL != *drawPile)
		//{
		//	destroy_a_deck(drawPile);
		//}

		// Clear the screen
		CLEAR_SCREEN;

		// Counts the number of players
		currentPlayer = playerNode;

		do
		{
			numOfPlayers++;
			currentPlayer = currentPlayer->nextPlayer;
		} while (currentPlayer != playerNode && NULL != currentPlayer);
		assert(1 < numOfPlayers);

		if (numOfPlayers < 2 || numOfPlayers > MAX_NUM_PLAYERS)
		{
			sgReturnValue = -2;
			return sgReturnValue;
		}

		//// Ensure there's enough cards to deal 7 cards to each player and flip a discard
		//// It's fine if the draw pile is empty because that case will be handled elsewhere
		while ((numOfPlayers * 7) > (count_the_cards(*drawPile) - 1))
		{
			tmpDeck = build_a_new_deck();								// Get another deck of cards
			assert(NULL != tmpDeck);

			if (NULL != tmpDeck)										// Add that new deck to the shoe
			{
				tmpRetVal = link_a_card(*drawPile, tmpDeck);
				assert(tmpRetVal == tmpDeck);							// Link a card places the new card/deck on top

				if (tmpRetVal == tmpDeck)
				{
					tmpDeck = NULL;										// This variable is now part of the drawPile
					*drawPile = tmpRetVal;								// This variable now becomes the drawPile
					tmpRetVal = NULL;									// Clear this temp variable to avoid "accidents"
				}
			}
		}

		// Shuffles the deck
		tmpRetVal = shuffle_a_deck(*drawPile);
		assert(NULL != tmpRetVal);

		if (NULL != tmpRetVal)
		{
			*drawPile = tmpRetVal;
			tmpRetVal = NULL;										// Clear this temp variable to avoid "accidents"
		}

		// Deals cards to each of the players
		currentPlayer = playerNode;

		do
		{
			currentPlayer->currentHand = deal_a_hand(drawPile);
			assert(NULL != currentPlayer->currentHand);

			if (NULL != currentPlayer->currentHand)
			{
				currentPlayer->currentCardCount = count_the_cards(currentPlayer->currentHand);
				assert(7 == currentPlayer->currentCardCount);

				if (7 != currentPlayer->currentCardCount)
				{
					sgReturnValue = -3;
					return sgReturnValue;
				}
			}
			else
			{
				sgReturnValue = -3;
				return sgReturnValue;
			}

			currentPlayer = currentPlayer->nextPlayer;
		} while (currentPlayer != playerNode && NULL != currentPlayer);

		// Flips a card onto the discard pile (and prompts the user for a color if it was Wild)
		tmpRetVal = move_one_card(drawPile, discardPile, 0);
		assert(NULL != tmpRetVal);

		if (NULL != tmpRetVal)
		{
			// Check for a Wild card
			if (TYPE_WILD == tmpRetVal->type || TYPE_WILD_DRAW_FOUR == tmpRetVal->type)
			{
				while (0 == wildCardValue && numTries <= STOP_LOOPING)
				{
					// Prompt user for color of the wild card
					printf("What color do you choose for the Wild card?  ");
#ifdef AUTOMATE_PLAY
					wildCardValue = rando_me(COLOR_RED, COLOR_BLUE);

					if (wildCardValue < COLOR_RED || wildCardValue > COLOR_BLUE)
					{
						wildCardValue = COLOR_BLUE;
					}
#else
					wildCardColor = getchar();
					clear_stdin();
					wildCardValue = get_val_from_color(wildCardColor);
#endif // AUTOMATE_PLAY



					//// Verify the color choice is a legal play
					if (COLOR_RED <= wildCardValue && COLOR_BLUE >= wildCardValue)
					{
						tmpRetVal->color = wildCardValue;
					}
					else
					{
						numTries++;
						printf("Illegal choice.\n");

						if (numTries <= STOP_LOOPING)
						{
							printf("Try again.\n");
						}
					}
				}
			}

			if (numTries < STOP_LOOPING)
			{
				sgReturnValue = 0;
			}
		}
	}

	return sgReturnValue;
}


// Purpose:
//		Provide the point value of a single card
// Returns:
//		Point value of the card on success
//		-1 on NULL pointer
//		-2 on unspecified error
int card_value(oneCard * singleCard)
{
	int cvReturnValue = -1;

	if (NULL != singleCard)
	{
		if (singleCard->type >= TYPE_ZERO && singleCard->type <= TYPE_NINE)
		{
			cvReturnValue = (int)singleCard->type % 48;
		}
		else if (
			singleCard->type == TYPE_DRAW_TWO || 
			singleCard->type == TYPE_REVERSE || 
			singleCard->type == TYPE_SKIP)
		{
			cvReturnValue = 20;
		}
		else if (singleCard->type == TYPE_WILD || singleCard->type == TYPE_WILD_DRAW_FOUR)
		{
			cvReturnValue = 50;
		}
		else
		{
			cvReturnValue = -2;		// Unknown card type
		}
	}

	return cvReturnValue;
}


// Purpose:
//		Provide the point value for an entire deck (see: player's hand)
// Returns:
//		Point value total on success
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		Calls card_value() under the hood
int hand_value(oneCard * headNode)
{
	int hvReturnValue = -1;
	int tempRetVal = 0;
	oneCard * currentCard = NULL;

	if (NULL != headNode)
	{
		hvReturnValue = 0;

		currentCard = find_the_first_card(headNode);		// Rewind the hand
		assert(NULL != currentCard);

		if (NULL == currentCard)
		{
			hvReturnValue = -2;
		}
		else
		{
			while (NULL != currentCard)
			{
				tempRetVal = card_value(currentCard);

				if (tempRetVal >= 0)
				{
					hvReturnValue += tempRetVal;
					currentCard = currentCard->nextCard;
				}
				else
				{
					hvReturnValue = -2;				// Unspecified error
					break;
				}
			}
		}
	}

	return hvReturnValue;
}


// Purpose:
//		Shuffle the discard pile into the draw pile, leaving one card in the discard pile
// Returns:
//		0 on success
//		-1 on NULL pointer
//		-2 for an unspecified error
// Notes:
//		The drawPile head node will likely change
//		The discardPile head node should not change
int shuffle_the_discards_in(oneCard ** drawPile, oneCard ** discardPile)
{
	int stdiReturnValue = -1;
	int numOfCardsInHand = 0;
	oneCard * currentCard = NULL;
	oneCard * tmpCard = NULL;

	if (NULL != drawPile && NULL != discardPile)
	{
		numOfCardsInHand = count_the_cards(*discardPile);

		// Detemine if drawPile is empty or not
		if (NULL == *drawPile)					// No cards to draw
		{
			// Reshuffle the discard pile into the draw pile
			if (numOfCardsInHand > 1)			// Time to reshuffle the discards in
			{
				// Shuffle the discards into the draw pile
				//// Ensure the discard pile is "rewound"
				currentCard = find_the_first_card(*discardPile);
				assert(NULL != currentCard);

				if (NULL != currentCard)
				{
					//// Assing the 2nd card in the discards
					*drawPile = currentCard->nextCard;		// The draw pile now holds a pointer to the remainder of the discards
					(*drawPile)->prevCard = NULL;			// The top of the draw pile no longer points to anything else (see: prevCard)
					currentCard->nextCard = NULL;			// Sever the connection between the top of the discardPile and the remaining cards

					//// Ensure the discard pile has the right oneCard pointer
					if (currentCard != *discardPile)
					{
						*discardPile = currentCard;
					}
					currentCard = NULL;						// No longer using this pointer

					//// Shuffle the draw pile
					currentCard = shuffle_a_deck(*drawPile);
					assert(NULL != currentCard);

					if (NULL != currentCard)
					{
						*drawPile = currentCard;
						stdiReturnValue = 0;
					}
					else
					{
						stdiReturnValue = -2;
					}
				}
				else
				{
					stdiReturnValue = -2;
				}
			}
			else if (1 == numOfCardsInHand)		// Time to add another deck to the shoe and shuffle
			{
				// Add another deck to the shoe and reshuffle it
				currentCard = build_a_new_deck();
				assert(NULL != currentCard);

				if (NULL != currentCard)
				{
					*drawPile = currentCard;
					currentCard = shuffle_a_deck(*drawPile);
					assert(NULL != currentCard);

					if (NULL != currentCard)
					{
						*drawPile = currentCard;
						stdiReturnValue = 0;
					}
					else
					{
						stdiReturnValue = -2;
					}
				}
				else
				{
					stdiReturnValue = -2;
				}
			}
			else								// Something went wrong with the card count
			{
				stdiReturnValue = -2;
			}
		}
		else									// Add more cards to the draw pile(?)
		{
			if (numOfCardsInHand > 1)
			{
				// Shuffle the discards into the draw pile
				//// Ensure the discard pile is "rewound"
				currentCard = find_the_first_card(*discardPile);
				assert(NULL != currentCard);

				if (NULL != currentCard)
				{
					// Verify the discard pile is pointing at the top card
					if (currentCard != *discardPile)
					{
						//// Ensure the discard pile has the right oneCard pointer
						*discardPile = currentCard;
					}
					currentCard = NULL;						// No longer using this pointer

					// Find the end of the draw pile
					currentCard = find_the_last_card(*drawPile);
					assert(NULL != currentCard);

					if (NULL != currentCard)
					{
						//// Assign the 2nd card in the discards to the end of the draw pile
						tmpCard = (*discardPile)->nextCard;
						//// Current Card now points to the end of the draw pile
						//// Temp Card now points to the 2nd card in the discard pile
						currentCard->nextCard = tmpCard;				// The bottom of the draw pile now holds a pointer to the remainder of the discards
						tmpCard->prevCard = currentCard;				// The remainder of the discards now point to the bottom of the draw pile
						(*discardPile)->nextCard = NULL;				// Sever the connection between the top of the discardPile and the remaining cards

						//// Shuffle the draw pile
						currentCard = shuffle_a_deck(*drawPile);
						assert(NULL != currentCard);

						if (NULL != currentCard)
						{
							*drawPile = currentCard;
							stdiReturnValue = 0;
						}
						else
						{
							stdiReturnValue = -2;
						}
					}
				}
				else
				{
					stdiReturnValue = -2;
				}
			}
			else if (1 == numOfCardsInHand)		// Time to add another deck to the shoe and shuffle
			{
				// Add another deck to the shoe and reshuffle it
				currentCard = build_a_new_deck();
				assert(NULL != currentCard);

				if (NULL != currentCard)
				{
					*drawPile = currentCard;
					currentCard = shuffle_a_deck(*drawPile);
					assert(NULL != currentCard);

					if (NULL != currentCard)
					{
						*drawPile = currentCard;
						stdiReturnValue = 0;
					}
					else
					{
						stdiReturnValue = -2;
					}
				}
				else
				{
					stdiReturnValue = -2;
				}
			}
			else								// Something went wrong with the card count
			{
				stdiReturnValue = -2;
			}
		}
	}

	return stdiReturnValue;
}


// Purpose:
//		One-stop-function for player_menu() and AI_algorithms() to respond to certain cards
// Returns:
//		1 if the player can take an action (e.g., play, draw, pass)
//		0 if the player can not take an action (e.g., skipped, draw two, draw four)
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		Verifies drawPile has cards.  If it doesn't, shuffles the discards in.
//		On a skip, top discard is modified to reflect that someone has already been skipped
//		On a draw two, top discard is modified to reflect that someone has already drawn the cards
//		On a draw four, top discard is modified to reflect that someone has already drawn the cards
int can_player_play(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer)
{
	int cppReturnValue = -1;
	int tempRetVal1 = 0;			// Used to temporarily hold int return values
	int i = 0;						// Iterating variable
	oneCard * currentCard = NULL;	// Temp variable to store one card

	if (NULL != playerHand && NULL != drawPile && NULL != discardPile && NULL != currentPlayer)
	{
		// Verify there's a draw pile
		tempRetVal1 = count_the_cards(*drawPile);

		// If not, shuffle the discards in
		if (tempRetVal1 < 5)  // Max cards that could be drawn are 4 so we must leave one
		{
			tempRetVal1 = shuffle_the_discards_in(drawPile, discardPile);
			assert(0 == tempRetVal1);

			if (0 != tempRetVal1)
			{
				cppReturnValue = -2;
				return cppReturnValue;
			}
		}

		if (NULL != *playerHand && NULL != *drawPile && NULL != *discardPile)
		{
			// Check Skips
			if (TYPE_SKIP == (*discardPile)->type && 0 == (*discardPile)->processed)
			{
				(*discardPile)->processed = 1;
				cppReturnValue = 0;
				printf("Player #%d got skipped\n", currentPlayer->playerNumber);
				PAUSE_OUTPUT(NUM_SECONDS_TO_PAUSE);
			}
			// Check Draw Twos
			else if (TYPE_DRAW_TWO == (*discardPile)->type && 0 == (*discardPile)->processed)
			{
				cppReturnValue = 0;

				// Draw two cards
				for (i = 0; i < 2; i++)
				{
					currentCard = move_one_card(drawPile, playerHand, 0);
					assert(NULL != currentCard);

					if (NULL == currentCard)
					{
						cppReturnValue = -2;
						break;
					}
				}

				(*discardPile)->processed = 1;

				if (0 == cppReturnValue)
				{
					printf("Player #%d just drew two cards\n", currentPlayer->playerNumber);
					PAUSE_OUTPUT(NUM_SECONDS_TO_PAUSE);
				}
			}
			// Check Wild Draw Fours
			else if (TYPE_WILD_DRAW_FOUR == (*discardPile)->type && 0 == (*discardPile)->processed)
			{
				cppReturnValue = 0;

				// Draw four cards
				for (i = 0; i < 4; i++)
				{
					currentCard = move_one_card(drawPile, playerHand, 0);
					assert(NULL != currentCard);

					if (NULL == currentCard)
					{
						cppReturnValue = -2;
						break;
					}
				}

				(*discardPile)->processed = 1;

				if (0 == cppReturnValue)
				{
					printf("Player #%d just drew four cards\n", currentPlayer->playerNumber);
					PAUSE_OUTPUT(NUM_SECONDS_TO_PAUSE);
				}
			}
			else
			{
				cppReturnValue = 1;
			}

			// Update number of cards in hand (currentPlayer)
			tempRetVal1 = count_the_cards(currentPlayer->currentHand);
			assert(tempRetVal1 > 0);

			if (tempRetVal1 > 0)
			{
				currentPlayer->currentCardCount = tempRetVal1;
			}
			else
			{
				cppReturnValue = -2;
			}
		}
		else
		{
			cppReturnValue = -1;
		}		
	}
	else
	{
		cppReturnValue = -1;
	}

	return cppReturnValue;
}


// Purpose:
//		Continue playing another game with the same players, deck, and score
// Returns:
//		0 on success
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		Player hands are moved into the drawPile
//		Discards are moved into the drawPile
//		No shuffling is done because setup_game() takes care of that
int reset_game(oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer)
{
	int rgReturnValue = -1;
	onePlayer * thisPlayer = currentPlayer;
	oneCard * currentDeck = NULL;
	oneCard * tmpDeckRetVal = NULL;
	int tmpRetVal = 0;

	if (NULL != drawPile && NULL != discardPile && NULL != currentPlayer)
	{
		rgReturnValue = -2;		// Default

		// Player hands are moved into the drawPile
		do
		{
			currentDeck = thisPlayer->currentHand;

			// Link the bottom of the current player's deck to the top of the draw pile
			if (NULL != currentDeck)			// This must be the previous winner
			{
				tmpDeckRetVal = link_a_card(*drawPile, currentDeck);
				assert(NULL != tmpDeckRetVal);
				assert(tmpDeckRetVal == currentDeck);

				if (NULL != tmpDeckRetVal)
				{
					*drawPile = tmpDeckRetVal;
				}
				else
				{
	//				rgReturnValue = -2;
					return rgReturnValue;
				}
			}

			// Zeroize the current player's hand
			thisPlayer->currentHand = NULL;
			currentDeck = NULL;
			tmpDeckRetVal = NULL;

			// Proceed to the next player
			thisPlayer = thisPlayer->nextPlayer;
		} while (thisPlayer != currentPlayer && NULL != thisPlayer);

		// Discards are moved into the drawPile
		if (NULL != *discardPile)
		{
			tmpDeckRetVal = link_a_card(*drawPile, *discardPile);
			assert(NULL != tmpDeckRetVal);
			assert(tmpDeckRetVal == *discardPile);

			if (NULL != tmpDeckRetVal)
			{
				*drawPile = tmpDeckRetVal;

				// Zeroize the discard pile
				*discardPile = NULL;
				tmpDeckRetVal = NULL;
			}
			else
			{
//				rgReturnValue = -2;
				return rgReturnValue;
			}
		}

		// Verify work
		rgReturnValue = 0;
		thisPlayer = currentPlayer;

		//// Discard pile should be empty
		if (NULL != *discardPile)
		{
			rgReturnValue = -2;
		}

		//// Each player's hand should be empty
		if (0 == rgReturnValue)
		{
			do
			{
				if (NULL != thisPlayer->currentHand)
				{
					rgReturnValue = -2;
					break;
				}
				thisPlayer = thisPlayer->nextPlayer;
			} while (thisPlayer != currentPlayer && NULL != thisPlayer);
		}

		//// Draw pile should have a normal amount of cards
		if (0 == rgReturnValue && NULL != *drawPile)
		{
			tmpRetVal = count_the_cards(*drawPile);
			assert(0 == (tmpRetVal % 108));

			if (0 != (tmpRetVal % 108))
			{
				rgReturnValue = -2;
			}
		}
	}

	return rgReturnValue;
}


// Purpose:
//		Update the score for the winner
// Returns:
//		1 on success but a player's score will overflow the data type
//		0 on success
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		Will iterate through the circular doubly linked list of players, reading their hand totals...
//			...and updating their score.
//		Call this function after someone has won
//		Calls hand_value() under the hood
int score_players_hands(onePlayer * theWinner)
{
	int sphReturnValue = -1;
	int tempRetVal = 0;
	onePlayer * thisPlayer = theWinner;

	if (NULL != theWinner)
	{
		if (NULL == theWinner->currentHand)
		{
			do
			{
				if (NULL != thisPlayer->currentHand)
				{
					tempRetVal = hand_value(thisPlayer->currentHand);
					assert(tempRetVal >= 0);

					if (tempRetVal >= 0)
					{
						theWinner->currentScore += tempRetVal;
					}
					else
					{
						sphReturnValue = -2;
					}
				}

				thisPlayer = thisPlayer->nextPlayer;
			} while (thisPlayer != theWinner && NULL != thisPlayer);

			if (-1 == sphReturnValue)
			{
				sphReturnValue = 0;

				// Print the results
				thisPlayer = theWinner;
				puts("\n\n");

				do
				{
					printf("Player #%d has a score of %d.\n", thisPlayer->playerNumber, thisPlayer->currentScore);

					// While we're at it, ensure no one is about to overflow the score
					if (thisPlayer->currentScore > MAX_SCORE)
					{
						sphReturnValue = 1;
					}

					thisPlayer = thisPlayer->nextPlayer;
				} while (thisPlayer != theWinner && NULL != thisPlayer);

				puts("\n\n");
			}
		}
		else
		{
			sphReturnValue = -2;
		}
	}

	return sphReturnValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// ARTIFICIAL INTELLIGENCE ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Purpose:
//		Dynamically allocates a NULL-terminated array of oneCard pointers that contains pointers to valid plays
// Returns:
//		A dynamically allocated, NULL-terminated, array of oneCard pointers on success
//		NULL if there's no valid play
//		NULL on error
// Notes:
//		It is the responsibility of the calling function to free the returned array
oneCard ** AI_valid_choices(oneCard * playersHand, oneCard * discardPile)
{
	oneCard ** AIvcReturnValue = NULL;
	oneCard * currentCard = playersHand;
	int numOfCards = 0;
	int tempRetVal = 0;

	if (NULL != playersHand && NULL != discardPile)
	{
		// Count the cards
		numOfCards = count_the_cards(playersHand);
		//if (1 == numOfCards)			// DEBUGGING
		//{
		//	puts("DEBUGGING AI_valid_choices()!");
		//}
		assert(numOfCards > 0);

		// Allocate an array + 1 (NULL terminated)
		AIvcReturnValue = (oneCard**)calloc(numOfCards + 1, sizeof(oneCard*));
		assert(NULL != AIvcReturnValue);

		if (numOfCards > 0 && NULL != AIvcReturnValue)
		{
			numOfCards = 0;						// Reusing variable as array offset

			while (NULL != currentCard)
			{
				tempRetVal = is_this_play_valid(currentCard, discardPile);

				if (1 == tempRetVal)
				{
					(*(AIvcReturnValue + numOfCards)) = currentCard;
					numOfCards++;
				}

				currentCard = currentCard->nextCard;
			}

			if (NULL != AIvcReturnValue)
			{
				if (NULL == *AIvcReturnValue)		// No valid choices were found
				{
					free(AIvcReturnValue);
					AIvcReturnValue = NULL;
				}
			}
		}
		else
		{
			if (NULL != AIvcReturnValue)
			{
				free(AIvcReturnValue);
				AIvcReturnValue = NULL;
			}
		}
	}

	return AIvcReturnValue;
}


// Purpose:
//		This algorithm replicates a get-rid-of-points strategy
// Returns:
//		1 if player won
//		0 on success
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		This function:
//			Attempts to play the highest value card in the hand
//			Appropriately modifies the other decks (see: linked lists)
//			May modify the "head node" pointers for any of these linked lists (see: decks)
int AI_algorithm_points(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer)
{
	int AIapReturnValue = -1;
	int tempRetVal1 = 0;
	int tempRetVal2 = 0;
	oneCard ** validChoiceArray = NULL;
	oneCard * tmpCard1 = NULL;
	oneCard * tmpCard2 = NULL;
	int numCardsInHand = 0;
	int colorValueCounts[6] = { 0 };		// This array stores value counts per color to aid in choosing the color of a wild card
	int colorChoice = 0;					// This stores the current front runner for the Wild card color selection
	int i = 0;								// Iterating variable

	if (NULL != playerHand && NULL != drawPile && NULL != discardPile)
	{
		tempRetVal1 = can_player_play(playerHand, drawPile, discardPile, currentPlayer);
		assert(0 == tempRetVal1 || 1 == tempRetVal1);

		if (1 != tempRetVal1)
		{
			AIapReturnValue = tempRetVal1;
		}
		else
		{
			if (NULL != *playerHand && NULL != *discardPile)		// ...then let's play.
			{
				// Count the players hand
				numCardsInHand = count_the_cards(*playerHand);
				assert(numCardsInHand > 0);

				if (numCardsInHand > 0)
				{
					// Determine available plays
					validChoiceArray = AI_valid_choices(*playerHand, *discardPile);

					if (NULL == validChoiceArray)						// No available play.  Must draw.
					{
						// Draw a card
						tmpCard1 = move_one_card(drawPile, playerHand, 0);
						assert(NULL != tmpCard1);

						if (NULL != tmpCard1)
						{
							//if (TYPE_WILD == AIaaTempCard1->type || TYPE_WILD_DRAW_FOUR == AIaaTempCard1->type)
							//{
							//	puts("DEBUGGING WILD CARDS!");	// DEBUGGING
							//}

							// Can I play that card?
							tempRetVal1 = is_this_play_valid(tmpCard1, *discardPile);
							assert(tempRetVal1 >= 0);

							// I can play it
							if (1 == tempRetVal1)
							{
								tempRetVal1 = find_this_position(tmpCard1, *playerHand);
								assert(tempRetVal1 >= 0);

								if (tempRetVal1 >= 0)
								{
									tmpCard2 = move_one_card(playerHand, discardPile, tempRetVal1);
									assert(NULL != tmpCard2);
									assert(tmpCard1 == tmpCard2);

									if (NULL == tmpCard2 || tmpCard1 != tmpCard2)
									{
										AIapReturnValue = -2;
									}
									else
									{
										if (NULL == *playerHand)
										{
											AIapReturnValue = 1;
										}
										else
										{
											AIapReturnValue = 0;
										}

										// Check for a Wild card
										if (TYPE_WILD == tmpCard2->type || TYPE_WILD_DRAW_FOUR == tmpCard2->type)
										{
											//// Choose a color for the Wild card
											// TODO:  Write an algorithm to determine which color...
											//			...in the player's hand contains the most points.
											//			For now, we're hard-coding "Green"
//											AIaaTempCard2->color = COLOR_GREEN;

											// Find the begging of the player's hand
											tmpCard1 = find_the_first_card(*playerHand);
											assert(NULL != tmpCard1);

											// Count the values while iterating through the hand
											while (NULL != tmpCard1)
											{
												colorValueCounts[tmpCard1->color] += card_value(tmpCard1);
												tmpCard1 = tmpCard1->nextCard;
											}

											// Find the largest value
											colorChoice = rando_me(1, 5);		// Starting point
											assert(colorChoice >= 1 && colorChoice <= 5);

											if (colorChoice < 1 || colorChoice > 5)
											{
												colorChoice = 1;
											}

											for (i = 1; i < 5; i++)
											{
												if (colorValueCounts[i] > colorValueCounts[colorChoice])
												{
													colorChoice = i;
												}
											}

											// Set the color
											tmpCard2->color = colorChoice;
										}
									}
								}
							}
							// I can't play it
							else
							{
								AIapReturnValue = 0;
							}
						}
						else
						{
							AIapReturnValue = -2;
						}
					}
					else		// We have some choices
					{
						// Reset AIaaNumCardsInHand to use as an iterator
						numCardsInHand = 0;					// Now, an interator

						// AIaaTempCard1 == "current card"
						tmpCard1 = (*(validChoiceArray + numCardsInHand));

						// AIaaTempCard2 == "highest value card"
						tmpCard2 = tmpCard1;
						tempRetVal2 = card_value(tmpCard2);
						//if (AIaaTempRetVal2 < 0)			// DEBUGGING
						//{
						//	puts("DEBUGGING!");
						//}
						assert(tempRetVal2 >= 0);

						while (NULL != tmpCard1)
						{
							numCardsInHand++;
							tmpCard1 = (*(validChoiceArray + numCardsInHand));

							tempRetVal1 = card_value(tmpCard1);

							// Reassign the higher value card
							if (tempRetVal1 > tempRetVal2)
							{
								tmpCard2 = tmpCard1;
								tempRetVal2 = tempRetVal1;
							}
							else if (tempRetVal1 == tempRetVal2)
							{
								// TODO:  Decision on how to break ties.  
								//			Maybe, sum the values for each color
								//			Maybe, choose the most frequent color
								//			Maybe, don't worry about it
							}
						}

						// Free the valid choice array... we're done with it
						if (NULL != validChoiceArray)
						{
							free(validChoiceArray);
						}

						// AIaaTempCard2 now contains a pointer to the appropriate "play"
						//// Convert AIaaTempCard2 to a hand position
						tempRetVal2 = find_this_position(tmpCard2, *playerHand);
						assert(tempRetVal2 >= 0);

						if (tempRetVal2 >= 0)
						{
							tmpCard1 = move_one_card(playerHand, discardPile, tempRetVal2);
							assert(NULL != tmpCard1);
							assert(tmpCard1 == tmpCard2);

							if (NULL != tmpCard1 && tmpCard1 == tmpCard2)
							{
								if (NULL == *playerHand)
								{
									AIapReturnValue = 1;
								}
								else
								{
									AIapReturnValue = 0;
								}

								// Check for a Wild card
								if (TYPE_WILD == tmpCard2->type || TYPE_WILD_DRAW_FOUR == tmpCard2->type)
								{
									//// Choose a color for the Wild card
									// TODO:  Write an algorithm to determine which color...
									//			...in the player's hand contains the most points.
									//			For now, we're hard-coding "Green"
//									AIaaTempCard2->color = COLOR_GREEN;

									// Find the begging of the player's hand
									tmpCard1 = find_the_first_card(*playerHand);
									assert(NULL != tmpCard1);

									// Count the values while iterating through the hand
									while (NULL != tmpCard1)
									{
										colorValueCounts[tmpCard1->color] += card_value(tmpCard1);
										tmpCard1 = tmpCard1->nextCard;
									}

									// Find the largest value
									colorChoice = rando_me(1, 5);		// Starting point
									assert(colorChoice >= 1 && colorChoice <= 5);

									if (colorChoice < 1 || colorChoice > 5)
									{
										colorChoice = 1;
									}

									for (i = 1; i < 5; i++)
									{
										if (colorValueCounts[i] > colorValueCounts[colorChoice])
										{
											colorChoice = i;
										}
									}

									// Set the color
									tmpCard2->color = colorChoice;
								}
							}
							else
							{
								AIapReturnValue = -2;
							}
						}
						else
						{
							AIapReturnValue = -2;
						}
					}
				}
				else			// Something is wrong with the hand
				{
					AIapReturnValue = -2;
				}
			}
			else				// Either missing a hand or a discard pile
			{
				AIapReturnValue = -1;
			}
		}
	}

	// Update card count
	if (NULL != playerHand && NULL != currentPlayer)
	{
		if (NULL != *playerHand)
		{
			tempRetVal1 = count_the_cards(*playerHand);
			assert(tempRetVal1 > 0);

			if (tempRetVal1 > 0)
			{
				currentPlayer->currentCardCount = tempRetVal1;
			}
		}
		
	}

	return AIapReturnValue;
}


// Purpose:
//		This algorithm randomizes a legal card to play
// Returns:
//		1 if player won
//		0 on success
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		This function:
//			Attempts to play the highest value card in the hand
//			Appropriately modifies the other decks (see: linked lists)
//			May modify the "head node" pointers for any of these linked lists (see: decks)
int AI_algorithm_random(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer)
{
	int AIarReturnValue = -1;
	int AIarTempRetVal1 = 0;
	int AIarTempRetVal2 = 0;
	oneCard ** AIarValidChoiceArray = NULL;
	oneCard * AIarTempCard1 = NULL;
	oneCard * AIarTempCard2 = NULL;
	int AIarNumCardsInHand = 0;
	int AIarColorChoice = 0;					// This stores the current front runner for the Wild card color selection
	int i = 0;								// Iterating variable

	if (NULL != playerHand && NULL != drawPile && NULL != discardPile)
	{
		AIarTempRetVal1 = can_player_play(playerHand, drawPile, discardPile, currentPlayer);
		assert(0 == AIarTempRetVal1 || 1 == AIarTempRetVal1);

		if (1 != AIarTempRetVal1)
		{
			AIarReturnValue = AIarTempRetVal1;
		}
		else
		{
			if (NULL != *playerHand && NULL != *discardPile)		// ...then let's play.
			{
				// Count the players hand
				AIarNumCardsInHand = count_the_cards(*playerHand);
				assert(AIarNumCardsInHand > 0);

				if (AIarNumCardsInHand > 0)
				{
					// Determine available plays
					AIarValidChoiceArray = AI_valid_choices(*playerHand, *discardPile);

					if (NULL == AIarValidChoiceArray)						// No available play.  Must draw.
					{
						// Draw a card
						AIarTempCard1 = move_one_card(drawPile, playerHand, 0);
						assert(NULL != AIarTempCard1);

						if (NULL != AIarTempCard1)
						{
							// Can I play that card?
							AIarTempRetVal1 = is_this_play_valid(AIarTempCard1, *discardPile);
							assert(AIarTempRetVal1 >= 0);

							// I can play it
							if (1 == AIarTempRetVal1)
							{
								AIarTempRetVal1 = find_this_position(AIarTempCard1, *playerHand);
								assert(AIarTempRetVal1 >= 0);

								if (AIarTempRetVal1 >= 0)
								{
									AIarTempCard2 = move_one_card(playerHand, discardPile, AIarTempRetVal1);
									assert(NULL != AIarTempCard2);
									assert(AIarTempCard1 == AIarTempCard2);

									if (NULL == AIarTempCard2 || AIarTempCard1 != AIarTempCard2)
									{
										AIarReturnValue = -2;
									}
									else
									{
										if (NULL == *playerHand)
										{
											AIarReturnValue = 1;
										}
										else
										{
											AIarReturnValue = 0;
										}

										// Check for a Wild card
										if (TYPE_WILD == AIarTempCard2->type || TYPE_WILD_DRAW_FOUR == AIarTempCard2->type)
										{
											//// Choose a color for the Wild card
											// Find a random value
											AIarColorChoice = rando_me(1, 4);		// Random color
											assert(AIarColorChoice >= 1 && AIarColorChoice <= 4);

											if (AIarColorChoice < 1 || AIarColorChoice > 4)
											{
												AIarColorChoice = 1;
											}

											// Set the color
											AIarTempCard2->color = AIarColorChoice;
										}
									}
								}
							}
							// I can't play it
							else
							{
								AIarReturnValue = 0;
							}
						}
						else
						{
							AIarReturnValue = -2;
						}
					}
					else		// We have some choices
					{
						// Reset AIaaNumCardsInHand to use as an iterator
						AIarNumCardsInHand = 0;					// Now, an interator

						// AIaaTempCard1 == "current card"
						AIarTempCard1 = (*(AIarValidChoiceArray + AIarNumCardsInHand));

						// Count the available choices
						while (NULL != AIarTempCard1)
						{
							AIarTempCard1 = (*(AIarValidChoiceArray + AIarNumCardsInHand));
							if (NULL != AIarTempCard1)
							{
								AIarNumCardsInHand++;
							}
						}
						assert(1 <= AIarNumCardsInHand);

						// Reduce to count to be an array offsest
						AIarNumCardsInHand--;

						// Randomize one of the available solutions
						if (0 <= AIarNumCardsInHand)
						{
							AIarTempCard2 = (*(AIarValidChoiceArray + AIarNumCardsInHand));
						}
						else 
						{
							AIarTempCard2 = NULL;		// Error
						}

						// Free the valid choice array... we're done with it
						if (NULL != AIarValidChoiceArray)
						{
							free(AIarValidChoiceArray);
						}

						// AIaaTempCard2 now contains a pointer to the appropriate "play"
						//// Convert AIaaTempCard2 to a hand position
						AIarTempRetVal2 = find_this_position(AIarTempCard2, *playerHand);
						assert(AIarTempRetVal2 >= 0);

						if (AIarTempRetVal2 >= 0)
						{
							AIarTempCard1 = move_one_card(playerHand, discardPile, AIarTempRetVal2);
							assert(NULL != AIarTempCard1);
							assert(AIarTempCard1 == AIarTempCard2);

							if (NULL != AIarTempCard1 && AIarTempCard1 == AIarTempCard2)
							{
								if (NULL == *playerHand)
								{
									AIarReturnValue = 1;
								}
								else
								{
									AIarReturnValue = 0;
								}

								// Check for a Wild card
								if (TYPE_WILD == AIarTempCard2->type || TYPE_WILD_DRAW_FOUR == AIarTempCard2->type)
								{
									//// Choose a color for the Wild card
									// Find a random value
									AIarColorChoice = rando_me(1, 4);		// Random color
									assert(AIarColorChoice >= 1 && AIarColorChoice <= 4);

									if (AIarColorChoice < 1 || AIarColorChoice > 4)
									{
										AIarColorChoice = 1;
									}

									// Set the color
									AIarTempCard2->color = AIarColorChoice;
								}
							}
							else
							{
								AIarReturnValue = -2;
							}
						}
						else
						{
							AIarReturnValue = -2;
						}
					}
				}
				else			// Something is wrong with the hand
				{
					AIarReturnValue = -2;
				}
			}
			else				// Either missing a hand or a discard pile
			{
				AIarReturnValue = -1;
			}
		}
	}

	// Update card count
	if (NULL != playerHand && NULL != currentPlayer)
	{
		if (NULL != *playerHand)
		{
			AIarTempRetVal1 = count_the_cards(*playerHand);
			assert(AIarTempRetVal1 > 0);

			if (AIarTempRetVal1 > 0)
			{
				currentPlayer->currentCardCount = AIarTempRetVal1;
			}
		}
	}

	return AIarReturnValue;
}


// Purpose:
//		This algorithm replicates a chaotic strategy (prioritize changes)
// Returns:
//		1 if player won
//		0 on success
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		This function:
//			Attempts to play the highest value card in the hand
//			Appropriately modifies the other decks (see: linked lists)
//			May modify the "head node" pointers for any of these linked lists (see: decks)
int AI_algorithm_chaotic(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer)
{
	int AIacReturnValue = -1;
	int AIacTempRetVal1 = 0;
	int AIacTempRetVal2 = 0;
	oneCard ** AIacValidChoiceArray = NULL;
	oneCard * AIacTempCard1 = NULL;
	oneCard * AIacTempCard2 = NULL;
	int AIacNumCardsInHand = 0;
	int AIacColorChoice = 0;				// This stores the current front runner for the Wild card color selection
	int i = 0;								// Iterating variable
	int AIacNumTries = 0;					// Iterating variables for while loops to guard against infinite loops

	if (NULL != playerHand && NULL != drawPile && NULL != discardPile)
	{
		AIacTempRetVal1 = can_player_play(playerHand, drawPile, discardPile, currentPlayer);
		assert(0 == AIacTempRetVal1 || 1 == AIacTempRetVal1);

		if (1 != AIacTempRetVal1)
		{
			AIacReturnValue = AIacTempRetVal1;
		}
		else
		{
			if (NULL != *playerHand && NULL != *discardPile)		// ...then let's play.
			{
				// Count the players hand
				AIacNumCardsInHand = count_the_cards(*playerHand);
				assert(AIacNumCardsInHand > 0);

				if (AIacNumCardsInHand > 0)
				{
					// Determine available plays
					AIacValidChoiceArray = AI_valid_choices(*playerHand, *discardPile);

					if (NULL == AIacValidChoiceArray)						// No available play.  Must draw.
					{
						// Draw a card
						AIacTempCard1 = move_one_card(drawPile, playerHand, 0);
						assert(NULL != AIacTempCard1);

						if (NULL != AIacTempCard1)
						{
							// Can I play that card?
							AIacTempRetVal1 = is_this_play_valid(AIacTempCard1, *discardPile);
							assert(AIacTempRetVal1 >= 0);

							// I can play it
							if (1 == AIacTempRetVal1)
							{
								AIacTempRetVal1 = find_this_position(AIacTempCard1, *playerHand);
								assert(AIacTempRetVal1 >= 0);

								if (AIacTempRetVal1 >= 0)
								{
									AIacTempCard2 = move_one_card(playerHand, discardPile, AIacTempRetVal1);
									assert(NULL != AIacTempCard2);
									assert(AIacTempCard1 == AIacTempCard2);

									if (NULL == AIacTempCard2 || AIacTempCard1 != AIacTempCard2)
									{
										AIacReturnValue = -2;
									}
									else
									{
										if (NULL == *playerHand)
										{
											AIacReturnValue = 1;
										}
										else
										{
											AIacReturnValue = 0;
										}

										// Check for a Wild card
										if (TYPE_WILD == AIacTempCard2->type || TYPE_WILD_DRAW_FOUR == AIacTempCard2->type)
										{
											//// Choose a color for the Wild card
											// Find a random value
											AIacColorChoice = (*discardPile)->color;
											AIacNumTries = 0;

											while (AIacColorChoice == (*discardPile)->color && AIacNumTries <= MAX_TRIES)
											{
												AIacColorChoice = rando_me(1, 4);		// Random color
												AIacNumTries++;
											}
											assert(AIacColorChoice >= 1 && AIacColorChoice <= 4);

											if (AIacColorChoice < 1 || AIacColorChoice > 4)
											{
												AIacColorChoice = 1;
											}

											// Set the color
											AIacTempCard2->color = AIacColorChoice;
										}
									}
								}
							}
							// I can't play it
							else
							{
								AIacReturnValue = 0;
							}
						}
						else
						{
							AIacReturnValue = -2;
						}
					}
					else		// We have some choices
					{
						// Reset AIaaNumCardsInHand to use as a counter
						AIacNumCardsInHand = 0;					// Now, an iterator

						// AIaaTempCard1 == "current card"
						AIacTempCard1 = (*(AIacValidChoiceArray + AIacNumCardsInHand));

						// Find a choice that changes the color
						AIacTempCard2 = NULL;

						while (NULL != AIacTempCard1)
						{
							if ((*discardPile)->color != AIacTempCard1->color)
							{
								if (NULL == AIacTempCard2)
								{
									AIacTempCard2 = AIacTempCard1;
								}
								else if (card_value(AIacTempCard2) < card_value(AIacTempCard1))
								{
									AIacTempCard2 = AIacTempCard1;
								}
							}

							AIacNumCardsInHand++;
							AIacTempCard1 = (*(AIacValidChoiceArray + AIacNumCardsInHand));
						}

						// Check the result
						if (NULL == AIacTempCard2)
						{
							// Find the largest card value
							AIacNumCardsInHand = 0;
							AIacTempCard1 = (*(AIacValidChoiceArray + AIacNumCardsInHand));

							while (NULL != AIacTempCard1)
							{
								if (NULL == AIacTempCard2)
								{
									AIacTempCard2 = AIacTempCard1;
								}
								else if (card_value(AIacTempCard2) < card_value(AIacTempCard1))
								{
									AIacTempCard2 = AIacTempCard1;
								}

								AIacNumCardsInHand++;
								AIacTempCard1 = (*(AIacValidChoiceArray + AIacNumCardsInHand));
							}
						}
						assert(NULL != AIacTempCard2);

						// Free the valid choice array... we're done with it
						if (NULL != AIacValidChoiceArray)
						{
							free(AIacValidChoiceArray);
						}

						// AIaaTempCard2 now contains a pointer to the appropriate "play"
						//// Convert AIaaTempCard2 to a hand position
						AIacTempRetVal2 = find_this_position(AIacTempCard2, *playerHand);
						assert(AIacTempRetVal2 >= 0);

						if (AIacTempRetVal2 >= 0)
						{
							AIacTempCard1 = move_one_card(playerHand, discardPile, AIacTempRetVal2);
							assert(NULL != AIacTempCard1);
							assert(AIacTempCard1 == AIacTempCard2);

							if (NULL != AIacTempCard1 && AIacTempCard1 == AIacTempCard2)
							{
								if (NULL == *playerHand)
								{
									AIacReturnValue = 1;
								}
								else
								{
									AIacReturnValue = 0;
								}

								// Check for a Wild card
								if (TYPE_WILD == AIacTempCard2->type || TYPE_WILD_DRAW_FOUR == AIacTempCard2->type)
								{
									//// Choose a color for the Wild card
									// Find a random value
									AIacColorChoice = (*discardPile)->color;
									AIacNumTries = 0;

									while (AIacColorChoice == (*discardPile)->color && AIacNumTries <= MAX_TRIES)
									{
										AIacColorChoice = rando_me(1, 4);		// Random color
										AIacNumTries++;
									}
									assert(AIacColorChoice >= 1 && AIacColorChoice <= 4);

									if (AIacColorChoice < 1 || AIacColorChoice > 4)
									{
										AIacColorChoice = 1;
									}

									// Set the color
									AIacTempCard2->color = AIacColorChoice;
								}
							}
							else
							{
								AIacReturnValue = -2;
							}
						}
						else
						{
							AIacReturnValue = -2;
						}
					}
				}
				else			// Something is wrong with the hand
				{
					AIacReturnValue = -2;
				}
			}
			else				// Either missing a hand or a discard pile
			{
				AIacReturnValue = -1;
			}
		}
	}

	// Update card count
	if (NULL != playerHand && NULL != currentPlayer)
	{
		if (NULL != *playerHand)
		{
			AIacTempRetVal1 = count_the_cards(*playerHand);
			assert(AIacTempRetVal1 > 0);

			if (AIacTempRetVal1 > 0)
			{
				currentPlayer->currentCardCount = AIacTempRetVal1;
			}
		}
	}

	return AIacReturnValue;
}


// Purpose:
//		This algorithm replicates an aggressive strategy (prioritize draws, skips, then reverses)
// Returns:
//		1 if player won
//		0 on success
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		This function:
//			Attempts to play the highest value card in the hand
//			Appropriately modifies the other decks (see: linked lists)
//			May modify the "head node" pointers for any of these linked lists (see: decks)
int AI_algorithm_aggressive(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer)
{
	int AIaaReturnValue = -1;
	int AIaaTempRetVal1 = 0;
	int AIaaTempRetVal2 = 0;
	oneCard ** AIaaValidChoiceArray = NULL;
	oneCard * AIaaTempCard1 = NULL;
	oneCard * AIaaTempCard2 = NULL;
	int AIaaNumCardsInHand = 0;
	int AIaaColorChoice = 0;				// This stores the current front runner for the Wild card color selection
	int i = 0;								// Iterating variable
	int AIaaNumTries = 0;					// Iterating variables for while loops to guard against infinite loops
	int arrayOfChoiceValues[6] = { 0 };		// index represents color values.  [index] represents weight.  Draw Twos == 2; Skips == 1.

	if (NULL != playerHand && NULL != drawPile && NULL != discardPile)
	{
		AIaaTempRetVal1 = can_player_play(playerHand, drawPile, discardPile, currentPlayer);
		assert(0 == AIaaTempRetVal1 || 1 == AIaaTempRetVal1);

		if (1 != AIaaTempRetVal1)
		{
			AIaaReturnValue = AIaaTempRetVal1;
		}
		else
		{
			if (NULL != *playerHand && NULL != *discardPile)		// ...then let's play.
			{
				// Count the players hand
				AIaaNumCardsInHand = count_the_cards(*playerHand);
				assert(AIaaNumCardsInHand > 0);

				if (AIaaNumCardsInHand > 0)
				{
					// Determine available plays
					AIaaValidChoiceArray = AI_valid_choices(*playerHand, *discardPile);

					if (NULL == AIaaValidChoiceArray)						// No available play.  Must draw.
					{
						// Draw a card
						AIaaTempCard1 = move_one_card(drawPile, playerHand, 0);
						assert(NULL != AIaaTempCard1);

						if (NULL != AIaaTempCard1)
						{
							// Can I play that card?
							AIaaTempRetVal1 = is_this_play_valid(AIaaTempCard1, *discardPile);
							assert(AIaaTempRetVal1 >= 0);

							// I can play it
							if (1 == AIaaTempRetVal1)
							{
								AIaaTempRetVal1 = find_this_position(AIaaTempCard1, *playerHand);
								assert(AIaaTempRetVal1 >= 0);

								if (AIaaTempRetVal1 >= 0)
								{
									AIaaTempCard2 = move_one_card(playerHand, discardPile, AIaaTempRetVal1);
									assert(NULL != AIaaTempCard2);
									assert(AIaaTempCard1 == AIaaTempCard2);

									if (NULL == AIaaTempCard2 || AIaaTempCard1 != AIaaTempCard2)
									{
										AIaaReturnValue = -2;
									}
									else
									{
										if (NULL == *playerHand)
										{
											AIaaReturnValue = 1;
										}
										else
										{
											AIaaReturnValue = 0;
										}

										// Check for a Wild card
										if (TYPE_WILD == AIaaTempCard2->type || TYPE_WILD_DRAW_FOUR == AIaaTempCard2->type)
										{
											//// Choose a color for the Wild card
											// Count the colors that allow player to play Draws and Skips
											AIaaTempCard1 = *playerHand;
											AIaaNumCardsInHand = 0;			// Now, a counter

											while (NULL != AIaaTempCard1)
											{
												if (TYPE_DRAW_TWO == AIaaTempCard1->type)
												{
													arrayOfChoiceValues[AIaaTempCard1->color] += 2;
													AIaaNumCardsInHand++;
												}
												else if (TYPE_SKIP == AIaaTempCard1->type)
												{
													arrayOfChoiceValues[AIaaTempCard1->color] += 1;
													AIaaNumCardsInHand++;
												}

												AIaaTempCard1 = AIaaTempCard1->nextCard;
											}

											// Find the most aggressive color
											AIaaColorChoice = rando_me(1, 4);
											assert(AIaaColorChoice >= 1 && AIaaColorChoice <= 4);

											if (AIaaColorChoice < 1 || AIaaColorChoice > 4)
											{
												AIaaColorChoice = COLOR_BLUE;
											}

											if (0 < AIaaNumCardsInHand) // Found some Draw Twos and Skips
											{
												for (i = 1; i < 5; i++)
												{
													if (arrayOfChoiceValues[AIaaColorChoice] < arrayOfChoiceValues[i])
													{
														AIaaColorChoice = i;
													}
												}
											}
											else	// No aggressive plays
											{
												// Find the color with the most points
												AIaaTempCard1 = *playerHand;

												// Reset the array becasue it didn't help the first time
												for (i = 0; i < 6; i++)
												{
													arrayOfChoiceValues[i] = 0;
												}

												// Tally the values
												while (NULL != AIaaTempCard1)
												{
													AIaaTempRetVal1 = card_value(AIaaTempCard1);
													assert(AIaaTempRetVal1 >= 0);

													if (AIaaTempRetVal1 > 0)
													{
														arrayOfChoiceValues[AIaaTempCard1->color] += AIaaTempRetVal1;
													}

													AIaaTempCard1 = AIaaTempCard1->nextCard;
												}

												// Find the color with the most points
												for (i = 1; i < 5; i++)
												{
													if (arrayOfChoiceValues[AIaaColorChoice] < arrayOfChoiceValues[i])
													{
														AIaaColorChoice = i;
													}
												}
												assert(AIaaColorChoice >= 1 && AIaaColorChoice <= 4);
											}

											// Set the color
											AIaaTempCard2->color = AIaaColorChoice;
										}
									}
								}
							}
							// I can't play it
							else
							{
								AIaaReturnValue = 0;
							}
						}
						else
						{
							AIaaReturnValue = -2;
						}
					}
					else		// We have some choices
					{
						// AIaaTempCard1 == "current card"
						AIaaTempCard1 = (*(AIaaValidChoiceArray + AIaaNumCardsInHand));

						// Find an aggressive choice
						AIaaTempCard2 = NULL;

						while (NULL != AIaaTempCard1)
						{
							if (TYPE_WILD_DRAW_FOUR == AIaaTempCard1->type)
							{
								AIaaTempCard2 = AIaaTempCard1;
								break;
							}
							else if (TYPE_DRAW_TWO == AIaaTempCard1->type)
							{
									AIaaTempCard2 = AIaaTempCard1;
							}
							else if (TYPE_SKIP == AIaaTempCard1->type)
							{
								if (NULL == AIaaTempCard2)
								{
									AIaaTempCard2 = AIaaTempCard1;
								}
							}

							AIaaTempCard1 = (*(AIaaValidChoiceArray + AIaaNumCardsInHand));
						}

						// Check the result
						if (NULL == AIaaTempCard2)
						{
							// Find the biggest play
							AIaaNumCardsInHand = 0;
							AIaaTempCard1 = (*(AIaaValidChoiceArray + AIaaNumCardsInHand));

							while (NULL != AIaaTempCard1)
							{
								if (NULL == AIaaTempCard2)
								{
									AIaaTempCard2 = AIaaTempCard1;
								}
								else if (card_value(AIaaTempCard2) < card_value(AIaaTempCard1))
								{
									AIaaTempCard2 = AIaaTempCard1;
								}

								AIaaNumCardsInHand++;
								AIaaTempCard1 = (*(AIaaValidChoiceArray + AIaaNumCardsInHand));
							}
						}
						assert(NULL != AIaaTempCard2);

						// Free the valid choice array... we're done with it
						if (NULL != AIaaValidChoiceArray)
						{
							free(AIaaValidChoiceArray);
						}

						// AIaaTempCard2 now contains a pointer to the appropriate "play"
						//// Convert AIaaTempCard2 to a hand position
						AIaaTempRetVal2 = find_this_position(AIaaTempCard2, *playerHand);
						assert(AIaaTempRetVal2 >= 0);

						if (AIaaTempRetVal2 >= 0)
						{
							AIaaTempCard1 = move_one_card(playerHand, discardPile, AIaaTempRetVal2);
							assert(NULL != AIaaTempCard1);
							assert(AIaaTempCard1 == AIaaTempCard2);

							if (NULL != AIaaTempCard1 && AIaaTempCard1 == AIaaTempCard2)
							{
								if (NULL == *playerHand)
								{
									AIaaReturnValue = 1;
								}
								else
								{
									AIaaReturnValue = 0;
								}

								// Check for a Wild card
								if (TYPE_WILD == AIaaTempCard2->type || TYPE_WILD_DRAW_FOUR == AIaaTempCard2->type)
								{
									//// Choose a color for the Wild card
									// Count the colors that allow player to play Draws and Skips
									AIaaTempCard1 = *playerHand;
									AIaaNumCardsInHand = 0;			// Now, a counter

									while (NULL != AIaaTempCard1)
									{
										if (TYPE_DRAW_TWO == AIaaTempCard1->type)
										{
											arrayOfChoiceValues[AIaaTempCard1->color] += 2;
											AIaaNumCardsInHand++;
										}
										else if (TYPE_SKIP == AIaaTempCard1->type)
										{
											arrayOfChoiceValues[AIaaTempCard1->color] += 1;
											AIaaNumCardsInHand++;
										}

										AIaaTempCard1 = AIaaTempCard1->nextCard;
									}

									// Find the most aggressive color
									AIaaColorChoice = rando_me(1, 4);
									assert(AIaaColorChoice >= 1 && AIaaColorChoice <= 4);

									if (AIaaColorChoice < 1 || AIaaColorChoice > 4)
									{
										AIaaColorChoice = COLOR_BLUE;
									}

									if (0 < AIaaNumCardsInHand) // Found some Draw Twos and Skips
									{
										for (i = 1; i < 5; i++)
										{
											if (arrayOfChoiceValues[AIaaColorChoice] < arrayOfChoiceValues[i])
											{
												AIaaColorChoice = i;
											}
										}
									}
									else
									{
										// Find the color with the most points
										AIaaTempCard1 = *playerHand;

										// Reset the array becasue it didn't help the first time
										for (i = 0; i < 6; i++)
										{
											arrayOfChoiceValues[i] = 0;
										}

										// Tally the values
										while (NULL != AIaaTempCard1)
										{
											AIaaTempRetVal1 = card_value(AIaaTempCard1);
											assert(AIaaTempRetVal1 >= 0);

											if (AIaaTempRetVal1 > 0)
											{
												arrayOfChoiceValues[AIaaTempCard1->color] += AIaaTempRetVal1;
											}

											AIaaTempCard1 = AIaaTempCard1->nextCard;
										}

										// Find the color with the most points
										for (i = 1; i < 5; i++)
										{
											if (arrayOfChoiceValues[AIaaColorChoice] < arrayOfChoiceValues[i])
											{
												AIaaColorChoice = i;
											}
										}
										assert(AIaaColorChoice >= 1 && AIaaColorChoice <= 4);
									}

									// Set the color
									AIaaTempCard2->color = AIaaColorChoice;
								}
							}
							else
							{
								AIaaReturnValue = -2;
							}
						}
						else
						{
							AIaaReturnValue = -2;
						}
					}
				}
				else			// Something is wrong with the hand
				{
					AIaaReturnValue = -2;
				}
			}
			else				// Either missing a hand or a discard pile
			{
				AIaaReturnValue = -1;
			}
		}
	}

	// Update card count
	if (NULL != playerHand && NULL != currentPlayer)
	{
		if (NULL != *playerHand)
		{
			AIaaTempRetVal1 = count_the_cards(*playerHand);
			assert(AIaaTempRetVal1 > 0);

			if (AIaaTempRetVal1 > 0)
			{
				currentPlayer->currentCardCount = AIaaTempRetVal1;
			}
		}
	}

	return AIaaReturnValue;
}
