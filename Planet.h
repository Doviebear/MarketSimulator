// PROJECT IDENTIFIER: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#include <vector>
#include <algorithm>
#include <iostream>
#include <algorithm> // std::sort
#include <getopt.h>
#include <queue> 
#include <string>  

using namespace std;


struct OutputArgs {
    bool verbose = false;
    bool median = false;
    bool watcher = false;

    OutputArgs(bool v_in, bool m_in, bool w_in) : 
    verbose(v_in), median(m_in), watcher(w_in) { };

    OutputArgs() {};

};

struct General {
    uint32_t jediDeployed = 0;
    uint32_t sithDeployed = 0;
    uint32_t numDied = 0;
};

struct Deployment {
    uint32_t ts; 
    uint32_t UID;
    double forceSen;
    double quantity;


    Deployment(uint32_t ts_in, uint32_t UID_in, double fs_in, double q_in) : 
    ts(ts_in), UID(UID_in), forceSen(fs_in), quantity(q_in) { };
};



class DeployJediComp {
public: 
    bool operator() (const Deployment& lhs, const Deployment& rhs) {
        if (lhs.forceSen > rhs.forceSen) {
            return true;
        } else if (lhs.forceSen < rhs.forceSen) {
            return false;
        } else {
            if (lhs.UID > rhs.UID){
                return true;
            } else {
                return false;
            }
        }
    }
};

class DeploySithComp {
public:
    bool operator() (const Deployment& lhs, const Deployment& rhs) {
        if (lhs.forceSen > rhs.forceSen) {
            return false;
        } else if (lhs.forceSen < rhs.forceSen) {
            return true;
        } else {
            if (lhs.UID > rhs.UID){
                return true;
            } else {
                return false;
            }
        }
    }
};



class Planet {
public:
    Planet() { };

    Planet(uint32_t ID_in) : id(ID_in) { };


    //return number of battles resolved
    uint32_t addJediDeploy(Deployment toAdd, vector<General>& genVec){
        jediDeploys.push(toAdd);
        return resolveBattles(genVec);
    }

    //return number of battles resolved
    uint32_t addSithDeploy(Deployment toAdd, vector<General>& genVec) {
        sithDeploys.push(toAdd);
        return resolveBattles(genVec);
    }

    uint32_t resolveBattles(vector<General>& genVec);

    void updateOutputArgs(OutputArgs oArgsToU) {
        oArgs.verbose = oArgsToU.verbose;
        oArgs.median = oArgsToU.median;
        oArgs.watcher = oArgsToU.watcher;
    }

    double getMedian() { 
        if(minMedianQ.size() == 0 && maxMedianQ.size() == 0) {
            return 0;
        }

        
        if(minMedianQ.size() == maxMedianQ.size() ) {
            double toReturn = (minMedianQ.top() + maxMedianQ.top()) / 2;
            return toReturn;
        } else if (minMedianQ.size() > maxMedianQ.size() ){
            double toReturn = minMedianQ.top();
            return toReturn;
        } else {
            double toReturn = maxMedianQ.top();
            return toReturn;
        }


    }

    void addToMedian(double numToAdd) {
        double currMedian = getMedian();
        if(maxMedianQ.size() > minMedianQ.size()){
            if(numToAdd < currMedian) {
                double swapEle = maxMedianQ.top();
                maxMedianQ.pop();
                minMedianQ.push(swapEle);
                maxMedianQ.push(numToAdd);
            } else {
                minMedianQ.push(numToAdd);
            }
        } else if (minMedianQ.size() > maxMedianQ.size()) {
            if(numToAdd > currMedian) {
                double swapEle = minMedianQ.top();
                minMedianQ.pop();
                maxMedianQ.push(swapEle);
                minMedianQ.push(numToAdd);
            } else {
                maxMedianQ.push(numToAdd);
            }
        } else {
            if(numToAdd < currMedian) {
                maxMedianQ.push(numToAdd);
            } else {
                minMedianQ.push(numToAdd);
            }
        }
    }



    


private:
    priority_queue<Deployment, vector<Deployment>, DeployJediComp> jediDeploys;
    priority_queue<Deployment, vector<Deployment>, DeploySithComp> sithDeploys;
    uint32_t id = 0;
    priority_queue<double> maxMedianQ;
    priority_queue<double, vector<double>, greater<double> > minMedianQ;


    OutputArgs oArgs;

    bool checkForBattle() const;
};