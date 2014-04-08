/** @ingroup Network */
/** @{ */
/**
 * @file packet_min_utils.c
 */
/** @} */

#include "packet_min_utils.h"

/*	
 * experiment_a.c - A simple packet encapsulation/decapsulation library
 *	
 * Designed by Clark Allenby & Shane Spoor
 * Written by Clark Allenby
 *	
 * For starters, this library will convert a PKT_POS_UPDATE_MIN and a
 * PKT_ALL_POS_UPDATE_MIN to a more network-efficient structure. The total benefit
 * of this provides a 71.3% decrease in network bandwidth usage for these packets,
 * significantly increasing network performance.
 *	
 *	There are 4 functions implemented here (see their comments for more information):
 *		PKT_POS_UPDATE_MIN_MIN encapsulate_pos_update(PKT_POS_UPDATE_MIN old_pkt);
 *		PKT_POS_UPDATE_MIN decapsulate_pos_update(PKT_POS_UPDATE_MIN_MIN pkt);
 *		PKT_ALL_POS_UPDATE_MIN_MIN encapsulate_all_pos_update(PKT_ALL_POS_UPDATE_MIN old_pkt);
 *		PKT_ALL_POS_UPDATE_MIN decapsulate_all_pos_update(PKT_ALL_POS_UPDATE_MIN_MIN pkt);
 *	
 *	There are also 2 new structures here:
 *		PKT_POS_UPDATE_MIN:
 *			This structure contains just 2 variables, a uint32_t 'data', and a
 *			uint8_t 'vel'. 'data' contains a bitwise representation of 'floor',
 *			'player_number', 'xPos', and 'yPos'. 'floor' gets the first 5 bits in
 *			'data', 'player_number' gets the next 5 bits, 'xPos' gets the next 11
 *			bits, and 'yPos' gets the last 11 bits. 'vel' contains the 'xVel' and
 *			'yVel' of the player. 'xVel' gets the first 4 bits, and 'yVel' gets
 *			the last 4 bits.
 *		PKT_ALL_POS_UPDATE_MIN:
 *			This structure contains many variables, first, a uint32_t
 *			'players_on_floor', then two 11 element uint32_t arrays 'xVel' and
 *			'yVel', and one 32 element uint8_t array 'vel'. 'players_on_floor'
 *			allows 1 bit per player to represent existance on this floor, player
 *			zero at the most significant bit and player 31 at the least significant
 *			bit. 'xPos' and 'yPos' are the most interesting, they each contain 32
 *			11 bit variables packed awkwardly into an array, most significant bit
 *			to least significant bit ordering. For example, the 11 most significant
 *			bits of 'xPos[0]' represent the 0th player's 'xPos', and the next 11
 *			most significant bits represent the 1st player's 'xPos'. The 3rd players
 *			'xPos' is in both the least significant bits of 'xPos[0]', and the
 *			most significant bits of 'xPos[1]'. 'vel' contains 32 uint8_t
 *			variables in an array, each element to contain the 'xVel' as the most
 *			significant 4 bits, and 'yVel' as the least significant 4 bits.
 *	
 *	These functions were lovingly designed and implemented by Clark Allenby with
 *	special consideration to Shane Spoor with work on the design, to be easily
 *	ported to existing code. The only changes needed are minor, and are not worth
 *	complaining about due to the significant increase in networking performance.
 *	Please use these and be happy.
 */

/**
 * This encapsulates the data from an old packet into a new packet and frees the old packet, no
 * questions asked.
 *
 * This function returns NULL if memory couldn't be allocated, but otherwise always returns
 * a successfully encapsulated packet. If there is data that is out of bounds, (for example
 * a position higher than 4096) it will overflow silently. The structure returned can be 
 * easily sent over the network.
 *
 * @param old_pkt Pointer to the PKT_POS_UPDATE_ packet to be encapsulated. Must be valid.
 * @return Pointer to the encapsulated PKT_POS_UPDATE_MIN. Returns NULL if the packet couldn't be 
 *         allocated.
 *
 * @designer Clark Allenby
 * @author   Clark Allenby
 */
PKT_POS_UPDATE_MIN *encapsulate_pos_update(PKT_POS_UPDATE *old_pkt) {
	PKT_POS_UPDATE_MIN *pkt = (PKT_POS_UPDATE_MIN *)malloc(sizeof(PKT_POS_UPDATE_MIN));
	uint32_t n_xPos, n_yPos, n_xVel, n_yVel;
	
	if(!pkt)
		return NULL;
	
	n_xPos = (uint32_t)round(old_pkt->xPos / GRANULARITY_POS);
	n_yPos = (uint32_t)round(old_pkt->yPos / GRANULARITY_POS);
	n_xVel = (uint32_t)round(old_pkt->xVel * GRANULARITY_VEL + FACTOR);
	n_yVel = (uint32_t)round(old_pkt->yVel * GRANULARITY_VEL + FACTOR);
	
	pkt->data = 0;
	pkt->data |= old_pkt->floor & 0x1F;
	pkt->data <<= 5;
	pkt->data |= old_pkt->player_number & 0x1F;
	pkt->data <<= 11;
	pkt->data |= n_xPos & 0x7FF;
	pkt->data <<= 11;
	pkt->data |= n_yPos & 0x7FF;
	
	pkt->vel = 0;
	pkt->vel |= n_xVel & 0xFF;
	pkt->vel <<= 8;
	pkt->vel |= n_yVel & 0xFF;
	
	free(old_pkt);
	return pkt;
}

/**
 * This decapsulates a new packet into an old packet, allowing for
 * easy implementation to existing code.
 *
 * This function fails only if memory can't be allocated and always returns with a valid
 * structure. Note that the actual transmitted data may be wrong.
 *
 * @param pkt Pointer to the PKT_POS_UPDATE_MIN packet to be decapsulated. Must be valid.
 * @return Pointer to the decapsulated PKT_POS_UPDATE_. This may contain incorrect
 *         or impossible values (according to game rules). Returns NULL if the packet 
 *         couldn't be allocated.
 *
 * @designer Clark Allenby
 * @author   Clark Allenby
 */
PKT_POS_UPDATE *decapsulate_pos_update(PKT_POS_UPDATE_MIN *pkt) {
	PKT_POS_UPDATE *old_pkt = (PKT_POS_UPDATE *)malloc(sizeof(PKT_POS_UPDATE));
	
	if(!old_pkt)
		return NULL;
	
	old_pkt->floor = (pkt->data >> 27) & 0x1F;
	old_pkt->player_number = (pkt->data >> 22) & 0x1F;
	old_pkt->xPos = (float)(GRANULARITY_POS * ((pkt->data >> 11) & 0x7FF));
	old_pkt->yPos = (float)(GRANULARITY_POS * (pkt->data & 0x7FF));
	
	old_pkt->xVel = (float)((((pkt->vel >> 8) & 0xFF) - FACTOR) / GRANULARITY_VEL);
	old_pkt->yVel = (float)(((pkt->vel & 0xFF) - FACTOR) / GRANULARITY_VEL);
	
	free(pkt);
	
	return old_pkt;
}

/**
 * This encapsulates the data from an old packet into a new packet, no
 * questions asked.
 *
 * This function returns NULL if memory can't be allocated, but otherwise returns with
 * a successful packet. If there is data that is out of bounds, (for example a position
 * higher than 4096) it will overflow silently. The structure returned can be easily sent
 * over the network.
 *
 * Revisions: <ul>
 *					<li>March 22, 2014 - Shane Spoor - Uses loops instead of hard coding
 *                      each step, and returns a pointer to the packet.</li>
 *            </ul>
 *
 * @param old_pkt Pointer to the PKT_ALL_POS_UPDATE packet to be encapsulated. Must be valid.
 * @return Pointer to the encapsulated PKT_ALL_POS_UPDATE_MIN, or NULL if the memory couldn't be allocated.
 *
 * @designer Clark Allenby
 * @author   Clark Allenby
 */
PKT_ALL_POS_UPDATE_MIN *encapsulate_all_pos_update(PKT_ALL_POS_UPDATE *old_pkt) {
	PKT_ALL_POS_UPDATE_MIN *pkt = (PKT_ALL_POS_UPDATE_MIN*)malloc(sizeof (PKT_ALL_POS_UPDATE_MIN));
	int i, j;
	uint32_t n_xPos[32], n_yPos[32], n_xVel, n_yVel, and_mask;
	
	if(!pkt)
		return NULL;
	
	pkt->floor = (uint8_t)old_pkt->floor;
	pkt->players_on_floor = 0;
	for (i = 0; i < 32; i++) {
		n_xVel = (uint32_t)round(old_pkt->xVel[i] * GRANULARITY_VEL + FACTOR);
		n_yVel = (uint32_t)round(old_pkt->yVel[i] * GRANULARITY_VEL + FACTOR);
		n_xPos[i] = (uint32_t)round(old_pkt->xPos[i] / GRANULARITY_POS);
		n_yPos[i] = (uint32_t)round(old_pkt->yPos[i] / GRANULARITY_POS);
		pkt->vel[i] = 0;
		pkt->vel[i] |= n_xVel & 0xFF;
		pkt->vel[i] <<= 8;
		pkt->vel[i] |= n_yVel & 0xFF;
		pkt->players_on_floor |= (old_pkt->players_on_floor[i]) << i;
	}
	
	memset(&pkt->xPos, 0x00, sizeof(uint32_t) * 11);
	memset(&pkt->yPos, 0x00, sizeof(uint32_t) * 11);

	pkt->xPos[0] |= (n_xPos[0] & 0x7FF) << 21;
	pkt->xPos[0] |= (n_xPos[1] & 0x7FF) << 10;
	pkt->xPos[0] |= (n_xPos[2] & 0x7FE) >> 1;
	
	for(i = 2, j = 1, and_mask = 1; j < 11; and_mask += 1 << j, i+=3, j++)
	{
		pkt->xPos[j] |= (n_xPos[i] & and_mask) << (32 - j);
		pkt->xPos[j] |= (n_xPos[i + 1] & 0x7FF) << (21 - j);
		pkt->xPos[j] |= (n_xPos[i + 2] & 0x7FF) << (10 - j);
		pkt->xPos[j] |= (n_xPos[i + 3] & (0x7FE - (1 << j))) >> (j + 1);
	}
	
	pkt->yPos[0] |= (n_yPos[0] & 0x7FF) << 21;
	pkt->yPos[0] |= (n_yPos[1] & 0x7FF) << 10;
	pkt->yPos[0] |= (n_yPos[2] & 0x7FE) >> 1;

	for(i = 2, j = 1, and_mask = 1; j < 11; and_mask += 1 << j, i+=3, j++)
	{
		pkt->yPos[j] |= (n_yPos[i] & and_mask) << (32 - j);
		pkt->yPos[j] |= (n_yPos[i + 1] & 0x7FF) << (21 - j);
		pkt->yPos[j] |= (n_yPos[i + 2] & 0x7FF) << (10 - j);
		pkt->yPos[j] |= (n_yPos[i + 3] & (0x7FE - (1 << j))) >> (j + 1);
	}

	free(old_pkt);
	return pkt;
}

/**
 * This decapsulates a new packet into an old packet, allowing for
 * easy implementation to existing code.
 *
 * This function returns NULL if memory can't be allocated, but otherwise
 * returns with a valid structure. Note that the actual transmitted data may be wrong.
 *
 * Revisions: <ul>
 *					<li>March 22, 2014 - Shane Spoor - Uses loops instead of hard coding
 *                      each step, and returns a pointer to the packet.</li>
 *            </ul>
 *
 * @param pkt Pointer to the PKT_ALL_POS_UPDATE_MIN packet to be decapsulated. Must be valid.
 * @return Pointer to the decapsulated PKT_ALL_POS_UPDATE. This may contain incorrect
 *         or impossible values (according to game rules). Returns NULL if the packet
 *         couldn't be allocated.
 *
 * @designer Clark Allenby
 * @author   Clark Allenby
 */
PKT_ALL_POS_UPDATE *decapsulate_all_pos_update(PKT_ALL_POS_UPDATE_MIN *pkt) {
	PKT_ALL_POS_UPDATE *old_pkt = (PKT_ALL_POS_UPDATE*)malloc(sizeof(PKT_ALL_POS_UPDATE));
	int i, j;
	uint32_t temp, and_mask;
	
	if(!old_pkt)
		return NULL;
	
	old_pkt->floor = (floorNo_t)pkt->floor;
	
	for (i = 0; i < 32; i++) {
		old_pkt->players_on_floor[i] = (pkt->players_on_floor >> i) & 0x1;
		old_pkt->xVel[i] = (float)((((pkt->vel[i] >> 8) & 0xFF) - FACTOR) / GRANULARITY_VEL);
		old_pkt->yVel[i] = (float)(((pkt->vel[i] & 0xFF) - FACTOR) / GRANULARITY_VEL);
	}
	
	old_pkt->xPos[0] = (float)(GRANULARITY_POS * ((pkt->xPos[0] >> 21) & 0x7FF));
	old_pkt->xPos[1] = (float)(GRANULARITY_POS * ((pkt->xPos[0] >> 10) & 0x7FF));
	temp = (pkt->xPos[0] << 1) & 0x7FE;
	temp += (pkt->xPos[1] >> 31) & 0x1;

	for(i = 2, j = 1, and_mask = 3; i < 30; i += 3, j++, and_mask += 1 << j)
	{
		old_pkt->xPos[i] = (float)(GRANULARITY_POS * temp);
		old_pkt->xPos[i + 1] = (float)(GRANULARITY_POS * ((pkt->xPos[j] >> (21 - j)) & 0x7FF));
		old_pkt->xPos[i + 2] = (float)(GRANULARITY_POS * ((pkt->xPos[j] >> (10 - j)) & 0x7FF));

		temp =  (pkt->xPos[j] << (j + 1)) & (0x7FE - (1 << j));
		temp += (pkt->xPos[j + 1] >> (31 - j)) & and_mask;
	}
	
	old_pkt->yPos[0] = (float)(GRANULARITY_POS * ((pkt->yPos[0] >> 21) & 0x7FF));
	old_pkt->yPos[1] = (float)(GRANULARITY_POS * ((pkt->yPos[0] >> 10) & 0x7FF));
	temp = (pkt->yPos[0] << 1) & 0x7FE;
	temp += (pkt->yPos[1] >> 31) & 0x1;

	for(i = 2, j = 1, and_mask = 3; i < 30; i += 3, j++, and_mask += 1 << j)
	{
		old_pkt->yPos[i] = (float)(GRANULARITY_POS * temp);
		old_pkt->yPos[i + 1] = (float)(GRANULARITY_POS * ((pkt->yPos[j] >> (21 - j)) & 0x7FF));
		old_pkt->yPos[i + 2] = (float)(GRANULARITY_POS * ((pkt->yPos[j] >> (10 - j)) & 0x7FF));

		temp =  (pkt->yPos[j] << (j + 1)) & (0x7FE - (1 << j));
		temp += (pkt->yPos[j + 1] >> (31 - j)) & and_mask;
	}
	
	free(pkt);
	return old_pkt;
}

