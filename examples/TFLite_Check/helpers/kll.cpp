#include <iostream>
#include <vector>
#include <algorithm>

class KLLSketch {
public:
    KLLSketch(int maxBins) : maxBins(maxBins) {
        bins.reserve(maxBins);
    }

    void update(double value) {
        bins.push_back(value);
        std::sort(bins.begin(), bins.end());
        if (bins.size() > maxBins) {
            // Merge bins if we have more than maxBins
            mergeBins();
        }
    }

    double getQuantile(double quantile) {
        if (bins.empty()) {
            return 0.0; // or handle this case as you need
        }
        int rank = static_cast<int>(quantile * bins.size());
        return bins[rank];
    }

private:
    void mergeBins() {
        std::vector<double> newBins;
        int binSize = bins.size() / maxBins;
        for (int i = 0; i < bins.size(); i += binSize) {
            double binSum = 0;
            for (int j = i; j < i + binSize; ++j) {
                binSum += bins[j];
            }
            newBins.push_back(binSum / binSize);
        }
        bins = newBins;
    }

    std::vector<double> bins;
    int maxBins;
};

int main() {
    KLLSketch sketch(128); // Set the maximum number of bins

    for (int i = 0; i < 1000; i++) {
        double data_point = static_cast<double>(i);
        sketch.update(data_point);
    }

    double quantile = 0.5; // 50th percentile
    double estimate = sketch.getQuantile(quantile);
    std::cout << "Estimated " << quantile * 100 << "th percentile: " << estimate << std::endl;

    return 0;
}

