#ifndef _ALGORITHM_H
#define _ALGORITHM_H

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

template <typename T>
class Database {
public:
  void load_database(const std::string &filename);
  void save_database(const std::string &filename);

  std::vector<T> db;
  std::vector<std::string> filenames;
};

template <typename T>
void Database<T>::load_database(const std::string &filename) {
  std::ifstream ifs(filename, std::ios::binary);
  boost::archive::binary_iarchive ia(ifs);
  ia >> db >> filenames;
}

template <typename T>
void Database<T>::save_database(const std::string &filename) {
  std::ofstream ofs(filename, std::ios::binary);
  boost::archive::binary_oarchive oa(ofs);
  oa << db << filenames;
}

class BarDatabase {
public:
  void load_database(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    boost::archive::binary_iarchive ia(ifs);
    ia >> db >> filenames >> db_g;
  }

  void save_database(const std::string &filename) {
    std::ofstream ofs(filename, std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << db << filenames << db_g;
  }

protected:
  std::vector<std::vector<unsigned char>> db;
  std::vector<std::vector<double>> db_g;
  std::vector<std::string> filenames;
};


class Algorithm {
public:
  void reload_func(const std::string &func_name);
  virtual std::vector<std::pair<double, std::string>> query(const std::string &filename) const = 0;
  virtual void update_database(const std::string &fn_imagelist) = 0;
  virtual ~Algorithm();

protected:
  void *calc_distance;

private:
  void *handle = 0;
};

class TensorAlgorithm: public Algorithm, public Database<std::vector<double>> {
public:
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

class SakiAlgorithm: public Algorithm, public Database<std::vector<double>> {
public:
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

class FooAlgorithm: public Algorithm, public Database<std::vector<int>> {
public:
  typedef int value_type;
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

class BarAlgorithm: public Algorithm, public BarDatabase {
public:
  typedef unsigned char value_type;
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

class CostAlgorithm: public Algorithm, public Database<bool> {
public:
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

class AWAlgorithm: public Algorithm, public Database<unsigned char> {
public:
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

class AWTensorAlgorithm: public Algorithm, public Database<unsigned char> {
public:
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

class GFAlgorithm: public Algorithm, public Database<std::pair<std::vector<double>, std::vector<unsigned char>>> {
public:
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

class ScaleTensorAlgorithm: public Algorithm, public Database<std::vector<double>> {
public:
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

class ScaleCaoAlgorithm: public Algorithm, public Database<std::vector<std::pair<int, int>>> {
public:
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

class ScaleAWAlgorithm: public Algorithm, public Database<std::vector<std::pair<int, int>>> {
public:
  ScaleAWAlgorithm();
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

class RAlgorithm: public Algorithm, public Database<std::vector<double>> {
public:
  RAlgorithm();
  std::vector<std::pair<double, std::string>> query(const std::string &filename) const;
  void update_database(const std::string &fn_imagelist);
};

typedef RAlgorithm DefaultAlgorithm;

#endif
