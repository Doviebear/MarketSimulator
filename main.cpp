// PROJECT IDENTIFIER: AD48FB4835AF347EB0CA8009E24C3B13F8519882
/* 
Struture of the things
    Planet Class
        Sith prio Queue
        Jedi Prio Queue
        
    Deployment Struct
        Timestamp
        General ID
        Force sensitivity
        Quantity



*/


#include <vector>
#include <iostream>
#include <getopt.h>
#include <string>    // needed for VS
#include <queue> 
#include <sstream>
#include <locale>
#include "Planet.h"

using namespace std;

//int argc, char** argv


enum class State {Initial, SeenFirst, SeenBoth, MaybeBetter };

struct MovieWatcher {
    int32_t tsFirst = -1;
    int32_t tsSecond = -1;
    double fsFirst = 0;
    double fsSecond = 0;
    int32_t maybeTs = 0;
    double maybeFs = 0;
    State state = State::Initial;
};

struct MovieWatcherWrapper{
    MovieWatcher attackWatcher;
    MovieWatcher ambushWatcher;
};




class Driver {
public:

    Driver(int argc, char** argv){

        getOptions(argc,argv);

        string input;

        getline(cin, input);


        //Remove top line
        
        numPlanets = 1;
        planetVec.resize(numPlanets);
        if(oArgs.watcher) {
            watcherVec.resize(numPlanets);
        }
        for(uint32_t i = 0; i < planetVec.size(); i++) {
            planetVec[i] = Planet(i);
            planetVec[i].updateOutputArgs(oArgs);
        }
        

        
       
    }

    void printMedian(){
        if(oArgs.median) {
            for(size_t i = 0; i < planetVec.size(); i++) {
                double medianNum = planetVec[i].getMedian();
                if (medianNum != 0) {
                    cout << "Median trade amount for commodity " << i << " at time ";
                    cout << currTime << " is " << medianNum << ".\n";
                }
            }
        }
    }

    void updateWatchers(Deployment deploy, char side, uint32_t planet) {
        updateAttackWatcher(deploy, side, planet);
        updateAmbushWatcher(deploy, side, planet);
    }

    void updateAttackWatcher(Deployment deploy, char side, uint32_t planet) {
        MovieWatcher& w = watcherVec[planet].attackWatcher;
        if(w.state == State::Initial) {
            if (side == 'S') {
                w.tsFirst = (int32_t) deploy.ts;
                w.fsFirst = deploy.forceSen;
                w.state = State::SeenFirst;
            }
        } else if(w.state == State::SeenFirst) {
            if (side == 'B' && deploy.forceSen >= w.fsFirst){
                w.tsSecond = (int32_t) deploy.ts;
                w.fsSecond = deploy.forceSen;
                w.state = State::SeenBoth;
            } else if (side == 'S' && w.fsFirst > deploy.forceSen){
                w.tsFirst = (int32_t) deploy.ts;
                w.fsFirst = deploy.forceSen;
            }
        } else if(w.state == State::SeenBoth) {
            if(side == 'S' && w.fsFirst > deploy.forceSen ) {
                w.maybeFs = deploy.forceSen;
                w.maybeTs = (int32_t) deploy.ts;
                w.state = State::MaybeBetter;
            } else if (side == 'B' &&  deploy.forceSen > w.fsSecond) {
                w.tsSecond = (int32_t) deploy.ts;
                w.fsSecond = deploy.forceSen; 
            }  
        } else {
            int32_t maybeFsDiff = (int32_t) deploy.forceSen - (int32_t) w.maybeFs;
            int32_t currFsDiff = (int32_t) w.fsSecond - (int32_t) w.fsFirst;
            if (side == 'B' && maybeFsDiff > currFsDiff) {
                w.tsFirst = w.maybeTs;
                w.tsSecond = (int32_t) deploy.ts;
                w.fsFirst = w.maybeFs;
                w.fsSecond = deploy.forceSen;
                w.state = State::SeenBoth;
            } else if (side == 'S' && w.maybeFs > deploy.forceSen) {
                w.maybeFs = deploy.forceSen;
                w.maybeTs = (int32_t) deploy.ts;
            }
        }
    }

    void updateAmbushWatcher(Deployment deploy, char side, uint32_t planet) {
        MovieWatcher& w = watcherVec[planet].ambushWatcher;
        if(w.state == State::Initial) {
            if (side == 'B') {
                w.tsFirst = (int32_t) deploy.ts;
                w.fsFirst = deploy.forceSen;
                w.state = State::SeenFirst;
            }
        } else if(w.state == State::SeenFirst) {
            if (side == 'S' && w.fsFirst >= deploy.forceSen){
                w.tsSecond = (int32_t) deploy.ts;
                w.fsSecond = deploy.forceSen;
                w.state = State::SeenBoth;
            } else if (side == 'B' && deploy.forceSen > w.fsFirst){
                w.tsFirst = (int32_t) deploy.ts;
                w.fsFirst = deploy.forceSen;
            }

        } else if(w.state == State::SeenBoth) {
            if(side == 'B' && deploy.forceSen > w.fsFirst) {
                w.maybeFs = deploy.forceSen;
                w.maybeTs = (int32_t) deploy.ts;
                w.state = State::MaybeBetter;
            } else if (side == 'S' &&  w.fsSecond > deploy.forceSen ) {
                w.tsSecond = (int32_t) deploy.ts;
                w.fsSecond = deploy.forceSen; 
            }  
        } else {
            int32_t maybeFsDiff = (int32_t) w.maybeFs - (int32_t) deploy.forceSen;
            int32_t currFsDiff = (int32_t) w.fsFirst - (int32_t) w.fsSecond;
            if (side == 'S' && maybeFsDiff > currFsDiff) {
                w.tsFirst = w.maybeTs;
                w.tsSecond = (int32_t) deploy.ts;
                w.fsFirst = w.maybeFs;
                w.fsSecond = deploy.forceSen;
                w.state = State::SeenBoth;
            } else if (side == 'B' && deploy.forceSen > w.maybeFs) {
                w.maybeFs = deploy.forceSen;
                w.maybeTs = (int32_t) deploy.ts;
            }
        }
    }

    void printWatcher() {
        cout << "---Best Differential---\n";
        for (size_t i = 0; i < watcherVec.size(); ++i) {
            MovieWatcherWrapper w = watcherVec[i];
            double fsDiffAttack = w.attackWatcher.fsSecond - w.attackWatcher.fsFirst;
            double fsDiffAmbush = w.ambushWatcher.fsFirst - w.ambushWatcher.fsSecond;
            if(w.attackWatcher.state == State::SeenFirst){
                w.attackWatcher.tsFirst = -1;
                fsDiffAttack = 0;
            }
            if(w.ambushWatcher.state == State::SeenFirst) {
                w.ambushWatcher.tsFirst = -1;
                fsDiffAmbush = 0;
            }

            cout << "The best theoretical match of with buyer first and seller second with commodity " << i;
            cout << " is with buyer at time " << w.ambushWatcher.tsFirst << " and seller at time ";
            cout << w.ambushWatcher.tsSecond << " with a price differential of " << fsDiffAmbush;
            cout << ".\n";

            
            cout << "The best theoretical match of with seller first and buyer second with commodity " << i;
            cout << " is with seller at time " << w.attackWatcher.tsFirst << " and buyer at time ";
            cout << w.attackWatcher.tsSecond << " with a price differential of " << fsDiffAttack;
            cout << ".\n";
        }
    }

    void readDeployments(){
        uint32_t UID;
        uint32_t ts;

        string UIDS;
        string tsS;

        string side;
        string date;

        double price; //forceSen
        double amount;

        string priceS; //forceSen
        string amountS;

        string localVal;
        string pair;
        string exchange;


        double battlesResolved = 0;


        cout << "Reading Orders...\n";


        while(getline(cin,UIDS,',')) {
            getline(cin,tsS,',');
            getline(cin,date,',');
            getline(cin,side,',');
            getline(cin,priceS,',');
            getline(cin,amountS,',');
            getline(cin,localVal,',');
            getline(cin,pair,',');
            getline(cin,exchange);

            



            UID = (uint32_t) stoul(UIDS);
            ts = (uint32_t) stoul(tsS);
            price = (double) stold(priceS);
            amount = (double) stold(amountS);
            
            //Error checking
            if(price <= 0){
                cerr << "Invalid price\n";
                exit(1);
            }
            if(amount <= 0){
                cerr << "Invalid amount\n";
                exit(1);
            }



            if(ts > currTime) {
                printMedian();
                currTime = ts;
            }


            Deployment deployToAdd(ts, UID, price, amount);
           

            if(side == "ask") {
                /*if(oArgs.general) {
                    generalVec[generalID].jediDeployed += numTroops;
                }*/
                if(oArgs.watcher) {
                    updateWatchers(deployToAdd,'S',0);
                }
                battlesResolved += planetVec[0].addJediDeploy(deployToAdd, generalVec);
            } else {
                /*if(oArgs.general) {
                    generalVec[generalID].sithDeployed += numTroops;
                }*/
                if(oArgs.watcher) {
                    updateWatchers(deployToAdd,'B',0);
                }
                battlesResolved += planetVec[0].addSithDeploy(deployToAdd, generalVec);
            }
            

        }
        printMedian();
        cout << "---End of Day---\n";
        cout << "Trades Resolved: " << battlesResolved << '\n';

        /*
        if(oArgs.general) {
            cout << "---General Evaluation---\n";
            for(size_t i = 0; i < generalVec.size(); ++i) {
                uint32_t numTotal = generalVec[i].jediDeployed + generalVec[i].sithDeployed;
                cout << "General " << i << " deployed " << generalVec[i].jediDeployed;
                cout << " Jedi troops and " << generalVec[i].sithDeployed << " Sith troops, and ";
                cout << (numTotal - generalVec[i].numDied) << "/" << numTotal << " troops survived.\n";
            }
        }
        */
        if(oArgs.watcher) {
            printWatcher();
        }
    }

    void getOptions(int argc, char** argv) {
        int option_index = 0, option = 0;
        
    
        opterr = false;
        
        // use getopt to find command line options
        struct option longOpts[] = {{ "verbose", no_argument, nullptr, 'v' },
                                    { "median", no_argument, nullptr, 'm' },
                                    { "watcher", no_argument, nullptr, 'w' },
                                    { nullptr, 0, nullptr, '\0' }};
        

        while ((option = getopt_long(argc, argv, "vmw", longOpts, &option_index)) != -1) {
            switch (option) {
                case 'v':
                    oArgs.verbose = true;
                    break;
                case 'm': 
                    oArgs.median = true;
                    break;
                case 'w':
                    oArgs.watcher = true;
                    break;
                default:
                    exit(1);
            }
        }
        
    }


private:
    uint32_t currTime = 0;
    vector<Planet> planetVec;
    vector<General> generalVec;
    uint32_t numPlanets = 0;
    vector<MovieWatcherWrapper> watcherVec;
    OutputArgs oArgs;

};



int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false);

    Driver driver(argc, argv);
    driver.readDeployments(); 

    return 0;

} 




