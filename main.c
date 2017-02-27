#include "One_Header_v2.h"
#include <assert.h>

int main(void)
{
	// Setup draw pile
	oneCard * drawPile = build_a_new_deck();
	assert(drawPile);

	// Start discard pile
	oneCard * discardPile = NULL;

	// Prepare player 1
	onePlayer me = { NULL, 0 };
	me.currentHand = deal_a_hand(&drawPile); 
	assert(me.currentHand);
	me.currentCardCount = count_the_cards(me.currentHand);

	

	

	assert(destroy_a_deck(&drawPile));
	assert(destroy_a_deck(&(me.currentHand)));

	return 0;
}
