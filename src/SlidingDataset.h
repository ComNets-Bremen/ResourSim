#ifndef SLIDINGDATASET_H_
#define SLIDINGDATASET_H_

#include <omnetpp.h>
#include <deque>
#include <utility>

using namespace omnetpp;

namespace eventsimulator {

template<class T>
class SlidingDataset {

public:
    /**
     * Helper class to calculate a sliding window value
     *
     * windowSize    Window size in seconds
     */
    SlidingDataset(double windowSize) {
        this->windowSize = windowSize;
    }

    /**
     * Constructor with default window size (1/2 hour)
     */
    SlidingDataset(){
        this->windowSize = 60*30; // 1/2 hour
    }

    /**
     * Default destructor
     */
    ~SlidingDataset() {
    }

    /**
     * Set the window size
     *
     * windowSize   The window size in seconds
     */
    void setWindowSize(double windowSize){
        this->windowSize = windowSize;
    }

    /**
     * Return the window size
     */
    double getWindowSize(){
        return this->windowSize;
    }

    /**
     * Add a dataset
     *
     * time     The time
     * value    The value
     */
    void addDataset(simtime_t time, T value) {
        values.push_front(std::pair<simtime_t, T>(time, value));
    }

    /**
     * Add a dataset using the current simTime()
     *
     * value    The value of type T to be added
     */
    void addDataset(T value) {
        addDataset(simTime(), value);
    }

    /**
     * Get the number of stored values
     */
    size_t getLen() {
        return values.size();
    }

    simtime_t storedDuration() {
        return values.front().first - values.back().first;
    }

    /**
     * Remove values older than the windows size
     */
    void cleanup() {
        while (values.size() > 3
                && values[values.size() - 2].first
                        < (values.front().first - windowSize)) {
            values.pop_back();
        }
        values.shrink_to_fit();
    }

    /**
     * Get the percentage of the current value. The template type has to
     * be comparable via = operator.
     *
     * Use the current simTime()
     *
     * value    The value to check for
     */
    double getPercentageOfValue(T value) {
        return getPercentageOfValue(value, simTime());
    }

    /**
         * Get the percentage of the current value. The template type has to
         * be comparable via = operator.
         *
         * value    The value to check for
         * time     The timestamp to use
         */
    double getPercentageOfValue(T value, simtime_t time){
        return getPercentageOfValue(value, time, this->windowSize);
    }


    /**
     * Get the percentage of the current value. The template type has to
     * be comparable via = operator.
     *
     * value    The value to check for
     * time     The timestamp to use
     * windowSize   The windowSize
     */
    double getPercentageOfValue(T value, simtime_t time, double windowSize) {
        cleanup();
        if (values.empty()){
            return 0.0;
        }
        std::map<T, double> resultMap;
        simtime_t lowerBound = time - windowSize;
        if (time < windowSize)
            lowerBound = values.back().first; // Timestamp of 1st value
        simtime_t upperBound = time;
        std::pair<simtime_t, T> lastPair;

        for (auto v = values.rbegin(); v != values.rend(); v++) {

            if (resultMap.count(v->second) == 0) {
                resultMap[v->second] = 0.0;
            }
            if (v->first > upperBound)
                continue;
            if (v->first <= lowerBound) {
                lastPair = *v;
                continue;
            }

            simtime_t startTime = lastPair.first;
            if (startTime < lowerBound)
                startTime = lowerBound;

            resultMap[lastPair.second] += (v->first - startTime).dbl()
                    / (upperBound - lowerBound);
            lastPair = *v;

        }

        resultMap[lastPair.second] += (upperBound - lastPair.first).dbl()
                / (upperBound - lowerBound);

        if (resultMap.count(value) == 0) return 0.0;
        return resultMap[value];
    }

    double getPercentageOfValue24Hrs(T value){
        return getPercentageOfValue(value, simTime(), 60*60*24);
    }

    /**
     * Print all values
     */
    void printDeque() {
        for (auto v : values) {
            EV_INFO << "t: " << v.first << " v: " << v.second << std::endl;
        }
    }
private:
    double windowSize;
    std::deque<std::pair<simtime_t, T>> values;

};
/* class */

} /* namespace eventsimulator */

#endif /* SLIDINGDATASET_H_ */
