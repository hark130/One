#ifndef ONE_HEADER__H
#define ONE_HEADER__H (int(5)) // Version 5

// NOTES
//// Version 3
//////	Incorporates new function to shuffle a deck (see: linked list of oneCard nodes)
//// Version 4
//////	Create AI
//////  Modified onePlayer struct to incorporate a ciruclar doubly linked list
//////  oneCard struct now includes a member to store whether the action has been taken for draws and skips
//////  Skips, Draw Two, and Wild Draw Four now implemented.
//// Version 5
//////	Incoporated score permanence and game continuation

// TO DO
//// print_the_hand() spacing gets out of control with hand size > 9
//// Escape sequence while choosing card number to play?  Not necessary.
//// Has draw_a_card() been entirely factored out with move_a_card()?
//// Add challenge on Wild Draw 4
//// Consider refactoring shuffle_the_discards_in()... there's a mostly redundant code blocks
//// Write an internal algorithm for AI_algorithm_points() that properly chooses a tie based on value
//// Consider refactoring player/algorithm functions now that can_player_play() validates input
//// Refactor all player/algorithm functions to reference an external update_card_count()?
//// Consider refactoring the algorithm functions into callback functions to a wrapper shell
//// Consider refactoring the valid_choice() algorithm-helper function to take a callback function


// BUGS
//// Make better decisions about playing a Wild


#ifndef NULL
#define NULL ((void*)0)
#endif // !NULL

//#define AUTOMATE_PLAY 1				// Indicate that the game should be played by all AI players
#define STOP_LOOPING 200			// Upper limit for while-loop retries
#define MAX_NUM_PLAYERS 24			// Max number of players allowed
#define PLAY_CLOCKWISE 69			// Indicates player1 --> player2 --> player3 --> player4
#define PLAY_COUNTER_CLOCKWISE 96	// Indicates player4 --> player3 --> player2 --> player1
#define MAX_SCORE 1000000			// Maximum allowable score

#ifdef AUTOMATE_PLAY
#define NUM_SECONDS_TO_PAUSE 0		// Turn off pauses during automation
#define NDEBUG
#else
#define NUM_SECONDS_TO_PAUSE 3		// Default prompt pause
#endif // AUTOMATE_PLAY


#define COLOR_RED ((int)1)
#define COLOR_YELLOW ((int)2)
#define COLOR_GREEN ((int)3)
#define COLOR_BLUE ((int)4)
#define COLOR_BLACK ((int)5)

#define TYPE_ZERO ((char)'0')
#define TYPE_ONE ((char)'1')
#define TYPE_TWO ((char)'2')
#define TYPE_THREE ((char)'3')
#define TYPE_FOUR ((char)'4')
#define TYPE_FIVE ((char)'5')
#define TYPE_SIX ((char)'6')
#define TYPE_SEVEN ((char)'7')
#define TYPE_EIGHT ((char)'8')
#define TYPE_NINE ((char)'9')
#define TYPE_REVERSE ((char)'R')
#define TYPE_SKIP ((char)'S')
#define TYPE_DRAW_TWO ((char)'D')
#define TYPE_WILD ((char)'w')
#define TYPE_WILD_DRAW_FOUR ((char)'W')

#define BORDER_UPPER_LEFT ((unsigned char)201)
#define BORDER_UPPER_RIGHT ((unsigned char)187)
#define BORDER_VERTICAL ((unsigned char)186)
#define BORDER_LOWER_RIGHT ((unsigned char)188)
#define BORDER_LOWER_LEFT ((unsigned char)200)
#define BORDER_HORIZONTAL ((unsigned char)205)
#define BORDER_SPACE ((unsigned char)' ')

typedef struct
{
	struct oneCard * prevCard;
	char type;
	int color;
	int processed;
	struct oneCard * nextCard;
} oneCard;

typedef struct
{
	struct onePlayer * prevPlayer;
	int playerNumber;
	oneCard * currentHand;
	unsigned int currentCardCount;
	unsigned int currentScore;
	int(*algorithm)(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, struct onePlayer * currentPlayer, int directionOfPlay);
	struct onePlayer * nextPlayer;
} onePlayer;


#ifdef _WIN32
#    define CLEAR_SCREEN system("cls")					// 32 and 64-bit Windows OS
#	 include <Windows.h>
#	 define PAUSE_OUTPUT(n) Sleep(n * 1000)
#elif defined(__linux__)
#    define CLEAR_SCREEN system("clear")				// Linux kernel
#	 include <unistd.h>
#	 define PAUSE_OUTPUT(n) sleep(n)
#elif defined(__unix__)
#    define CLEAR_SCREEN system("clear")				// UNIX environment
#	 include <unistd.h>
#	 define PAUSE_OUTPUT(n) sleep(n)
#elif defined(__APPLE__)
#    define CLEAR_SCREEN system("clear")				// Mac OS X
#	 include <unistd.h>
#	 define PAUSE_OUTPUT(n) sleep(n)
#else
#    define CLEAR_SCREEN puts("\n\n\n\n\n\n\n\n\n")		// Poor man's clear||cls
#	 define PAUSE_OUTPUT(n) { puts("Press enter to continue."); getchar(); clear_stdin(); }		// Poor man's sleep||Sleep
#endif


// Purpose:
//		Builds a doubly linked list of "One" cards
// Returns:
//		Pointer to head node on success
//		NULL on failure
// Notes:
//		This deck is "shuffled"
oneCard * build_a_new_deck(void);


// Purpose:
//		Free all the dynamically allocated memory for all the cards in the linked list
// Returns:
//		Number of cards freed on success
//		0 on failure
//		-1 on NULL pointer
//		-2 on unspecified error
int destroy_a_deck(oneCard ** oldDeck);


oneCard * allocate_a_card(void);


// Purpose:
//		Free dynamically allocated memory and zero the data
// Returns:
//		0 on success
//		-1 on failure
int destroy_a_card(oneCard ** oldCard);


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
oneCard * link_a_card(oneCard * existingDeck, oneCard * cardToLink);


// Purpose:
//		Remove a card from the deck
// Returns:
//		Pointer to the new head node
//		NULL on error
oneCard * remove_a_card(oneCard * cardToRemove);


// Purpose:
//		Add a card into a deck
// Returns:
//		Pointer to the new destination head node (see: cardToAdd)
//		NULL on error
// Note:
//		cardToAdd goes "on top" of the destination (see: new head node)
//		This function handles all the prevCard and nextCard linkages
oneCard * add_a_card(oneCard * cardToAdd, oneCard * destination);


// Purpose:
//		Randomize one card out of the linked list starting at stackOfCards and remove it from the deck
// Returns:
//		Pointer to a card struct
//		NULL on error
// Note:
//		The original pointer to stackOfCards may be modified if the head node is drawn
oneCard * draw_a_card(oneCard ** stackOfCards);


// Purpose:
//		Find the head node in this double linked list
// Returns:
//		NULL on error
//		Pointer to the head node on success
oneCard * find_the_first_card(oneCard * anyCard);


// Purpose:
//		Find the tail node in this double linked list
// Returns:
//		NULL on error
//		Pointer to the head node on success
oneCard * find_the_last_card(oneCard * anyCard);


// Purpose:
//		Deal a hand of 7 cards
// Returns:
//		Return a pointer to the hand
//		NULL on error
// Note:
//		The original pointer to the drawPile may be modified if the head node happens to be drawn
oneCard * deal_a_hand(oneCard ** drawPile);


// Purpose:
//		Count the number of nodes in a oneCard struct
// Returns:
//		Number of cards on success
//		-1 on NULL
//		0 on error
int count_the_cards(oneCard * anyCard);


// Purpose:
//		Print the cards in a player's doubly linked list
// Returns:
//		None
void print_players_hand(oneCard * anyCard);


// Purpose:
//		Print one card
// Returns:
//		None
void print_one_card(oneCard * oneCard);


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
oneCard * move_one_card(oneCard ** oldLocation, oneCard ** newLocation, int cardNum);


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
int player_menu(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer, int directionOfPlay);


// Purpose:
//		Remove any pesky characters lurking around on the stdin stream
void clear_stdin(void);


// Purpose:
//		One function to print the current status of the game
// Notes:
//		Prints the player status, discard pile, and player's hand
void print_the_game(oneCard * playerHand, oneCard * discardPile, onePlayer * currentPlayer, int directionOfPlay);


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
int is_this_play_valid(oneCard * intendedPlay, oneCard * discardPile);


// Purpose:
//		Find this card number in this deck
// Returns:
//		Pointer to the card
//		NULL on error
// Notes:
//		Start counting at 0
oneCard * find_this_card(int cardNum, oneCard * haystackDeck);


// Purpose:
//		Find the position number of this card in this deck
// Returns:
//		Position number, starting at 0, on success
//		-1 on NULL
//		-2 if not in the deck
//		-3 on unspecified error
// Notes:
//		Start counting at 0
int find_this_position(oneCard * singleCard, oneCard * haystackDeck);


// Purpose:
//		Randomize the order of a deck of cards
// Returns:
//		Pointer to the new head node
//		NULL on error
// Notes:
//		The head node will likely change when the deck is shuffled
oneCard * shuffle_a_deck(oneCard * newDeck);


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
int get_val_from_color(char colorLetter);


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
int setup_game(oneCard ** drawPile, oneCard ** discardPile, onePlayer * playerNode);


// Purpose:
//		Provide the point value of a single card
// Returns:
//		Point value of the card on success
//		-1 on NULL pointer
//		-2 on unspecified error
int card_value(oneCard * singleCard);


// Purpose:
//		Provide the point value for an entire deck (see: player's hand)
// Returns:
//		Point value total on success
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		Calls card_value() under the hood
int hand_value(oneCard * headNode);


// Purpose:
//		Shuffle the discard pile into the draw pile, leaving one card in the discard pile
// Returns:
//		0 on success
//		-1 on NULL pointer
//		-2 for an unspecified error
// Notes:
//		The drawPile head node will likely change
//		The discardPile head node should not change
int shuffle_the_discards_in(oneCard ** drawPile, oneCard ** discardPile);


// Purpose:
//		One-stop-function for player_menu() and AI_algorithms() to respond to certain cards
// Returns:
//		1 if the player can take an action (e.g., play, draw, pass)
//		0 if the player can not take an action (e.g., skipped, draw two, draw four)
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		On a skip, top discard is modified to reflect that someone has already been skipped
//		On a draw two, top discard is modified to reflect that someone has already drawn the cards
//		On a draw four, top discard is modified to reflect that someone has already drawn the cards
int can_player_play(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer);


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
int reset_game(oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer);


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
int score_players_hands(onePlayer * theWinner);

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
oneCard ** AI_valid_choices(oneCard * playersHand, oneCard * discardPile);


// Purpose:
//		This algorithm replicates a get-rid-of-points strategy
// Returns:
//		0 on success
//		-1 on NULL pointer
//		-2 on unspecified error
// Notes:
//		This function:
//			Attempts to play the highest value card in the hand
//			Appropriately modifies the other decks (see: linked lists)
//			May modify the "head node" pointers for any of these linked lists (see: decks)
int AI_algorithm_points(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer);


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
int AI_algorithm_random(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer);


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
int AI_algorithm_chaotic(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer);


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
int AI_algorithm_aggressive(oneCard ** playerHand, oneCard ** drawPile, oneCard ** discardPile, onePlayer * currentPlayer);

#endif // ONE_HEADER__H

