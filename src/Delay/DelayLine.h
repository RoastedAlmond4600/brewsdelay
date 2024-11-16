#pragma once
#include <stdlib.h>
#include <vector>
template <typename Type>
class DelayLine {
    public:
        DelayLine() {
        }
        void push(Type value) noexcept{
            rawData[oldestIndex] = value;
            oldestIndex = oldestIndex == 0 ? size() - 1 : oldestIndex - 1;
        }
        Type get(size_t delayInSamples) const noexcept {
            jassert(delayInSamples > 0 && delayInSamples < rawData.size());
            return rawData[(oldestIndex + 1 + delayInSamples) % size()];
        }
        void set(size_t delayInSamples, Type newValue) noexcept{
            jassert(delayInSamples > 0 && delayInSamples < rawData.size());
            rawData[(oldestIndex + 1 + delayInSamples) % rawData.size()] = newValue;
        }
        void resize(size_t delayInSamples) {
            jassert(delayInSamples > 0 && delayInSamples < rawData.size());
            rawData.resize(delayInSamples);
            oldestIndex = 0;
        }
        void clear() {
            std::fill(rawData.begin(), rawData.end(), 0); 
        }
        size_t size() const noexcept {
            return rawData.size();
        }
    private:
        size_t oldestIndex;
        std::vector<Type> rawData;
};

template class DelayLine<float>;
template class DelayLine<double>;
