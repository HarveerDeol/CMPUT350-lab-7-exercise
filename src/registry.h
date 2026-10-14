#ifndef REGISTRY_H
#define REGISTRY_H

#include <cassert>
#include <cstddef> 
#include <utility>  
#include <vector>

template <typename T>
class Registry {
   public:
    template <typename U>
    size_t store(U&& value) {
        size_t id;
        if (!freeSlots_.empty()) {
            id = freeSlots_.back();
            freeSlots_.pop_back();
            data_[id].value = std::forward<U>(value);
            alive_[id] = true;
        } else {
            id = data_.size();
            data_.emplace_back();  
            data_.back().value = std::forward<U>(value);
            alive_.push_back(true);
        }
        ++count_;
        return id;
    }

    void remove(size_t id) {
        assert(valid(id));
        data_[id].value = T{};
        alive_[id] = false;
        freeSlots_.push_back(id);
        --count_;
    }

    T& operator[](size_t id) {
        assert(valid(id));
        return data_[id].value;
    }

    const T& operator[](size_t id) const {
        assert(valid(id));
        return data_[id].value;
    }

    bool valid(size_t id) const { return id < alive_.size() && alive_[id]; }

    size_t size() const { return count_; }

    void reserve(size_t capacity) {
        data_.reserve(capacity);
        alive_.reserve(capacity);
    }

   private:
    struct Slot {
        T value;
    };
    std::vector<Slot> data_;         
    std::vector<bool> alive_;          
    std::vector<size_t> freeSlots_;    
    size_t count_ = 0;                 

#endif  // REGISTRY_H