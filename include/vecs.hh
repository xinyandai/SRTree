//
// Created by xydai on 2021/9/6.
//

#ifndef HNSRTREE_INCLUDE_VECS_HH_
#define HNSRTREE_INCLUDE_VECS_HH_
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <cassert>
#include <random>
#include <stop_w.hh>


using std::vector;


template <typename DataType>
void load_data(DataType*& data, int& dimension, int &cardinality, std::string input_path)
{
  std::ifstream fin(input_path.c_str(), std::ios::binary | std::ios::ate);
  if (!fin) {
    std::cout << "cannot open file " << input_path << std::endl;
    exit(1);
  }

  size_t fileSize = fin.tellg();
  fin.seekg(0, fin.beg);
  if (fileSize == 0) {
    std::cout << "file size is 0 " << input_path << std::endl;
    exit(1);
  }

  int dim;
  fin.read(reinterpret_cast<char*>(&dim), sizeof(int));
  dimension = (size_t)dim;
  size_t bytesPerRecord = dimension * sizeof(DataType) + 4;
  if (fileSize % bytesPerRecord != 0) {
    std::cout << "File not aligned" << std::endl;
    exit(1);
  }
  cardinality = fileSize / bytesPerRecord;
  data = new DataType[cardinality * dimension];
  fin.read((char*)data, sizeof(DataType) * dimension);

  for (int i = 1; i < cardinality; ++i) {
    fin.read((char*)&dim, 4);
    assert(dim == dimension);
    fin.read((char*)(data + i * dimension), sizeof(DataType) * dimension);
  }
  fin.close();
}

template<typename T>
std::vector<std::vector<int > >
interval_search(const T* xb_, const T* xq_, const T* rg_,
                const int nx, const int nq, const int d) {
  std::vector<std::vector<int > > res(nq);

#ifdef verbose
  ProgressBar progress_bar(nq, "interval search");
#endif
#ifdef MULTI_THREAD
#pragma omp parallel for
#endif
  for (int qi = 0; qi < nq; ++qi) {
    for (int xi = 0; xi < nx; ++xi) {
      bool should_in = true;
      for (int di = 0; di < d; ++di) {

        T diff = xb_[xi * d + di] - xq_[qi * d + di];
        T range = rg_[qi * d + di];

        if (diff > range || diff < -range) {
          should_in = false;
          break;
        }
      }
      if (should_in) {
        res[qi].push_back(xi);
      }
    }
#ifdef verbose
    #pragma omp critical
    {
      ++progress_bar;
    }
#endif
  }
  return res;
}


vector<vector<int > > read_list(const char* file_name) {
  string line;
  std::ifstream file_stream(file_name);
  vector<vector<int > > res;

  while(getline(file_stream, line)) {
    std::istringstream iss(line);
    res.emplace_back();
    int id;
    while (iss >> id) {
      res.back().push_back(id);
    }
  }
  return res;
}

bool exists_test(const char* name) {
  std::ifstream f(name);
  return f.good();
}

void write_list(const char* file_name, vector<vector<int > > lists) {
  std::ofstream file_stream(file_name);
  if (file_stream.is_open()) {
    for (vector<int> ids : lists) {
      if (!ids.empty()) {
        file_stream << ids[0];
      }
      for (int i = 1; i < ids.size(); ++i) {
        file_stream << " " << ids[i];
      }
      file_stream << "\n";
    }
    file_stream.close();
  }
  else {
    std::cout << "Unable to open file : " << file_name << std::endl;
  }
}


template <bool Compute=false>
vector<vector<int > >
load_gt(const char* fg, const float* xb,
        const float* xq, const float* rg,
        int nb, int nq, int dimension) {
  if constexpr (Compute) {
    return interval_search<float >(xb, xq, rg, nb, nq, dimension);
  }
  if (exists_test(fg)) {
    return read_list(fg);
  } else {
    StopW stop_w;
    auto ground_truth = interval_search<float >(xb, xq, rg, nb, nq, dimension);
    std::cout << "average brute force search time : "
              << stop_w.getElapsedTimeMicro(true) / nq << std::endl;
    write_list(fg, ground_truth);
    return ground_truth;
  }
}
#endif //HNSRTREE_INCLUDE_VECS_HH_
