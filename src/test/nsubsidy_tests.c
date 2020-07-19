#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

static const int64_t COIN = 100000000;
const int nSubsidyHalvingInterval = 525600;

int64_t GetBlockSubsidy(int nHeight, bool fSuperblockPartOnly)
{
    int64_t nSubsidy = 10 * COIN;
    if (nHeight >= 175000) nSubsidy = 5 * COIN;

    int halvings = (nHeight - 175000) / nSubsidyHalvingInterval;
    // Force block reward to zero when right shift is undefined.
    if (halvings >= 64)
        return 0;

    // Subsidy is cut in half every 525,600 blocks which will occur approximately every 2 years.
    nSubsidy >>= halvings;

    // Hard fork to reduce the block reward by 10 extra percent, then later on 20 (at 175000 onwards)
    int64_t nSuperblockPart = 0;
    if (nHeight < 175000)
        nSuperblockPart = (nHeight >= 10000) ? nSubsidy/10 : 0; //! equiv 10% (ie. 20 / 10 = 2, or 10% of 20 is 2)
    else
        nSuperblockPart = nSubsidy/5;                                                               //! equiv 20% (ie. 20 / 5 = 4,  or 20% of 20 is 4)

    return fSuperblockPartOnly ? nSuperblockPart : nSubsidy - nSuperblockPart;
}

//  height 00000001 nSubsidy 1000000000 nSuperBlk 0
//  height 00010000 nSubsidy 900000000 nSuperBlk 100000000
//  height 00175000 nSubsidy 400000000 nSuperBlk 100000000
//  height 00700600 nSubsidy 200000000 nSuperBlk 50000000
//  height 01226200 nSubsidy 100000000 nSuperBlk 25000000
//  height 01751800 nSubsidy 50000000 nSuperBlk 12500000

int main()
{
    int64_t lastSubsidy = -1;
    int64_t lastSuperblock = -1;

    for (int i=1; i < 1901800; i++) {
        int64_t nSubsidy = GetBlockSubsidy(i, false);
        int64_t nSuperblock = GetBlockSubsidy(i, true);

        if (nSubsidy != lastSubsidy || lastSuperblock != nSuperblock)
            printf("height %08d nSubsidy %llu nSuperBlk %llu\n", i, nSubsidy, nSuperblock);

        lastSubsidy = nSubsidy;
        lastSuperblock = nSuperblock;
    }

    return 0;
}
