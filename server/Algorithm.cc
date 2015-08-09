#include "Algorithm.h"

#include <opencv2/opencv.hpp>

#include <dlfcn.h>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

using std::string;
using std::vector;
using std::pair;

// Algorithm

void Algorithm::reload_func(const string &func_name) {
  if (handle) dlclose(handle);

  handle = dlopen("./func.so", RTLD_NOW | RTLD_GLOBAL);

  if (!handle) {  
    std::cout << dlerror() << std::endl;
    return;
  }

  calc_distance = reinterpret_cast<void *>(dlsym(handle, func_name.c_str()));

  char *err = dlerror();
  if (err != NULL) {
    std::cout << err << std::endl;
    return;
  }

  std::cout << "reload func " << func_name << " succeed." << std::endl;
}

Algorithm::~Algorithm() {
  if (handle)
    dlclose(handle);
}
