#include <iostream>
#include <random>
#include <array>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>

constexpr uint16_t max_size = 50;
constexpr uint16_t filter_size = 0xFFFF;

uint16_t polyHash(const uint16_t coef, const std::string& input);

class BloomFilter {
private:
    uint16_t numHash;
    std::vector<uint8_t> filterArray;

public:
    BloomFilter(uint16_t m);
    void add(const std::string& input);
    bool checkCont(const std::string& input);
    void clear();
    const std::vector<uint8_t>& getFilterArray() const;
};

uint16_t polyHash(const uint16_t coef, const std::string& inputString) {
    if (inputString.size() > max_size) {
        std::cerr << "You exceed the string limit\n";
        return 0;
    }

    std::array<uint16_t, max_size / 2> arr{};

    for (size_t i = 0, j = 0; i < inputString.size(); i += 2, ++j) {
        arr[j] = static_cast<uint16_t>(inputString[i]) + (i + 1 < inputString.size() ? (static_cast<uint16_t>(inputString[i + 1]) << 8) : 0);
    }

    uint16_t res = 1;
    for (const auto& elem : arr)
        res = res * coef + elem;

    return res;
}

BloomFilter::BloomFilter(uint16_t m) : numHash(m), filterArray((filter_size + 7) / 8, 0) {}

void BloomFilter::add(const std::string& input) {
    for (uint16_t i = 1; i <= numHash; i++) {
        uint16_t hash = polyHash(i, input);
        uint16_t byteIndex = hash / 8;
        uint8_t bitIndex = hash % 8;

        filterArray[byteIndex] |= (1 << bitIndex);
    }
}

bool BloomFilter::checkCont(const std::string& input) {
    for (uint16_t i = 1; i <= numHash; i++) {
        uint16_t hash = polyHash(i, input);
        uint16_t byteIndex = hash / 8;
        uint8_t bitIndex = hash % 8;

        if (!(filterArray[byteIndex] & (1 << bitIndex))) {
            return false;
        }
    }
    return true;
}

void BloomFilter::clear() {
    std::fill(filterArray.begin(), filterArray.end(), 0);
}

const std::vector<uint8_t>& BloomFilter::getFilterArray() const {
    return filterArray;
}

std::string generateString() {
    std::random_device rd;
    std::mt19937 gen(rd());

    const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const int max_length = 50;

    std::uniform_int_distribution<> dis(0, characters.size() - 1);

    std::stringstream ss;

    for (int i = 0; i < max_length; ++i) {
        ss << characters[dis(gen)];
    }

    return ss.str();
}

void experiment(uint16_t m, double alpha, int numExperiments) {
    int N = static_cast<int>(filter_size);
    int n = static_cast<int>(alpha * N);
    int s = static_cast<int>(std::ceil(1 / alpha * std::log(2)));
    std::cout << "s = " << s << std::endl;

    double totalErrorProbability = 0.0;

    for (int exp = 0; exp < numExperiments; ++exp) {
        BloomFilter bloomFilter(s);

        std::unordered_set<std::string> messages;
        for (int i = 0; i < n; ++i) {
            std::string message = generateString();
            bloomFilter.add(message);
            messages.insert(message);
        }

        int M = 0;
        int falsePositives = 0;

        while (M < n) {
            std::string message = generateString();
            ++M;

            if (bloomFilter.checkCont(message)) {
                // Check if the message was one of the ones we added
                if (messages.find(message) == messages.end()) {
                    // This is a false positive
                    ++falsePositives;
                }
            }
            /*if (M % 1000 == 0) {
                std::cout << "Iteration: " << M << ", False Positives: " << falsePositives << std::endl;
            }*/
        }

        double errorProbability = static_cast<double>(falsePositives) / M;
        totalErrorProbability += errorProbability;
    }

    double averageErrorProbability = totalErrorProbability / numExperiments;

    std::cout << "Average error probability after " << numExperiments << " experiments: " << averageErrorProbability << "\n";
}


int main() {

    experiment(3, 0.05, 100);
    experiment(3, 0.1, 100);
    experiment(3, 0.15, 100);
    experiment(3, 0.20, 100);
    experiment(3, 0.25, 100);
    experiment(3, 0.3, 100);
    experiment(3, 0.35, 100);
    experiment(3, 0.4, 100);
    experiment(3, 0.45, 100);
    experiment(3, 0.5, 100);
}


