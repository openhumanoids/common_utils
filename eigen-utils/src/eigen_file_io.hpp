#ifndef EIGEN_FILE_IO_HPP_
#define EIGEN_FILE_IO_HPP_

#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include "eigen_lcm.hpp"
namespace eigen_utils {

template<typename ScalarType>
void writeToFile(std::ofstream &ofs, const Eigen::Matrix<ScalarType, Eigen::Dynamic, Eigen::Dynamic> & m)
{
  using namespace std;
  int32_t rows = m.rows();
  int32_t cols = m.cols();
  string type = typenameToStr<ScalarType>();
  int32_t typelen = type.length();
//  cerr << "rows: " << rows << " cols: " << cols << " typelen: " << typelen << " type: " << type << "\n";

  ofs.write((char *) &rows, sizeof(int32_t));
  ofs.write((char *) &cols, sizeof(int32_t));
  ofs.write((char *) &typelen, sizeof(int32_t));
  ofs.write(type.c_str(), typelen);
  ofs.write((char *) m.data(), m.size() * sizeof(ScalarType));
}
template<typename ScalarType>
void writeToFile(const std::string & name, const Eigen::Matrix<ScalarType, Eigen::Dynamic, Eigen::Dynamic> & m)
{
  using namespace std;
  ofstream ofs(name.c_str(), ios::binary);
  if (!ofs.good()) {
    cerr << "ERROR: Could not open file " << name << endl;
  }
  writeToFile(ofs, m);
  ofs.close();
}

template<typename ScalarType>
void readFromFile(std::ifstream &ifs, Eigen::Matrix<ScalarType, Eigen::Dynamic, Eigen::Dynamic> &m)
{
  using namespace std;
  int32_t rows = 0, cols = 0;
  int32_t typelen = 0;
  char type[256] = { 0 };
  ifs.read((char *) &rows, sizeof(int32_t));
  ifs.read((char *) &cols, sizeof(int32_t));
  ifs.read((char *) &typelen, sizeof(int32_t));
  ifs.read((char *) type, typelen);
//  cerr << "rows: " << rows << " cols: " << cols << " typelen: " << typelen << " type: " << type << "\n";
  if (type != typenameToStr<ScalarType>()) {
    cerr << "ERROR: file has type: " << type << " which doesn't match template type of " << typenameToStr<ScalarType>()
        << "\n";
    return;
  }
  m.resize(rows,cols);
  ifs.read((char *) m.data(), rows * cols * sizeof(ScalarType));
}
template<typename ScalarType>
void readFromFile(const std::string & name, Eigen::Matrix<ScalarType, Eigen::Dynamic, Eigen::Dynamic> &m)
{
  using namespace std;
  ifstream ifs(name.c_str(), ios::binary);
  if (!ifs.good()) {
    cerr << "ERROR: Could not open file " << name << endl;
  }
  readFromFile(ifs,m);
  ifs.close();
}

}
#endif /* EIGEN_FILE_IO_HPP_ */
