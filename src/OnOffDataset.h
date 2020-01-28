#ifndef ONOFFDATASET_H_
#define ONOFFDATASET_H_

#include <omnetpp.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>

using namespace omnetpp;

namespace eventsimulator {

//template<class T>
class OnOffDataset {

public:
    /**
     * Helper class to calculate a sliding window value
     *
     * windowSize    Window size in seconds
     */
    OnOffDataset() {
    }



    /**
     * Default destructor
     */
    ~OnOffDataset() {
    }


    /**
     * Add a dataset
     *
     * time     The time
     * value    The value
     */
    void addDataset(simtime_t time, bool value) {
        if (value == lastState) return;

        if (lastState == false && value == true){
            values.push_back(time - lastStateChange);
        }
        lastState = value;
        lastStateChange = time;
    }

    /**
     * Add a dataset using the current simTime()
     *
     * value    The value of type T to be added
     */
    void addDataset(bool value) {
        addDataset(simTime(), value);
    }

    /**
     * Get the number of stored values
     */
    size_t getLen() {
        return values.size();
    }

    /**
     * Return the quantil
     *
     * @param quantil Quantil in percent, 0.0...1.0
     */
    simtime_t getQuantil(double quantil){
        if (getLen() == 0) return 0.0;
        sortDataset();
        int pos = int(getLen()*quantil);
        return values[pos];
    }

    /**
     * Get the median
     */
    simtime_t getMedian(){
        return getQuantil(0.5);
    }

    std::vector<simtime_t> getVector(){
        return values;
    }

    void sortDataset(){
        if (getLen() != 0){
            sort(values.begin(), values.end());
        }
    }

    bool storeVectorToFile(std::string path){
        if (getLen() == 0) return false;
        sortDataset();
        std::ofstream output_file(path);
        std::ostream_iterator<simtime_t> output_iterator(output_file, "\n");
        std::copy(values.begin(), values.end(), output_iterator);
        return true;
    }

private:
    std::vector<simtime_t> values;

    bool lastState = false;
    simtime_t lastStateChange = 0.0;


};
/* class */

} /* namespace eventsimulator */

#endif /* ONOFFDATASET_H */
