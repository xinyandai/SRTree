//
// Created by xydai on 2021/9/6.
//

#ifndef HNSRTREE_INCLUDE_STOP_W_HH_
#define HNSRTREE_INCLUDE_STOP_W_HH_
#include <sstream>
#include <string>
#include <chrono>
#include <cassert>
#include <random>

#define Debug
#define verbose
//#define MULTI_THREAD

#define DELTA 0
//#define DELTA 0.001

template <typename T >
inline void check_range(T v, T a, T b) {
#ifdef Debug
  if (v < a || a >= b) {
    if (v < a) {
      std::cerr << " v_" << v << " is smaller than " << a;
    } else {
      std::cerr << " v_" << v << " is bigger than " << b;
    }
    throw std::runtime_error("exceed range");
  }
#endif
}

using std::string;
using std::vector;
using std::chrono::steady_clock;
using std::chrono::microseconds;
using std::chrono::duration_cast;

typedef float ValueType;
typedef int int_t;

class StopW {
  std::chrono::steady_clock::time_point time_begin;
 public:
  StopW() {
    reset();
  }

  long getElapsedTimeMicro(bool reset_time) {
    steady_clock::time_point time_end = steady_clock::now();
    long elapsed = duration_cast<microseconds>(time_end - time_begin).count();
    if (reset_time) {
      reset();
    }
    return elapsed;
  }

  void reset() {
    time_begin = steady_clock::now();
  }
};

class ProgressBar {
 public:
  ProgressBar(int len, string message): len_(len), cur_(0), star_(0) {
    std::cout << "0%   10   20   30   40   50   60   70   80   90   100%\t"
              << message
              << std::endl
              << "|----|----|----|----|----|----|----|----|----|----|"
              << std::endl;
  }

  ProgressBar& update(int i) {
    cur_ += i;
    int num_star = static_cast<int >(1.0 * cur_ / len_ * 50 + 1);
    if (num_star > star_) {
      for (int j = 0; j < num_star-star_; ++j) {
        std::cout << '*';
      }
      star_ = num_star;
      if (num_star == 51) {
        std::cout << std::endl;
      }
      std::cout << std::flush;
    }

    return *this;
  }

  ProgressBar& operator++() {
    return update(1);
  }

  ProgressBar& operator+=(int i) {
    return update(i);
  }

 private:
  int len_;
  int cur_;
  int star_;
};
#endif //HNSRTREE_INCLUDE_STOP_W_HH_
