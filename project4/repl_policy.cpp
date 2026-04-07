#include "repl_policy.h"

// =========================================================
// TODO: Task 1 / Task 3 replacement policies
// Implement LRU first, then extend with SRRIP / BIP.
// =========================================================

void LRUPolicy::onHit(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    auto& line = set[way];
    line.last_access = cycle;
    // TODO: mark the hit line as most recently used.
}

void LRUPolicy::onMiss(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    auto& line = set[way];
    if(!line.valid) line.valid = true;
    line.last_access = cycle;
    // TODO: initialize a newly inserted line as MRU.
}

int LRUPolicy::getVictim(std::vector<CacheLine>& set) {
    // TODO: return the least recently used way.
    int index = 0;
    uint64_t least = set[0].last_access;
    for(size_t i = 0; i < set.size(); i++){
        auto latest = set[i].last_access;
        if(least > latest){
            least = latest;
            index = i;
        }
    }
    return index;
}

void SRRIPPolicy::onHit(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    auto& line = set[way];
    line.rrpv = 0;
    // TODO: typically promote the line to RRPV=0.
}

void SRRIPPolicy::onMiss(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    auto& line = set[way];
    if(!line.valid) line.valid = true;
    line.rrpv = 2;
    // TODO: insert with a long re-reference interval, e.g. RRPV=2.
}

int SRRIPPolicy::getVictim(std::vector<CacheLine>& set) {
    // TODO: search for RRPV==3, otherwise age all lines and retry.
    while (true) {
        for (size_t i = 0; i < set.size(); ++i) {
            if (set[i].rrpv >= 3) {
                return static_cast<int>(i);
            }
        }

        for (auto& line : set) {
            if (line.rrpv < 3) {
                line.rrpv++;
            }
        }
    }
}

void BIPPolicy::onHit(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    auto& line = set[way];
    line.last_access = cycle;   
    // TODO: hits still become MRU.
}

void BIPPolicy::onMiss(std::vector<CacheLine>& set, int way, uint64_t cycle) {
    // TODO: mostly insert at LRU position, but occasionally insert at MRU.
    // Hint: use insertion_counter and throttle.
    auto& line = set[way];
    insertion_counter++;
    if((insertion_counter & (throttle-1)) == 0){
        line.last_access = cycle; 
        insertion_counter = 0;
    } else {
        uint64_t least = set[0].last_access;
        for(size_t i = 0; i < set.size(); i++){
            auto latest = set[i].last_access;
            if(least > latest){
                least = latest;
            }
        }

        line.last_access = (least == 0) ? 0 : least - 1; // Insert at LRU position         
    }
    if(!line.valid) line.valid = true;
}

int BIPPolicy::getVictim(std::vector<CacheLine>& set) {
    // TODO: BIP usually uses the same victim selection as LRU.
    int index = 0;
    uint64_t least = set[0].last_access;
    for(size_t i = 0; i < set.size(); i++){
        auto latest = set[i].last_access;
        if(least > latest){
            least = latest;
            index = i;
        }
    }
    return index;
}

ReplacementPolicy* createReplacementPolicy(std::string name) {
    if (name == "SRRIP") return new SRRIPPolicy();
    if (name == "BIP") return new BIPPolicy();
    return new LRUPolicy();
}

