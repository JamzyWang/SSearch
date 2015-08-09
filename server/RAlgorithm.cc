#include "Algorithm.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <cmath>
#include <fstream>

#include <thread>
#include <atomic>
#include <errno.h>

using std::string;
using std::vector;
using std::pair;

namespace {

double fast_exp[17][17][17];
double fast_exp2[17][17];


double calc_distance_scale_aw_q2db(const vector<pair<int, int>> &a, const vector<pair<int, int>> &b) {
  double r1 = 0;
  double cnt1 = 0;

  for (int i = 8; i < 232; ++i)
    for (int j = 8; j < 312; ++j) if (a[i * 320 + j].first != 0) {
      int ik = a[i * 320 + j].first;
      int di = b[ik * 240 * 320 + i * 320 + j].first - i;
      int dj = b[ik * 240 * 320 + i * 320 + j].second - j;

      double best_cost = sqrt(di * di + dj * dj);

      if (best_cost > 15) {
        r1 += 15;
        ++cnt1;
        continue;
      }

      double tmp = 0.0;
      double normal = 0.0;

      for (int p = i - 8; p <= i + 8; ++p) if (p + di >= 0 && p + di < 240) {
        for (int q = j - 8; q <= j + 8; ++q) if (q + dj >= 0 && q + dj < 320) {
          if (a[p * 320 + q].first != 0) {
            int pk = a[p * 320 + q].first;
            int dp = b[pk * 240 * 320 + (p + di) * 320 + q + dj].first - (p + di);
            int dq = b[pk * 240 * 320 + (p + di) * 320 + q + dj].second - (q + dj);

            double weight = fast_exp[p - i + 8][q - j + 8][pk - ik + 8];
            tmp += weight * std::min(std::sqrt(dp * dp + dq * dq), 15.0);
            normal += weight;
          } else {
            if (b[(p + di) * 320 + q + dj].first != 0) {
              double weight = b[(p + di) * 320 + q + dj].second * fast_exp2[p - i + 8][q - j + 8];
              tmp += weight * 15.0;
              normal += weight;
            }
          }
        }
      }

      r1 += tmp / normal;
      ++cnt1;
    }

  return r1 / cnt1;
}



double calc_distance_scale_aw_impl(const vector<pair<int, int>> &a, const vector<pair<int, int>> &b) {
  double r1 = 0;
  double cnt1 = 0;

  for (int i = 8; i < 232; ++i)
    for (int j = 8; j < 312; ++j) if (a[i * 320 + j].first != 0) {
      int ik = a[i * 320 + j].first;

      double best_cost = 30.0;

      for (int di = -8; di <= 8; ++di) if (i + di >= 0 && i + di < 240) {
        for (int dj = -8; dj <= 8; ++dj) if (j + dj >= 0 && j + dj < 320) {
          double tmp = 0.0;
          double normal = 0.0;

          for (int p = i - 8; p <= i + 8; ++p) if (p + di >= 0 && p + di < 240) {
            for (int q = j - 8; q <= j + 8; ++q) if (q + dj >= 0 && q + dj < 320) {
              if (a[p * 320 + q].first != 0) {
                int pk = a[p * 320 + q].first;
                int dp = b[pk * 240 * 320 + (p + di) * 320 + q + dj].first - (p + di);
                int dq = b[pk * 240 * 320 + (p + di) * 320 + q + dj].second - (q + dj);

                double weight = fast_exp[p - i + 8][q - j + 8][pk - ik + 8];
                tmp += weight * std::min(std::sqrt(dp * dp + dq * dq), 30.0);
                normal += weight;
              } else {
                if (b[(p + di) * 320 + q + dj].first != 0) {
                  double weight = 15.0 * fast_exp2[p - i + 8][q - j + 8];
                  tmp += weight * 30.0;
                  normal += weight;
                }
              }
            }
          }

          tmp /= normal;

          if (best_cost > tmp) {
            best_cost = tmp;
          }
        }
      }

      double weight = a[i * 320 + j].second / 30.0;
      r1 += weight * best_cost;
      cnt1 += weight;
    }

  return r1 / cnt1;
}


vector<double> extract_gradient(const string &filename)
{
  auto *file = fopen((filename.substr(0, filename.size() - 3) + "_s").c_str(), "rb");

  if (file == NULL)
    {
      std::cout << " open file error(extract_gradient) " << strerror(errno) <<std::endl;
   }

  vector<double> gradient(240 * 320);
  size_t cnt = fread(&gradient[0], sizeof (double), 240 * 320, file);

  if (cnt != 240 * 320) {
    std::cerr << cnt << " err while reading scale_gradient" << filename << std::endl;
  }

  fclose(file);

  return gradient;
}


vector<unsigned char> extract_scale(const string &filename, const double threshold)
{

  auto *file = fopen((filename.substr(0, filename.size() - 3) + "_s").c_str(), "rb");


      if( file == NULL ) std::cout << " open file error(extract_scale)" << strerror(errno) <<std::endl;

  unsigned char tmp[240 * 320];
  size_t r_cnt = fread(tmp, sizeof (unsigned char), 240 * 320, file);


  fclose(file);

  if (r_cnt != 240 * 320) {
    std::cerr << r_cnt << " err while reading image_scale" << filename << std::endl;
  }


  vector<unsigned char> scale(240 * 320);

  for (int i = 0; i < 240; ++i) {
    for (int j = 0; j < 320; ++j) {
      unsigned char s = tmp[j * 240 + i];
      scale[i * 320 + j] = s;
    }
  }


  if (threshold == 0.0) {
    return scale;
  }

  vector<bool> mask(240 * 320, 0);

  for (int i = 8; i < 232; ++i) {
    for (int j = 8; j < 312; ++j) if (scale[i * 320 + j] > 0) {
      int cnt[31];
      std::fill(cnt, cnt + 31, 0);

      for (int p = i - 8; p <= i + 8; ++p)
        for (int q = j - 8; q <= j + 8; ++q)
          ++cnt[scale[p * 320 + q]];

      for (int idx = 2; idx < 31; ++idx) {
        cnt[idx] += cnt[idx - 1];
      }

      if (cnt[scale[i * 320 + j]] < cnt[30] * threshold) {
        mask[i * 320 + j] = true;
      }
    }
  }

  for (int i = 0; i < 240 * 320; ++i) if (mask[i]) scale[i] = 0;
  return scale;
}

vector<pair<int, int>> extract_sketch_features(const string &filename) {
  vector<pair<int, int>> result(9 * 240 * 320);
  cv::Mat src = cv::imread(filename, 2);

  cv::Mat img;
  cv::resize(src, img, cv::Size(320, 240));

  cv::Mat canny;
  cv::Canny(img, canny, 50, 150);

  const unsigned char *data = canny.data;

  cv::Mat g_x, g_y;
  cv::Scharr(img, g_x, CV_64F, 1, 0);
  cv::Scharr(img, g_y, CV_64F, 0, 1);

  double *data_x = reinterpret_cast<double *>(g_x.data);
  double *data_y = reinterpret_cast<double *>(g_y.data);

  for (int i = 0; i < 240; ++i) {
    for (int j = 0; j < 320; ++j) {
      const int idx = i * 320 + j;
      result[idx].second = 15;

      if (data[idx] == 0) {
        result[idx].first = 0;
      } else if (data_y[idx] < 0) {
        result[idx].first = static_cast<int>((std::atan2(-data_y[idx], -data_x[idx]) / 3.1416 * 180 - 11) / 22.5) + 1;
      } else {
        result[idx].first = static_cast<int>((std::atan2(data_y[idx], data_x[idx]) / 3.1416 * 180 - 11) / 22.5) + 1;
      }
    }
  }

  int distance[240 * 320];
  for (int k = 1; k <= 8; ++k) {
    std::fill(distance, distance + 240 * 320, 12345678);

    for (int i = 0; i < 240; ++i)
      for (int j = 0; j < 320; ++j) {
        const int idx = i * 320 + j;
        if (result[idx].first == k) {
          distance[idx] = 0;
          for (int p = std::max(i - 30, 0); p <= i + 30 && p < 240; ++p)
            for (int q = std::max(j - 30, 0); q <= j + 30 && q < 320; ++q)
              if ((p - i) * (p - i) + (q - j) * (q - j) < distance[p * 320 + q]) {
                distance[p * 320 + q] = (p - i) * (p - i) + (q - j) * (q - j);
                result[k * 240 * 320 + p * 320 + q].first = i; 
                result[k * 240 * 320 + p * 320 + q].second = j; 
              }
        }
      }

    for (int i = 0; i < 240; ++i)
      for (int j = 0; j < 320; ++j) {
        const int idx = i * 320 + j;
        if (distance[idx] == 12345678) {
          result[k * 240 * 320 + i * 320 + j].first = 800; 
          result[k * 240 * 320 + i * 320 + j].second = 800; 
        }
      }

  }

  return result;
}

vector<pair<int, int>> extract_features_fast(const string &filename, const double threshold) {


  vector<pair<int, int>> result(240 * 320);

  vector<unsigned char> scale = ::extract_scale(filename, threshold);

  vector<double> gradient = ::extract_gradient(filename);


  for (int i = 0; i < 240; ++i) {
    for (int j = 0; j < 320; ++j) {
      const int idx = i * 320 + j;

      if (scale[idx] <= 0) {
        result[idx].first = 0;
        result[idx].second = 0;
      } else {
        result[idx].first = static_cast<int>((gradient[idx] - 11) / 22.5) + 1;
      }

      result[idx].second = scale[idx];
    }
  }

  return result;
}


vector<pair<int, int>> extract_features(const string &filename, const double threshold) {
  vector<pair<int, int>> result(9 * 240 * 320);
  vector<unsigned char> scale = ::extract_scale(filename, threshold);
  vector<double> gradient = ::extract_gradient(filename);

  for (int i = 0; i < 240; ++i) {
    for (int j = 0; j < 320; ++j) {
      const int idx = i * 320 + j;

      if (scale[idx] <= 0) {
        result[idx].first = 0;
        result[idx].second = 0;
      } else {
        result[idx].first = static_cast<int>((gradient[idx] - 11) / 22.5) + 1;
      }

      result[idx].second = scale[idx];
    }
  }

  int distance[240 * 320];
  for (int k = 1; k <= 8; ++k) {
    std::fill(distance, distance + 240 * 320, 12345678);

    for (int i = 0; i < 240; ++i)
      for (int j = 0; j < 320; ++j) {
        const int idx = i * 320 + j;
        if (result[idx].first == k) {
          distance[idx] = 0;
          for (int p = std::max(i - 30, 0); p <= i + 30 && p < 240; ++p)
            for (int q = std::max(j - 30, 0); q <= j + 30 && q < 320; ++q)
              if ((p - i) * (p - i) + (q - j) * (q - j) < distance[p * 320 + q]) {
                distance[p * 320 + q] = (p - i) * (p - i) + (q - j) * (q - j);
                result[k * 240 * 320 + p * 320 + q].first = i; 
                result[k * 240 * 320 + p * 320 + q].second = j; 
              }
        }
      }

    for (int i = 0; i < 240; ++i)
      for (int j = 0; j < 320; ++j) {
        const int idx = i * 320 + j;
        if (distance[idx] == 12345678) {
          result[k * 240 * 320 + i * 320 + j].first = 800; 
          result[k * 240 * 320 + i * 320 + j].second = 800; 
        }
      }

  }

  return result;
}


void multi_calc_distance(int beg, int end, const vector<pair<int, int>> &query_vector, vector<pair<double, string>> &rank_list) {
  std::cerr << "thread started for [" << beg << " , " << end << ")" << std::endl;

  for (int i = beg; i < end; ++i) {


    // vector<pair<int, int>> db_vector = ::extract_features(rank_list[i].second, 0.6);
    vector<pair<int, int>> db_vector = ::extract_features_fast(rank_list[i].second, 0.6);
  

    // rank_list[i].first = ::calc_distance_scale_aw_impl(db_vector, query_vector) * ::calc_distance_scale_aw_q2db(query_vector, db_vector);
    rank_list[i].first = ::calc_distance_scale_aw_impl(db_vector, query_vector);


  }
}

} // anonymous namespace


namespace {

double calc_distance(const vector<double> &a, const vector<double> &b) {
  int len = a.size();

  double result = 0;
  for (int i = 0; i < len; i += 4) {
    if (a[i + 1] == 0.0) continue;
    double tmp = 0;
    tmp += (a[i] - b[i]) * (a[i] - b[i]);
    tmp += (a[i + 1] - b[i + 1]) * (a[i + 1] - b[i + 1]);
    tmp += (a[i + 2] - b[i + 2]) * (a[i + 2] - b[i + 2]);
    tmp += (a[i + 3] - b[i + 3]) * (a[i + 3] - b[i + 3]);

    result += std::sqrt(tmp);
  }

  return result;
}


vector<double> extract_features_tensor(const string &filename) {
  cv::Mat src = cv::imread(filename, 2);
  cv::Mat img;
  cv::resize(src, img, cv::Size(1024, 768));
  cv::Mat g_x, g_y;
  cv::Scharr(img, g_x, CV_64F, 1, 0);
  cv::Scharr(img, g_y, CV_64F, 0, 1);

  double *data_x = reinterpret_cast<double *>(g_x.data);
  double *data_y = reinterpret_cast<double *>(g_y.data);

  vector<double> result;

  for (int i = 0; i < 768; ++i) {
    for (int j = 0; j < 1024; ++j) {
      int idx = i * 1024 + j;
      double ta = data_x[idx];
      double tb = data_y[idx];
      if (ta * ta + tb * tb < 0.0707107) {
        data_x[idx] = 0;
        data_y[idx] = 0;
      }
      if (data_y[idx] < 0) {
        data_y[idx] = -data_y[idx];
        data_x[idx] = -data_x[idx];
      }
    }
  }

  for (int i = 0; i < 768; i += 24) {
    for (int j = 0; j < 1024; j += 32) {
      double a11(0), a12(0), a21(0), a22(0);
      for (int ii = 0; ii < 24; ++ii) {
        for (int jj = 0; jj < 32; ++jj) {
          int idx = (i + ii) * 1024 + j + jj;
          a11 += data_x[idx] * data_x[idx];
          a22 += data_y[idx] * data_y[idx];
          a12 += data_x[idx] * data_y[idx];
        }
      }
      a21 = a12;
      double normal = sqrt(a11 * a11 + a12 * a12 + a21 * a21 + a22 * a22);
      if (normal < 0.0000001) normal = 0.0000001;

      result.push_back(a11 / normal);
      result.push_back(a12 / normal);
      result.push_back(a21 / normal);
      result.push_back(a22 / normal);
    }
  }

  return result;
}

} // anonymous namespace

RAlgorithm::RAlgorithm() {
  for (int i = -8; i <= 8; ++i) {
    for (int j = -8; j <= 8; ++j) {
      for (int k = -8; k <= 8; ++k) {
        ::fast_exp[i + 8][j + 8][k + 8] = std::exp(-(i * i + j * j) / 64.0 - k * k / 2.0);
        std::cout << fast_exp[i + 8][j + 8][k + 8] << std::endl;
      }

      ::fast_exp2[i + 8][j + 8] = std::exp(-(i * i + j * j) / 64.0) / 30.0;
    }
  }
}


vector<pair<double, string>> RAlgorithm::query(const string &filename) const {
  vector<pair<double, string>> rank_list(filenames.size());

  vector<double> query_vector = ::extract_features_tensor(filename);
  for (size_t i = 0; i < filenames.size(); ++i) {
    rank_list[i].first = ::calc_distance(query_vector, db[i]);
    rank_list[i].second = filenames[i];
  }

  std::sort(rank_list.begin(), rank_list.end());

  {
    vector<pair<int, int>> query_vector = ::extract_sketch_features(filename);
    // vector<pair<int, int>> query_vector = ::extract_features(filename, 0.0);

    std::thread t[4];
    const int len = std::min((size_t)40, filenames.size());
    int step = (len + 3) / 4;
    for (int i = 0; i < 4; ++i) {
      t[i] = std::thread(::multi_calc_distance, std::min(i * step, len), std::min((i + 1) * step, len), query_vector, std::ref(rank_list));
    }

    for (int i = 0; i < 4; ++i) {
      t[i].join();
    }

    std::sort(rank_list.begin(), rank_list.begin() + len);
  }


  return rank_list;
}

void RAlgorithm::update_database(const std::string &fn_imagelist) {
  std::ifstream images_list(fn_imagelist);

  std::string image_name;
  int cnt = 0;

  std::set<std::string> h(filenames.begin(), filenames.end());

  while (images_list >> image_name) {
    std::cerr << ++cnt << " " << image_name << std::endl;
    if (h.find(image_name) == h.end()) {
      db.push_back(extract_features_tensor(image_name));
      filenames.push_back(image_name);
      h.insert(image_name);
    }
  }
}
