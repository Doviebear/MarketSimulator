// PROJECT IDENTIFIER: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#include <vector>
#include <iostream>
#include <algorithm> // std::sort
#include <getopt.h>
#include <queue> 
#include <string>  
#include "Planet.h"

using namespace std;

bool Planet::checkForBattle() const {
    if(jediDeploys.empty() || sithDeploys.empty()) {
        return false;
    }
    if(jediDeploys.top().forceSen <= sithDeploys.top().forceSen){
        return true;
    } else {
        return false;
    }
}

//Returns number of battles resolved
uint32_t Planet::resolveBattles(vector<General>& genVec) {
    uint32_t battlesResolved = 0;
    if(checkForBattle()) {
        ++battlesResolved;
        Deployment jediBat = jediDeploys.top();
        Deployment sithBat = sithDeploys.top();
        jediDeploys.pop();
        sithDeploys.pop();

        double numTroopsLost = 0;

        if(jediBat.quantity > sithBat.quantity) {
            jediBat.quantity -= sithBat.quantity;
            numTroopsLost += (sithBat.quantity * 2);

            if(oArgs.verbose) {
                cout << "Buyer " << sithBat.UID << " bought seller " << jediBat.UID;
                cout << "'s commodity " << id << " for price " << jediBat.forceSen << ". " << numTroopsLost << " units were traded.\n";

            }
            if (oArgs.median) {
                addToMedian(numTroopsLost);
            }

            battlesResolved += addJediDeploy(jediBat, genVec);
        } else if(sithBat.quantity > jediBat.quantity) {
            sithBat.quantity -= jediBat.quantity;
            numTroopsLost += (jediBat.quantity * 2);
            if(oArgs.verbose) {
                cout << "Buyer " << sithBat.UID << " bought seller " << jediBat.UID;
                cout << "'s commodity " << id << " for price " << jediBat.forceSen << ". " << numTroopsLost << " units were traded.\n";
            }
            if (oArgs.median) {
                addToMedian(numTroopsLost);
            }

            battlesResolved += addSithDeploy(sithBat, genVec);
        } else {
            //they both lost, we do nothing, but call resolve battles again
            numTroopsLost += (sithBat.quantity * 2);
            if(oArgs.verbose) {
                cout << "Buyer " << sithBat.UID << " bought seller " << jediBat.UID;
                cout << "'s commodity " << id << " for price " << jediBat.forceSen << ". " << numTroopsLost << " units were traded.\n";
            }
            if (oArgs.median) {
                addToMedian(numTroopsLost);
            }

            battlesResolved += resolveBattles(genVec);
        }
    }
    return battlesResolved;
}


