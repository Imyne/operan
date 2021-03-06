/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/* file: crc_byte.c
   purpose: generate 3GPP LTE CRCs. Byte-oriented implementation of CRC's
   author: raymond.knopp@eurecom.fr
   date: 21.10.2009

   Original UMTS version by
   P. Humblet
   May 10, 2001
   Modified in June, 2001, to include  the length non multiple of 8
*/

#include "PHY/types.h"

// For initialization && verification purposes, bit by bit implementation with any polynomial
// The first bit is in the MSB of each byte

// Reference 38.212 V15.1.1 Section 5.1 (36-212 v8.6.0 , pp 8-9)
// The highest degree is set by default
/** 1000 0110 0100 1100 1111 1011  D^24 + D^23 + D^18 + D^17 + D^14 + D^11 + D^10 + D^7 + D^6 + D^5 + D^4 + D^3 + D + 1 */
static const uint32_t poly24a = 0x864cfb00;
/** 1000 0000 0000 0000 0110 0011  D^24 + D^23 + D^6 + D^5 + D + 1 */
static const uint32_t poly24b = 0x80006300;
/** 0001 0000 0010 0001            D^16 + D^12 + D^5 + 1 */
static const uint32_t poly16  = 0x10210000;
/** 1000 0000 1111                 D^12 + D^11 + D^3 + D^2 + D + 1 */
static const uint32_t poly12  = 0x80F00000;
/** 1001 1011                      D^8  + D^7  + D^4 + D^3 + D + 1 */
static const uint32_t poly8   = 0x9B000000;

// The following arrays are generated with the function 'crcTableInit'
/** Encoding table for CRC 24A */
static const uint32_t crc24aTable[256] = {0x0, 0x864cfb00, 0x8ad50d00, 0xc99f600, 0x93e6e100, 0x15aa1a00, 0x1933ec00, 0x9f7f1700, 0xa1813900, 0x27cdc200, 0x2b543400, 0xad18cf00, 0x3267d800, 0xb42b2300, 0xb8b2d500, 0x3efe2e00, 0xc54e8900, 0x43027200, 0x4f9b8400, 0xc9d77f00, 0x56a86800, 0xd0e49300, 0xdc7d6500, 0x5a319e00, 0x64cfb000, 0xe2834b00, 0xee1abd00, 0x68564600, 0xf7295100, 0x7165aa00, 0x7dfc5c00, 0xfbb0a700, 0xcd1e900, 0x8a9d1200, 0x8604e400, 0x481f00, 0x9f370800, 0x197bf300, 0x15e20500, 0x93aefe00, 0xad50d000, 0x2b1c2b00, 0x2785dd00, 0xa1c92600, 0x3eb63100, 0xb8faca00, 0xb4633c00, 0x322fc700, 0xc99f6000, 0x4fd39b00, 0x434a6d00, 0xc5069600, 0x5a798100, 0xdc357a00, 0xd0ac8c00, 0x56e07700, 0x681e5900, 0xee52a200, 0xe2cb5400, 0x6487af00, 0xfbf8b800, 0x7db44300, 0x712db500, 0xf7614e00, 0x19a3d200, 0x9fef2900, 0x9376df00, 0x153a2400, 0x8a453300, 0xc09c800, 0x903e00, 0x86dcc500, 0xb822eb00, 0x3e6e1000, 0x32f7e600, 0xb4bb1d00, 0x2bc40a00, 0xad88f100, 0xa1110700, 0x275dfc00, 0xdced5b00, 0x5aa1a000, 0x56385600, 0xd074ad00, 0x4f0bba00, 0xc9474100, 0xc5deb700, 0x43924c00, 0x7d6c6200, 0xfb209900, 0xf7b96f00, 0x71f59400, 0xee8a8300, 0x68c67800, 0x645f8e00, 0xe2137500, 0x15723b00, 0x933ec000, 0x9fa73600, 0x19ebcd00, 0x8694da00, 0xd82100, 0xc41d700, 0x8a0d2c00, 0xb4f30200, 0x32bff900, 0x3e260f00, 0xb86af400, 0x2715e300, 0xa1591800, 0xadc0ee00, 0x2b8c1500, 0xd03cb200, 0x56704900, 0x5ae9bf00, 0xdca54400, 0x43da5300, 0xc596a800, 0xc90f5e00, 0x4f43a500, 0x71bd8b00, 0xf7f17000, 0xfb688600, 0x7d247d00, 0xe25b6a00, 0x64179100, 0x688e6700, 0xeec29c00, 0x3347a400, 0xb50b5f00, 0xb992a900, 0x3fde5200, 0xa0a14500, 0x26edbe00, 0x2a744800, 0xac38b300, 0x92c69d00, 0x148a6600, 0x18139000, 0x9e5f6b00, 0x1207c00, 0x876c8700, 0x8bf57100, 0xdb98a00, 0xf6092d00, 0x7045d600, 0x7cdc2000, 0xfa90db00, 0x65efcc00, 0xe3a33700, 0xef3ac100, 0x69763a00, 0x57881400, 0xd1c4ef00, 0xdd5d1900, 0x5b11e200, 0xc46ef500, 0x42220e00, 0x4ebbf800, 0xc8f70300, 0x3f964d00, 0xb9dab600, 0xb5434000, 0x330fbb00, 0xac70ac00, 0x2a3c5700, 0x26a5a100, 0xa0e95a00, 0x9e177400, 0x185b8f00, 0x14c27900, 0x928e8200, 0xdf19500, 0x8bbd6e00, 0x87249800, 0x1686300, 0xfad8c400, 0x7c943f00, 0x700dc900, 0xf6413200, 0x693e2500, 0xef72de00, 0xe3eb2800, 0x65a7d300, 0x5b59fd00, 0xdd150600, 0xd18cf000, 0x57c00b00, 0xc8bf1c00, 0x4ef3e700, 0x426a1100, 0xc426ea00, 0x2ae47600, 0xaca88d00, 0xa0317b00, 0x267d8000, 0xb9029700, 0x3f4e6c00, 0x33d79a00, 0xb59b6100, 0x8b654f00, 0xd29b400, 0x1b04200, 0x87fcb900, 0x1883ae00, 0x9ecf5500, 0x9256a300, 0x141a5800, 0xefaaff00, 0x69e60400, 0x657ff200, 0xe3330900, 0x7c4c1e00, 0xfa00e500, 0xf6991300, 0x70d5e800, 0x4e2bc600, 0xc8673d00, 0xc4fecb00, 0x42b23000, 0xddcd2700, 0x5b81dc00, 0x57182a00, 0xd154d100, 0x26359f00, 0xa0796400, 0xace09200, 0x2aac6900, 0xb5d37e00, 0x339f8500, 0x3f067300, 0xb94a8800, 0x87b4a600, 0x1f85d00, 0xd61ab00, 0x8b2d5000, 0x14524700, 0x921ebc00, 0x9e874a00, 0x18cbb100, 0xe37b1600, 0x6537ed00, 0x69ae1b00, 0xefe2e000, 0x709df700, 0xf6d10c00, 0xfa48fa00, 0x7c040100, 0x42fa2f00, 0xc4b6d400, 0xc82f2200, 0x4e63d900, 0xd11cce00, 0x57503500, 0x5bc9c300, 0xdd853800};

/** Encoding table for CRC 24B */
static const uint32_t crc24bTable[256] = {0x0, 0x80006300, 0x8000a500, 0xc600, 0x80012900, 0x14a00, 0x18c00, 0x8001ef00, 0x80023100, 0x25200, 0x29400, 0x8002f700, 0x31800, 0x80037b00, 0x8003bd00, 0x3de00, 0x80040100, 0x46200, 0x4a400, 0x8004c700, 0x52800, 0x80054b00, 0x80058d00, 0x5ee00, 0x63000, 0x80065300, 0x80069500, 0x6f600, 0x80071900, 0x77a00, 0x7bc00, 0x8007df00, 0x80086100, 0x80200, 0x8c400, 0x8008a700, 0x94800, 0x80092b00, 0x8009ed00, 0x98e00, 0xa5000, 0x800a3300, 0x800af500, 0xa9600, 0x800b7900, 0xb1a00, 0xbdc00, 0x800bbf00, 0xc6000, 0x800c0300, 0x800cc500, 0xca600, 0x800d4900, 0xd2a00, 0xdec00, 0x800d8f00, 0x800e5100, 0xe3200, 0xef400, 0x800e9700, 0xf7800, 0x800f1b00, 0x800fdd00, 0xfbe00, 0x8010a100, 0x10c200, 0x100400, 0x80106700, 0x118800, 0x8011eb00, 0x80112d00, 0x114e00, 0x129000, 0x8012f300, 0x80123500, 0x125600, 0x8013b900, 0x13da00, 0x131c00, 0x80137f00, 0x14a000, 0x8014c300, 0x80140500, 0x146600, 0x80158900, 0x15ea00, 0x152c00, 0x80154f00, 0x80169100, 0x16f200, 0x163400, 0x80165700, 0x17b800, 0x8017db00, 0x80171d00, 0x177e00, 0x18c000, 0x8018a300, 0x80186500, 0x180600, 0x8019e900, 0x198a00, 0x194c00, 0x80192f00, 0x801af100, 0x1a9200, 0x1a5400, 0x801a3700, 0x1bd800, 0x801bbb00, 0x801b7d00, 0x1b1e00, 0x801cc100, 0x1ca200, 0x1c6400, 0x801c0700, 0x1de800, 0x801d8b00, 0x801d4d00, 0x1d2e00, 0x1ef000, 0x801e9300, 0x801e5500, 0x1e3600, 0x801fd900, 0x1fba00, 0x1f7c00, 0x801f1f00, 0x80212100, 0x214200, 0x218400, 0x8021e700, 0x200800, 0x80206b00, 0x8020ad00, 0x20ce00, 0x231000, 0x80237300, 0x8023b500, 0x23d600, 0x80223900, 0x225a00, 0x229c00, 0x8022ff00, 0x252000, 0x80254300, 0x80258500, 0x25e600, 0x80240900, 0x246a00, 0x24ac00, 0x8024cf00, 0x80271100, 0x277200, 0x27b400, 0x8027d700, 0x263800, 0x80265b00, 0x80269d00, 0x26fe00, 0x294000, 0x80292300, 0x8029e500, 0x298600, 0x80286900, 0x280a00, 0x28cc00, 0x8028af00, 0x802b7100, 0x2b1200, 0x2bd400, 0x802bb700, 0x2a5800, 0x802a3b00, 0x802afd00, 0x2a9e00, 0x802d4100, 0x2d2200, 0x2de400, 0x802d8700, 0x2c6800, 0x802c0b00, 0x802ccd00, 0x2cae00, 0x2f7000, 0x802f1300, 0x802fd500, 0x2fb600, 0x802e5900, 0x2e3a00, 0x2efc00, 0x802e9f00, 0x318000, 0x8031e300, 0x80312500, 0x314600, 0x8030a900, 0x30ca00, 0x300c00, 0x80306f00, 0x8033b100, 0x33d200, 0x331400, 0x80337700, 0x329800, 0x8032fb00, 0x80323d00, 0x325e00, 0x80358100, 0x35e200, 0x352400, 0x80354700, 0x34a800, 0x8034cb00, 0x80340d00, 0x346e00, 0x37b000, 0x8037d300, 0x80371500, 0x377600, 0x80369900, 0x36fa00, 0x363c00, 0x80365f00, 0x8039e100, 0x398200, 0x394400, 0x80392700, 0x38c800, 0x8038ab00, 0x80386d00, 0x380e00, 0x3bd000, 0x803bb300, 0x803b7500, 0x3b1600, 0x803af900, 0x3a9a00, 0x3a5c00, 0x803a3f00, 0x3de000, 0x803d8300, 0x803d4500, 0x3d2600, 0x803cc900, 0x3caa00, 0x3c6c00, 0x803c0f00, 0x803fd100, 0x3fb200, 0x3f7400, 0x803f1700, 0x3ef800, 0x803e9b00, 0x803e5d00, 0x3e3e00};

/** Encoding table for CRC 16 */
static const uint16_t crc16Table[256] = {0x0, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0xa50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0xc60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0xe70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0xa1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x2b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x8e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0xaf1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0xcc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0xed1, 0x1ef0};

/** Encoding table for CRC 12 */
static const uint16_t crc12Table[256] = {0x0, 0x80f0, 0x8110, 0x1e0, 0x82d0, 0x220, 0x3c0, 0x8330, 0x8550, 0x5a0, 0x440, 0x84b0, 0x780, 0x8770, 0x8690, 0x660, 0x8a50, 0xaa0, 0xb40, 0x8bb0, 0x880, 0x8870, 0x8990, 0x960, 0xf00, 0x8ff0, 0x8e10, 0xee0, 0x8dd0, 0xd20, 0xcc0, 0x8c30, 0x9450, 0x14a0, 0x1540, 0x95b0, 0x1680, 0x9670, 0x9790, 0x1760, 0x1100, 0x91f0, 0x9010, 0x10e0, 0x93d0, 0x1320, 0x12c0, 0x9230, 0x1e00, 0x9ef0, 0x9f10, 0x1fe0, 0x9cd0, 0x1c20, 0x1dc0, 0x9d30, 0x9b50, 0x1ba0, 0x1a40, 0x9ab0, 0x1980, 0x9970, 0x9890, 0x1860, 0xa850, 0x28a0, 0x2940, 0xa9b0, 0x2a80, 0xaa70, 0xab90, 0x2b60, 0x2d00, 0xadf0, 0xac10, 0x2ce0, 0xafd0, 0x2f20, 0x2ec0, 0xae30, 0x2200, 0xa2f0, 0xa310, 0x23e0, 0xa0d0, 0x2020, 0x21c0, 0xa130, 0xa750, 0x27a0, 0x2640, 0xa6b0, 0x2580, 0xa570, 0xa490, 0x2460, 0x3c00, 0xbcf0, 0xbd10, 0x3de0, 0xbed0, 0x3e20, 0x3fc0, 0xbf30, 0xb950, 0x39a0, 0x3840, 0xb8b0, 0x3b80, 0xbb70, 0xba90, 0x3a60, 0xb650, 0x36a0, 0x3740, 0xb7b0, 0x3480, 0xb470, 0xb590, 0x3560, 0x3300, 0xb3f0, 0xb210, 0x32e0, 0xb1d0, 0x3120, 0x30c0, 0xb030, 0xd050, 0x50a0, 0x5140, 0xd1b0, 0x5280, 0xd270, 0xd390, 0x5360, 0x5500, 0xd5f0, 0xd410, 0x54e0, 0xd7d0, 0x5720, 0x56c0, 0xd630, 0x5a00, 0xdaf0, 0xdb10, 0x5be0, 0xd8d0, 0x5820, 0x59c0, 0xd930, 0xdf50, 0x5fa0, 0x5e40, 0xdeb0, 0x5d80, 0xdd70, 0xdc90, 0x5c60, 0x4400, 0xc4f0, 0xc510, 0x45e0, 0xc6d0, 0x4620, 0x47c0, 0xc730, 0xc150, 0x41a0, 0x4040, 0xc0b0, 0x4380, 0xc370, 0xc290, 0x4260, 0xce50, 0x4ea0, 0x4f40, 0xcfb0, 0x4c80, 0xcc70, 0xcd90, 0x4d60, 0x4b00, 0xcbf0, 0xca10, 0x4ae0, 0xc9d0, 0x4920, 0x48c0, 0xc830, 0x7800, 0xf8f0, 0xf910, 0x79e0, 0xfad0, 0x7a20, 0x7bc0, 0xfb30, 0xfd50, 0x7da0, 0x7c40, 0xfcb0, 0x7f80, 0xff70, 0xfe90, 0x7e60, 0xf250, 0x72a0, 0x7340, 0xf3b0, 0x7080, 0xf070, 0xf190, 0x7160, 0x7700, 0xf7f0, 0xf610, 0x76e0, 0xf5d0, 0x7520, 0x74c0, 0xf430, 0xec50, 0x6ca0, 0x6d40, 0xedb0, 0x6e80, 0xee70, 0xef90, 0x6f60, 0x6900, 0xe9f0, 0xe810, 0x68e0, 0xebd0, 0x6b20, 0x6ac0, 0xea30, 0x6600, 0xe6f0, 0xe710, 0x67e0, 0xe4d0, 0x6420, 0x65c0, 0xe530, 0xe350, 0x63a0, 0x6240, 0xe2b0, 0x6180, 0xe170, 0xe090, 0x6060};

/** Encoding table for CRC 8 */
static const uint8_t crc8Table[256] = {0x0, 0x9b, 0xad, 0x36, 0xc1, 0x5a, 0x6c, 0xf7, 0x19, 0x82, 0xb4, 0x2f, 0xd8, 0x43, 0x75, 0xee, 0x32, 0xa9, 0x9f, 0x4, 0xf3, 0x68, 0x5e, 0xc5, 0x2b, 0xb0, 0x86, 0x1d, 0xea, 0x71, 0x47, 0xdc, 0x64, 0xff, 0xc9, 0x52, 0xa5, 0x3e, 0x8, 0x93, 0x7d, 0xe6, 0xd0, 0x4b, 0xbc, 0x27, 0x11, 0x8a, 0x56, 0xcd, 0xfb, 0x60, 0x97, 0xc, 0x3a, 0xa1, 0x4f, 0xd4, 0xe2, 0x79, 0x8e, 0x15, 0x23, 0xb8, 0xc8, 0x53, 0x65, 0xfe, 0x9, 0x92, 0xa4, 0x3f, 0xd1, 0x4a, 0x7c, 0xe7, 0x10, 0x8b, 0xbd, 0x26, 0xfa, 0x61, 0x57, 0xcc, 0x3b, 0xa0, 0x96, 0xd, 0xe3, 0x78, 0x4e, 0xd5, 0x22, 0xb9, 0x8f, 0x14, 0xac, 0x37, 0x1, 0x9a, 0x6d, 0xf6, 0xc0, 0x5b, 0xb5, 0x2e, 0x18, 0x83, 0x74, 0xef, 0xd9, 0x42, 0x9e, 0x5, 0x33, 0xa8, 0x5f, 0xc4, 0xf2, 0x69, 0x87, 0x1c, 0x2a, 0xb1, 0x46, 0xdd, 0xeb, 0x70, 0xb, 0x90, 0xa6, 0x3d, 0xca, 0x51, 0x67, 0xfc, 0x12, 0x89, 0xbf, 0x24, 0xd3, 0x48, 0x7e, 0xe5, 0x39, 0xa2, 0x94, 0xf, 0xf8, 0x63, 0x55, 0xce, 0x20, 0xbb, 0x8d, 0x16, 0xe1, 0x7a, 0x4c, 0xd7, 0x6f, 0xf4, 0xc2, 0x59, 0xae, 0x35, 0x3, 0x98, 0x76, 0xed, 0xdb, 0x40, 0xb7, 0x2c, 0x1a, 0x81, 0x5d, 0xc6, 0xf0, 0x6b, 0x9c, 0x7, 0x31, 0xaa, 0x44, 0xdf, 0xe9, 0x72, 0x85, 0x1e, 0x28, 0xb3, 0xc3, 0x58, 0x6e, 0xf5, 0x2, 0x99, 0xaf, 0x34, 0xda, 0x41, 0x77, 0xec, 0x1b, 0x80, 0xb6, 0x2d, 0xf1, 0x6a, 0x5c, 0xc7, 0x30, 0xab, 0x9d, 0x6, 0xe8, 0x73, 0x45, 0xde, 0x29, 0xb2, 0x84, 0x1f, 0xa7, 0x3c, 0xa, 0x91, 0x66, 0xfd, 0xcb, 0x50, 0xbe, 0x25, 0x13, 0x88, 0x7f, 0xe4, 0xd2, 0x49, 0x95, 0xe, 0x38, 0xa3, 0x54, 0xcf, 0xf9, 0x62, 0x8c, 0x17, 0x21, 0xba, 0x4d, 0xd6, 0xe0, 0x7b};


uint32_t crcbit (uint8_t * inputptr, int32_t octetlen, uint32_t poly)
{
    uint32_t i;
    uint32_t crc = 0;
    uint32_t c;

    while (octetlen-- > 0)
    {
        c = (*inputptr++) << 24;

        for (i = 8; i != 0; i--)
        {
            if ( (1 << 31) & (c ^ crc) )
            {
                crc = (crc << 1) ^ poly;
            }
            else
            {
                crc <<= 1;
            }

            c <<= 1;
        }
    }

    return crc;
}

// CRC table initialization

/*
void crcTableInit (void)
{
    uint8_t c = 0;

    do
    {
        crc24aTable[c] = crcbit (&c, 1, poly24a);
        crc24bTable[c] = crcbit (&c, 1, poly24b);
        crc16Table [c] = (uint16_t) (crcbit (&c, 1, poly16) >> 16);
        crc12Table [c] = (uint16_t) (crcbit (&c, 1, poly12) >> 16);
        crc8Table  [c] = (uint8_t)  (crcbit (&c, 1, poly8 ) >> 24);
    }
    while (++c);
}
*/

// Byte by byte implementations, assuming initial byte is 0 padded (in MSB) if necessary
uint32_t crc24a (uint8_t * inptr, uint32_t bitlen)
{
    int32_t octetlen = bitlen / 8;
    int32_t resbit = (bitlen % 8);
    uint32_t crc = 0;

    while (octetlen-- > 0)
    {
        crc = (crc << 8) ^ crc24aTable[(*inptr++) ^ (crc >> 24)];
    }

    if (resbit > 0)
    {
        crc = (crc << resbit) ^ crc24aTable[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))];
    }

    return crc;
}

uint32_t crc24b (uint8_t * inptr, uint32_t bitlen)
{
    int32_t octetlen = bitlen / 8;
    int32_t resbit = (bitlen % 8);
    uint32_t crc = 0;

    while (octetlen-- > 0)
    {
        crc = (crc << 8) ^ crc24bTable[(*inptr++) ^ (crc >> 24)];
    }

    if (resbit > 0)
    {
        crc = (crc << resbit) ^ crc24bTable[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))];
    }

    return crc;
}

uint32_t crc16 (uint8_t * inptr, uint32_t bitlen)
{
    int32_t octetlen = bitlen / 8;
    int32_t resbit = (bitlen % 8);
    uint32_t crc = 0;

    while (octetlen-- > 0)
    {
        crc = (crc << 8) ^ (crc16Table[(*inptr++) ^ (crc >> 24)] << 16);
    }

    if (resbit > 0)
    {
        crc = (crc << resbit) ^ (crc16Table[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))] << 16);
    }

    return crc;
}

uint32_t crc12 (uint8_t * inptr, uint32_t bitlen)
{
    int32_t octetlen = bitlen / 8;
    int32_t resbit = (bitlen % 8);
    uint32_t crc = 0;

    while (octetlen-- > 0)
    {
        crc = (crc << 8) ^ (crc12Table[(*inptr++) ^ (crc >> 24)] << 16);
    }

    if (resbit > 0)
    {
        crc = (crc << resbit) ^ (crc12Table[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))] << 16);
    }

    return crc;
}

uint32_t crc8 (uint8_t * inptr, uint32_t bitlen)
{
    int32_t octetlen = bitlen / 8;
    int32_t resbit = (bitlen % 8);
    uint32_t crc = 0;

    while (octetlen-- > 0)
    {
        crc = crc8Table[(*inptr++) ^ (crc >> 24)] << 24;
    }

    if (resbit > 0)
    {
        crc = (crc << resbit) ^ (crc8Table[((*inptr) >> (8 - resbit)) ^ (crc >> (32 - resbit))] << 24);
    }

    return crc;
}

#ifdef DEBUG_CRC
// Test code

#include <stdio.h>

main()
{
    unsigned char test[] = "Thebigredfox";
    crcTableInit();
    printf("%x\n", crcbit(test, sizeof(test) - 1, poly24));
    printf("%x\n", crc24 (test, (sizeof(test) - 1)*8));
    printf("%x\n", crcbit(test, sizeof(test) - 1, poly8));
    printf("%x\n", crc8  (test, (sizeof(test) - 1)*8));
}
#endif
