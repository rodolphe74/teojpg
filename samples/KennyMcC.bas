1 CLS
2 L=200:C=19:S=&H4000:E=S
21 FOR Y = 1 TO L
30 FOR X = 1 TO C
31 READ A
32 GOSUB 200
33 REM PRINT E,B1,B2
35 GOSUB 300
39 E=E+1
40 NEXT X
41 E=S+40*Y
45 NEXT Y
50 END
200 B=A
210 B1=INT(B/256)
220 B2=B-(B1*256)
230 RETURN
300 Z=PEEK(&HE7C3) AND 254
310 POKE &HE7C3,Z
320 POKE E,B2
330 Z=PEEK(&HE7C3) OR 1
340 POKE &HE7C3,Z
350 POKE E,B1
360 RETURN
1029 DATA 199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199
1058 DATA 199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199
1087 DATA 199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199
1116 DATA 199,199,199,199,199,199,199,199,62201,193,57551,199,199,199,199,199,199,199,199
1145 DATA 199,199,199,199,199,199,199,63737,1241,61145,49369,32991,199,199,199,199,199,199,199
1174 DATA 199,199,199,199,199,199,199,16075,44249,37081,5849,63695,199,199,199,199,199,199,199
1203 DATA 199,199,199,199,199,199,63737,6873,8921,60619,46795,8409,32991,199,199,199,199,199,199
1232 DATA 199,199,199,199,199,199,58105,37593,46795,27353,46799,22233,61647,199,199,199,199,199,199
1261 DATA 199,199,199,199,199,199,5337,50393,14027,63179,9433,30411,48335,199,199,199,199,199,199
1290 DATA 199,199,199,199,199,64761,37593,19673,35545,25817,27339,54987,11481,32975,199,199,199,199,199
1319 DATA 199,199,199,199,199,61689,37593,21209,25817,38105,37593,46795,9433,16591,199,199,199,199,199
1348 DATA 199,199,199,199,199,15563,21209,28363,54987,46795,21209,21209,21209,30927,199,199,199,199,199
1377 DATA 199,199,199,199,199,4825,54987,60107,4825,46795,56011,9945,46795,56527,199,199,199,199,199
1406 DATA 199,199,199,199,64761,11481,37593,25817,11467,10969,46795,9433,46795,13529,199,199,199,199,199
1435 DATA 199,199,199,199,1743,34009,38105,30411,4825,17625,37593,27851,21721,35033,49359,199,199,199,199
1464 DATA 199,199,199,199,7887,13017,42201,26315,10969,21721,43737,21209,8921,23243,24783,199,199,199,199
1493 DATA 199,199,199,199,52441,46795,56011,47819,17625,37593,46795,54987,21721,46795,55503,199,199,199,199
1522 DATA 199,199,199,199,1241,14027,10969,13017,9425,37593,37593,17625,33489,38105,27851,199,199,199,199
1551 DATA 199,199,199,65273,25817,38105,28363,60619,43723,21721,37593,44249,9931,17625,28363,199,199,199,199
1580 DATA 199,199,199,64761,27339,56011,19161,58585,37593,9433,27339,61131,63179,51915,22233,32967,199,199,199
1609 DATA 199,199,199,3791,8921,46795,9433,5337,46795,46795,18681,23243,39627,729,17625,32975,199,199,199
1638 DATA 199,199,199,3279,21721,22219,13017,14027,19161,46795,56011,21209,9433,46809,21753,24783,199,199,199
1667 DATA 199,199,199,51449,19161,19161,18649,46795,56011,10969,21209,54987,54987,8921,19161,28879,199,199,199
1696 DATA 199,199,199,35545,42201,37593,47819,27339,10457,28363,46795,46795,23243,46795,46799,55503,199,199,199
1725 DATA 199,199,199,56011,11001,23243,38603,9433,27339,39641,19161,38097,9433,26315,54987,56527,199,199,199
1754 DATA 199,2255,16591,10457,37593,46795,63179,9465,53977,8921,21721,27851,21721,28363,9433,22219,199,199,199
1783 DATA 199,16631,64760,27339,46795,54991,19161,21721,4825,39627,9433,24267,56011,45771,19161,18649,199,199,199
1812 DATA 199,199,64761,21209,9433,23243,13017,46795,22219,19161,46795,19673,37593,18649,26315,9945,32504,2295,199
1841 DATA 199,199,1743,56011,21721,37593,56011,46795,9433,37593,37593,46795,21209,46795,56523,9433,32975,2255,199
1870 DATA 199,199,3791,42201,46795,38105,14027,10969,37593,37593,22233,42699,56011,54475,21241,54987,16591,199,199
1899 DATA 199,199,58617,54991,9433,46795,4825,17625,37593,19161,48843,64203,9433,37081,54987,37593,57551,199,199
1928 DATA 199,32975,59097,23243,27339,56011,13017,21721,27339,9433,39641,46297,38105,23243,42233,43737,45263,199,199
1957 DATA 199,199,16079,9433,37593,42201,27371,9433,41689,23243,9433,37597,37593,18641,38105,47819,45263,199,33015
1986 DATA 199,199,39641,19161,21209,19673,42201,37593,38105,38105,42201,37593,44235,10969,37593,42201,47311,199,199
2015 DATA 199,199,4825,37593,46799,8921,37593,38105,38105,37593,27339,11001,46795,46795,56011,51915,56527,199,32975
2044 DATA 199,8439,9433,38105,42201,38105,37593,21209,21209,38105,37585,46795,9433,37593,21721,32985,26315,199,199
2073 DATA 199,65273,19161,23243,53963,38137,43737,19161,32976,24824,61633,37593,42201,38105,37593,46809,28363,199,199
2102 DATA 199,65273,21209,46795,8409,54987,46795,63688,60099,62202,53448,13049,19161,23243,46795,4825,21209,199,199
2131 DATA 199,56569,37593,46795,53963,46795,9433,31425,48335,64760,5329,31427,54987,9433,22219,9433,38105,33015,199
2160 DATA 199,63737,46795,10969,42233,37593,37593,59075,55495,61181,46785,35024,46827,54987,56011,25817,37593,32975,199
2189 DATA 199,30971,42699,17657,37593,21721,41689,53992,57551,33007,38097,39624,56011,56011,9465,10969,42201,32975,199
2218 DATA 199,61689,33497,38105,37593,41689,17624,37584,49351,41695,58617,37585,47811,10969,46795,28363,56011,49871,199
2247 DATA 199,63225,38603,9433,37593,10969,6856,4821,33519,16591,7879,9448,30407,17625,53977,19161,21209,57551,199
2276 DATA 759,62201,19161,21209,43737,18649,33489,41176,23291,2255,31481,9432,37080,54987,4825,21721,37593,8399,199
2305 DATA 199,58105,37593,38105,37593,28363,23233,55495,4847,16591,64760,17617,21192,56015,10969,37593,38105,24783,199
2334 DATA 199,51961,46795,9433,37593,44746,63174,47311,29947,13535,16591,10952,37592,46795,9433,37625,21209,45263,16607
2363 DATA 199,14027,46795,46827,21721,39624,23233,53447,46845,8431,2255,4817,35064,64195,46795,21721,37593,45263,199
2392 DATA 199,14027,10969,19161,30411,9424,42691,7928,27387,38651,56059,57592,42184,53497,38105,30411,19161,22735,199
2421 DATA 199,38105,37593,21209,21209,38081,46273,49351,9423,37103,8911,60152,37569,24257,9465,37593,46795,55503,8439
2450 DATA 199,37593,46795,46795,9433,30435,11000,33991,33007,43743,19679,61688,28355,8912,21209,19147,54987,54475,199
2479 DATA 199,56011,46795,54987,10457,35024,38088,8927,61177,9423,55033,64248,53960,11969,4825,1241,23243,56527,199
2508 DATA 199,13529,27339,46799,44739,61130,37575,4815,16591,30459,4831,8943,9424,34520,27339,38603,9425,21707,199
2537 DATA 199,1241,42201,37593,17105,19185,64711,65274,63229,61177,37599,32504,21192,18648,17113,19161,22219,60619,199
2566 DATA 65272,19147,9433,37593,38096,54465,59591,199,4335,9439,10439,199,2776,37081,9436,30411,4825,22219,199
2595 DATA 65273,17113,46795,21721,33497,54466,45255,199,1263,38127,16607,199,24259,60611,51393,45785,42201,46795,199
2624 DATA 65273,19161,46795,9433,30434,10952,45295,199,1247,17631,16615,199,54008,21224,30411,8921,19161,42201,199
2653 DATA 719,21209,46297,37593,60097,37576,49351,199,711,17103,199,199,51448,51416,44739,54987,9433,38105,199
2682 DATA 64761,38105,33497,21209,9936,46795,16583,199,199,2255,199,199,7879,56001,23752,9433,38105,37593,199
2711 DATA 719,9433,19147,10457,51912,14017,32967,199,199,32975,199,199,60152,9432,19673,23243,46795,9433,33015
2740 DATA 64761,37593,35033,17625,26306,34520,32967,199,33007,34023,199,199,61688,21200,45249,19161,21209,23243,32975
2769 DATA 719,21209,45775,22219,59073,18641,199,199,199,4335,199,199,62200,11457,39105,57035,9465,37593,32975
2798 DATA 64249,56011,52939,63179,54467,25800,199,1247,16631,4319,199,199,1735,19160,56003,38617,38105,26315,32975
2827 DATA 1743,46799,1241,41177,50888,47818,199,199,199,8399,199,199,63736,28355,20689,37081,37593,17625,16631
2856 DATA 64249,46795,37579,9977,11464,28359,199,2295,199,199,759,199,65273,42184,27841,64203,44235,13017,32975
2885 DATA 64249,21721,48843,59080,21232,22209,199,199,199,8407,16631,199,64760,23235,46273,19161,9433,37593,32975
2914 DATA 1743,46799,19161,42712,28359,56519,199,16607,199,4295,199,4335,64760,26309,45777,46795,9433,47819,49359
2943 DATA 64249,46795,13017,52936,23240,55495,199,4335,16591,199,57085,199,65272,27841,55489,8921,43737,22219,49351
2972 DATA 64249,19165,17625,50393,42224,64711,199,199,199,8431,199,4343,65272,49873,23234,9433,28363,5337,16591
3001 DATA 62201,46795,46795,56520,8921,11459,199,199,8439,8391,2271,199,65272,46785,61123,9433,37625,42201,16591
3030 DATA 62713,42201,38105,38105,35024,63687,751,1247,199,8391,199,199,65278,4849,18641,28363,56011,61131,49359
3059 DATA 62201,21209,46795,25281,37593,47311,1271,199,199,8391,199,199,199,9432,31427,8921,18649,43737,32975
3088 DATA 64249,30411,54987,13529,46785,26823,735,57086,199,8439,1271,735,199,23240,23265,21209,43723,17657,24783
3117 DATA 3791,43723,56011,56011,2768,55495,199,64760,49351,199,7879,199,199,46787,56010,59083,8913,54987,49359
3146 DATA 64249,8921,46795,48840,25305,47303,199,63736,49351,4303,57592,65278,33007,21192,27841,41177,23243,46795,49359
3175 DATA 62201,37593,37593,34499,37576,30927,199,64760,49351,8415,57592,199,199,39624,26306,8913,18649,37593,41191
3204 DATA 62201,19673,37593,32456,45770,55503,4335,64760,49351,199,3783,199,199,45816,45761,26315,56011,21209,41167
3233 DATA 62713,24267,43723,10456,21704,55495,8415,199,199,8399,199,199,199,29893,42193,30411,9433,38617,16591
3262 DATA 62713,37593,30411,9433,30402,48327,65277,199,199,4319,199,199,199,34008,27329,20697,38105,37081,49359
3291 DATA 2767,11481,46795,9433,37624,59591,8439,199,65277,17111,199,2287,199,52931,8913,19673,37593,46283,49359
3320 DATA 1743,57035,13017,19161,21192,46817,199,32991,199,2271,199,1271,8415,33488,17616,42201,46795,54987,49359
3349 DATA 3791,54475,2265,46795,39624,27847,199,199,199,18667,199,199,199,42689,53953,28363,46795,9433,16591
3378 DATA 1231,18649,38095,31944,37080,27847,65278,199,199,34015,199,199,8423,60610,22209,46795,10969,42233,16591
3407 DATA 61689,19161,16601,10457,19160,64707,199,199,65273,9423,199,199,65278,19656,27845,46795,9433,38105,16591
3436 DATA 65277,37593,29387,56011,9416,21720,199,199,759,4319,199,199,65273,53962,17624,18649,37593,46795,49351
3465 DATA 1743,9433,42233,9433,54475,46785,199,199,1231,37071,199,199,65272,9928,56003,54987,9433,43737,32975
3494 DATA 1743,9433,28363,46799,19144,59079,2271,199,64251,65273,199,199,65272,63170,23233,28363,9433,37593,32975
3523 DATA 1743,21209,23257,54987,63175,10952,1263,199,4815,18667,33007,199,64760,37592,56011,46795,43723,56011,32975
3552 DATA 64761,35545,48843,9433,25816,4816,199,8431,4831,19167,24799,1263,64760,37592,27845,18649,30411,9433,32975
3581 DATA 64761,37593,43723,9433,18632,42200,32975,32991,18639,37615,48893,199,63736,42696,8921,46795,9425,22219,32975
3610 DATA 719,13017,37593,56011,37080,55521,32967,8439,32975,9439,4303,65278,63736,38599,5329,28363,13017,46795,32975
3639 DATA 64761,34009,37593,37081,46798,57027,49351,711,4815,9423,35039,199,3271,21704,31435,13017,27851,56011,199
3668 DATA 64761,21227,9433,29899,63170,2776,49351,2287,65272,9439,37583,4319,61688,23233,22721,17113,46795,9433,33015
3697 DATA 719,57035,10457,28363,8945,44225,57551,48857,8399,8431,37087,32967,58616,60112,55489,53977,46795,9433,199
3726 DATA 759,9425,23243,46795,22209,57026,38621,2255,63225,64249,9443,2255,58104,39624,56011,65227,10969,21209,199
3755 DATA 65273,19705,37593,2777,30411,43713,61639,751,46843,16623,19167,8939,63178,10969,46791,37579,9433,27851,199
3784 DATA 65273,21209,46799,26841,42184,37580,40131,37071,8939,2287,18639,8415,51448,44737,46795,65231,54987,46795,199
3813 DATA 199,8921,22219,1241,9921,37592,63695,37599,33007,17135,1231,37071,37592,42689,37625,30411,46795,54991,199
3842 DATA 65278,27339,4825,23275,56010,9416,18640,46843,4815,46843,8399,1231,5832,45767,4825,22233,37593,27855,199
3871 DATA 199,10457,42201,24267,60104,43225,48839,2255,17143,37583,1255,31993,22264,44232,10969,24769,38105,22731,199
3900 DATA 199,56011,9433,30411,63178,14049,9424,16623,42223,39647,37103,61176,27336,23248,42201,46809,22219,54479,199
3929 DATA 199,51929,43737,20163,9425,24259,19144,35015,65277,9423,719,63736,43718,26312,46795,2265,47819,56527,199
3958 DATA 199,51417,46795,9433,25817,21196,48323,3833,2255,8927,30459,55544,35528,37592,51929,21209,42201,26831,199
3987 DATA 199,51961,37593,9433,38105,45761,56033,57543,48893,61177,63225,7363,21753,22209,21209,43723,13529,24783,199
4016 DATA 199,54009,37593,43737,4825,28355,43720,48835,751,30459,16623,58088,1240,30411,9433,35033,33497,22735,199
4045 DATA 199,53977,21721,27851,21209,9424,46785,27842,8431,18639,1223,59075,46819,56515,57035,56011,21721,45263,199
4074 DATA 199,60153,37593,18649,9433,50393,9962,21185,33987,34031,37103,21704,33992,9425,26315,13017,37593,24783,199
4103 DATA 199,58617,37593,45771,5337,28363,17608,41168,63227,4831,64760,25296,19160,46795,9437,38105,21209,16591,199
4132 DATA 199,62713,43737,20171,17145,37593,56035,11457,5880,10447,27896,13505,46817,30411,9433,41689,42201,24775,199
4161 DATA 199,1743,9433,30411,9433,43225,56001,51909,57543,46843,57592,57027,10968,27353,31435,56011,54987,49359,199
4190 DATA 199,64249,37593,9433,37081,27851,22209,25820,7361,8911,40655,54979,43713,17625,30411,11513,46795,32975,199
4219 DATA 199,1743,44235,25817,22211,9433,26315,23240,48327,1231,38097,34000,23275,37593,13017,17113,38105,32991,199
4248 DATA 199,719,9433,62667,9433,42201,37081,63171,36040,41159,56033,9432,63179,6873,8921,43737,42201,33015,16631
4277 DATA 199,65273,45771,24267,9433,43723,20185,20696,37072,16120,11464,54472,53977,16601,26315,4313,38105,199,199
4306 DATA 199,65273,37081,38617,43723,4825,20697,11979,56515,52424,38597,35545,19161,38603,42201,21707,46795,199,199
4335 DATA 199,199,21721,37593,46795,12481,37593,6873,9425,56513,62661,45785,46795,6337,19161,18649,43211,199,199
4364 DATA 199,33015,23243,21721,37593,25817,38617,24267,25817,22211,21195,37593,38105,15555,9433,37593,23759,199,199
4393 DATA 199,199,37625,46795,37593,46795,48843,19673,27339,10457,16601,4825,19161,13049,46799,27339,59599,199,199
4422 DATA 199,199,53977,46795,60107,18649,53963,9433,35033,34009,26827,27353,25817,2777,19161,10457,20687,199,199
4451 DATA 199,199,5839,9433,41689,46283,61131,56011,43723,46795,24267,63179,3777,14027,21209,19161,45263,199,199
4480 DATA 199,199,6863,22233,43723,10457,14529,21209,46799,5337,3289,22219,31427,5337,37593,14027,41167,199,199
4509 DATA 199,199,62713,33497,46799,9945,20163,54987,56011,17625,54489,38105,6361,42201,37593,54987,49383,1271,199
4538 DATA 199,199,64761,42699,9433,22219,2777,2776,37593,19161,8921,63688,44747,56011,43723,56011,32975,199,199
4567 DATA 199,199,719,54987,9433,28363,46795,32451,21209,4825,21209,18136,38617,37593,46795,18649,32991,199,199
4596 DATA 199,199,65275,24267,43737,23243,13017,14552,2776,46296,51416,61131,9433,46795,9433,19161,199,199,199
4625 DATA 199,33015,65276,11513,46795,54987,56011,1753,32984,192,64195,22219,46795,46795,9945,44239,199,199,199
4654 DATA 199,199,199,46795,9433,30411,8921,26841,17611,8921,54987,56011,54987,11001,22219,46283,199,16631,199
4683 DATA 199,199,199,14027,9433,51929,22219,46827,65227,11481,37593,46795,56011,9433,46795,53455,2295,199,33015
4712 DATA 199,759,199,5839,43723,4825,19705,35545,46809,32456,37593,19161,21209,46795,46795,55503,199,199,199
4741 DATA 199,199,199,58617,37625,45785,47819,21209,61131,21209,43723,9465,37593,37593,46799,45263,199,199,199
4770 DATA 199,199,199,15051,9425,17113,37593,9433,14027,4825,46795,13017,27339,52427,10969,48335,199,199,199
4799 DATA 199,199,199,41689,19193,26315,13017,22219,11481,21721,37625,46795,18649,30415,9433,19659,199,199,199
4828 DATA 199,199,199,39641,21209,46795,8921,47851,8409,42233,37593,47823,56011,21721,22219,63179,199,199,33007
4857 DATA 199,199,199,61130,9433,23243,19673,22219,22233,9433,43723,38603,9433,9433,37593,43225,199,199,199
4886 DATA 199,8439,65275,13017,38105,38105,37593,46795,9433,21209,46795,63179,54987,42233,30411,9945,199,759,32991
4915 DATA 199,199,64764,35545,19161,37593,21209,19161,46795,37625,46795,19161,37593,19161,14027,8921,32975,8431,199
4944 DATA 199,199,64761,21209,17625,37625,46795,21209,46795,2777,46795,9433,46795,5337,47819,19161,32975,33015,199
4973 DATA 199,33007,1231,56011,25305,43737,38105,37593,42201,53977,10969,39627,21209,41689,13017,21241,49359,199,199
5002 DATA 199,1263,64249,19161,5337,9433,37593,38105,38105,30411,9433,37593,19161,43723,46795,46795,16591,4319,199
5031 DATA 199,199,3791,21241,37593,30411,21209,46795,9465,22219,63179,4825,46795,4313,38105,38105,57551,199,8439
5060 DATA 759,199,62201,37593,21209,19161,19161,21241,42201,37593,38105,43737,38105,27851,21209,37625,8399,199,199
5089 DATA 199,199,58617,46795,10969,17113,46795,9433,27339,9433,37593,28363,19161,37593,9433,42201,28879,65277,199
5118 DATA 199,199,60153,13003,46795,23257,46795,38105,22219,9945,46795,56011,9429,10969,19161,19161,45263,199,199
5147 DATA 199,2295,50425,8409,37593,48842,10457,37593,9433,41689,46795,9425,41689,15051,37593,41689,47311,4343,32991
5176 DATA 199,199,14027,43723,27339,53963,56523,54987,37593,26315,43723,21721,31435,9433,59083,60107,51407,8431,199
5205 DATA 199,199,39641,35033,46795,9433,37081,46795,54987,4825,9433,30411,14553,13049,50393,22219,55503,199,199
5234 DATA 199,199,37105,44235,25817,22219,22233,37592,21209,39627,25821,28363,57035,14027,9945,17625,23759,199,199
5263 DATA 65275,33007,9945,42201,56011,4825,17625,37593,19161,46795,63178,53963,19673,60107,8921,43737,44235,199,199
5292 DATA 199,199,54987,56011,46795,21209,38137,22219,56008,35024,46794,8409,46795,38603,11481,38137,30411,4319,33015
5321 DATA 199,65274,46795,54987,54987,54987,9433,27339,47833,48847,21209,22233,37593,17625,30411,8921,22219,199,16623
5350 DATA 199,65273,56011,9433,37593,47819,54987,9433,61135,27865,37593,46795,10969,19161,38635,56011,56011,199,199
5379 DATA 199,65273,21209,23243,22233,22219,46283,56011,35545,63179,11001,28363,56011,54987,8921,26315,54987,199,199
5408 DATA 199,64761,54489,19161,8409,4825,18649,9433,13529,20171,9433,21707,18649,42201,37597,18649,42201,199,199
5437 DATA 199,64761,2777,37593,42187,42201,21707,21209,31435,57035,9433,35033,43723,56011,21209,45771,9433,199,32991
5466 DATA 199,65273,54489,37593,17625,5337,28363,56011,27353,19673,42205,37593,42201,38105,30411,44747,10969,33015,199
5495 DATA 199,1743,8921,43737,46795,42713,37593,18641,9433,28363,10969,45785,46795,46795,21721,37593,46795,32975,199
5524 DATA 199,1743,4825,18649,37593,18641,9945,45771,42201,51417,46795,729,46795,54987,9433,19161,41689,33487,199
5553 DATA 199,64249,9931,9433,46297,27851,44747,48843,4825,46795,37593,56025,43737,19161,17625,28363,13529,759,199
5582 DATA 751,64249,1273,38105,34009,43737,46795,14041,43723,10457,37593,2265,37593,21209,21721,43737,33497,32975,199
5611 DATA 199,62201,16067,46795,56011,61131,11481,37593,46795,57035,43723,39627,56011,10969,37113,28363,27353,32975,199
5640 DATA 199,62713,2776,49857,26315,10969,41689,18649,42201,22219,9433,37593,37593,46795,9945,23243,31937,49359,199
5669 DATA 199,62201,35521,44739,46795,17625,19161,45771,9433,26307,9433,37593,21209,37593,18649,10457,10480,16623,199
5698 DATA 199,1735,42216,32451,56015,10969,37625,44747,56011,48843,10969,19161,19193,21209,21721,28363,46794,49351,199
5727 DATA 65278,61688,9944,6385,42201,28363,56011,9433,22219,54987,47819,46795,46795,54987,9433,42233,27844,49351,199
5756 DATA 199,3783,18649,4812,37593,19161,25817,42713,46795,54987,54987,54987,54987,56011,37625,720,21720,28871,16623
5785 DATA 199,58616,30406,19672,10969,46795,54987,8921,19161,48843,9433,27339,56011,18649,37593,44771,35560,28871,8439
5814 DATA 199,50424,53960,14017,25821,43737,37593,39641,21209,42699,10969,42201,43737,19161,21721,56001,43714,55495,16607
5843 DATA 199,49913,35544,52426,63178,9433,37625,17113,37593,28363,8921,46795,61131,56011,9433,37589,25800,55495,199
5872 DATA 199,51448,20720,19141,54987,37593,19161,21721,27339,8913,19161,46795,21209,38105,46795,4824,56003,47303,199
5901 DATA 199,14019,38081,19664,51417,21209,21209,46795,8921,44747,21209,43737,46795,46795,38105,37576,26305,64711,199
5930 DATA 199,49400,37072,37576,29899,9433,42233,37593,43737,46795,9433,37593,45803,54987,9433,10947,27843,42183,199
5959 DATA 199,54521,54977,28359,8921,19673,27339,13533,46795,46795,54987,9425,28363,9433,18649,19656,38088,64719,199
5988 DATA 199,16071,17112,60099,23257,18649,37625,17113,37593,46799,56011,9433,42233,37593,13515,52929,9937,11463,199
6017 DATA 199,54521,54978,39105,33497,23243,46795,21721,38137,24267,9433,43737,19673,38105,21209,46274,22209,59599,199
6046 DATA 199,58617,35024,47818,10969,46795,9433,37625,21209,19161,21721,37593,37593,21209,30411,54988,17105,47303,199
6075 DATA 199,7911,46280,37592,44747,54987,10969,19161,19161,44747,56011,9433,41689,37593,21209,37592,46791,55495,199
6104 DATA 199,64248,37580,34009,23243,56011,21209,46795,46827,46795,46795,9433,27339,56011,9433,37585,9425,61639,199
6133 DATA 199,63736,43713,42699,5337,42201,30415,9433,37593,18681,37593,43737,37625,46799,54987,11969,8904,12487,199
6162 DATA 199,65274,54983,13049,42201,37625,46795,21721,37593,18649,37593,46795,54987,54987,46795,37579,20175,57551,199
6191 DATA 199,199,32451,33007,46795,10969,25817,37597,43737,46795,21721,37593,46795,9433,42201,61640,26305,49351,199
6220 DATA 199,199,199,199,14023,9433,37625,37593,17625,43225,30411,46795,56011,39627,56011,26315,9456,32991,199
6249 DATA 199,199,199,199,53977,42201,37593,18649,37593,8921,21209,43723,9433,4817,54987,65274,56527,199,199
6278 DATA 199,199,199,199,14027,63179,56011,56011,26315,21209,37593,37593,21721,54489,11971,199,199,199,199
6307 DATA 199,199,199,199,54489,42201,42201,42201,9433,28363,54987,9433,37593,2265,29903,199,199,199,199
6336 DATA 199,199,199,199,58585,42201,34001,38105,41689,46795,9433,37593,46795,44751,63179,199,199,199,199
6365 DATA 199,199,199,199,14031,10969,45785,12993,10969,46795,46795,22233,46794,53963,39627,199,199,199,199
6394 DATA 199,199,199,199,53977,28363,63179,29913,21185,37593,46795,43720,56011,9433,60623,199,199,199,199
6423 DATA 199,199,199,199,14027,10969,51929,9433,9419,9437,21209,9419,21209,27851,27851,199,199,199,199
6452 DATA 199,199,199,199,6863,9433,54987,46795,4825,56011,37593,61131,9433,18649,27339,199,199,199,199
6481 DATA 199,199,199,199,14027,19161,37593,37593,17625,11979,10969,38105,27339,18649,23243,199,199,199,199
6510 DATA 199,199,199,199,14027,9433,37593,4825,27865,29387,9433,53977,18649,27851,46795,199,199,199,199
6539 DATA 199,199,199,199,10955,9433,43737,10955,729,17625,42201,4825,23243,13017,54475,199,199,199,199
6568 DATA 199,199,199,199,53977,43723,9433,17113,42187,13529,37593,54489,27339,9433,28367,199,199,199,199
6597 DATA 65278,33007,199,199,14027,46795,56011,13529,37081,34009,38105,35545,46795,46795,26827,199,199,199,16623
6626 DATA 65275,199,199,199,6863,43720,22210,9425,37585,21712,37592,61122,37608,4816,61126,33015,199,199,16607
6655 DATA 199,199,199,64760,59076,192,736,59076,720,37600,4832,32976,65218,10948,16624,49351,199,199,199
6684 DATA 199,199,199,64760,1232,9424,34000,37584,43216,46786,9456,9944,37608,37569,34008,49351,199,199,199
6713 DATA 199,2295,199,64760,8912,19144,42184,61121,20680,54465,8912,37600,17616,32976,43232,49351,199,199,199
6742 DATA 199,751,4343,199,65275,19191,49911,17119,37111,9463,33015,199,199,199,199,199,199,4343,33015
6771 DATA 199,4319,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,2287,199
6800 DATA 199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,1271,199,199