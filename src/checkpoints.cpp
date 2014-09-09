// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64 nTimeLastCheckpoint;
        int64 nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (     0, hashGenesisBlock)
        (  	  1, uint256("0xe8666c8715fafbfb095132deb1dd2af63fe14d3d7163715341d48feffab458cc"))
        (  	 25, uint256("0xe49cfc3e60515965380cbc3a1add5ab007e5bd2f226624cad9ff0f79eef680cc"))
        (  	 50, uint256("0x0b082428186ab2dc55403b2b3c9bd14f087590b204e05c09a656914285520b4d"))
        (    98, uint256("0xd27e483ae4d334cc65575bcc66d65f7a97913f31188662e2d3fe329675714128"))
        ( 25000, uint256("0x76d94f81bf598f915b68a57db229ff015551fb175167546363e12d7e86226099"))
        ( 75000, uint256("0xdc26dd1c5c53d0e09ce3eec73107423aa518b8a2c0ebe47e6d4987866d68b881"))
        (100000, uint256("0x68d5027a570c605f6a0d24f8bad5c454769438eb4a237e93b4ee7a638eaa01b0"))
        (125000, uint256("0x28cf8f91b29aae787b20c1c915d1cc29283b0ee4c517c5908d6c4b1017c05ee9"))
        (150000, uint256("0xa9d3915cc6c9a18a6fe72429d496c985308c5335e60afe616fe6c8123c6e624f"))
        (175000, uint256("0xbd7448096c4323e765bba6ce2cef0f4affc4e76f661002dda9154c7e583a0434"))
        (200000, uint256("0x5f295d3a00a74641d9fda7bf538585456b30261d20bf559c4f4ca30a949062fe"))
        (225000, uint256("0xa4ccb19c88086010441a3262cc61e99fee2981da43e6172024814431c480dc88"))
        (250000, uint256("0xfacb5fd3f8e1053adeec85e780021c86a1e850d33b1e2d405c439789e838c5b0"))
        (290000, uint256("0x815cde17499d0c13689df3c567b55a34e3b801cd3ef539ffd39bf4acbe17db47"))
        ;
    static const CCheckpointData data = {
        &mapCheckpoints,
        1401873728, // * UNIX timestamp of last checkpoint block
        746473,     // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        2160.0      // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        (     0, uint256("0x"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1371387277,
        547,
        2160
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (!GetBoolArg("-checkpoints", true))
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL)
            return 0.0;

        int64 nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (!GetBoolArg("-checkpoints", true))
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}
