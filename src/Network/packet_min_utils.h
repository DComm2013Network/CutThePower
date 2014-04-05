#ifndef PKT_MIN_UTILS_H
#define PKT_MIN_UTILS_H

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include "Packets.h"

#define TILE_SIZE 40
#define GRANULARITY_VEL 6.35
#define GRANULARITY_POS 2.0
#define FACTOR 128

PKT_POS_UPDATE_MIN *encapsulate_pos_update(PKT_POS_UPDATE *old_pkt);
PKT_POS_UPDATE *decapsulate_pos_update(PKT_POS_UPDATE_MIN *pkt);
PKT_ALL_POS_UPDATE_MIN *encapsulate_all_pos_update(PKT_ALL_POS_UPDATE *old_pkt);
PKT_ALL_POS_UPDATE *decapsulate_all_pos_update(PKT_ALL_POS_UPDATE_MIN *pkt);

#endif
