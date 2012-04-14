/* 
 *  Copyright (c) 2012 Daisuke Okanohara
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 * 
 *   1. Redistributions of source code must retain the above Copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above Copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 */

#include <iostream>
#include "../lib/RSDic.hpp"
#include "../lib/RSDicBuilder.hpp"

#include <time.h>
#include <sys/time.h>
#include <stdio.h>

double gettimeofday_sec()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + (double)tv.tv_usec*1e-6;
}


using namespace std;
using namespace rsdic;

int main(int argc, char* argv[]){
  if (argc != 3){
    cerr << argv[0] << " num ratio" << endl;
    return -1;
  }
  uint64_t num = atoll(argv[1]);
  float ratio = atof(argv[2]);

  RSDicBuilder bvb;
  for (uint64_t i = 0; i < num; ++i){
    float r = (float)rand() / RAND_MAX;
    if (r < ratio) bvb.PushBack(1);
    else bvb.PushBack(0);
  }


  RSDic bv;
  bvb.Build(bv);

  uint64_t dummy = 0;


  cout << (float)bv.GetUsageBytes() / (num / 8) << "\t";

  double start = gettimeofday_sec();
  for (uint64_t i = 0; i < 1000000; ++i){
    uint64_t pos = rand() % num;
    dummy += bv.GetBit(pos);
  }
  cout << gettimeofday_sec() - start << "\t";

  start = gettimeofday_sec();
  for (uint64_t i = 0; i < 1000000; ++i){
    uint64_t pos = rand() % num;
    dummy += bv.Rank(pos, 1);
  }
  cout << gettimeofday_sec() - start << "\t";

  uint64_t one_num = bv.one_num();
  start = gettimeofday_sec();
  for (uint64_t i = 0; i < 1000000; ++i){
    uint64_t pos = rand() % one_num;
    dummy += bv.Select(pos, 1);
  }
  cout << gettimeofday_sec() - start << endl;

  if (dummy == 777){
    cout << "your very lucky" << endl;
  }

  return 0;
}
