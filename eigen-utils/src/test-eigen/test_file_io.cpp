#include "../eigen_file_io.hpp"
#include "../eigen_utils.hpp"

using namespace Eigen;
using namespace eigen_utils;
using namespace std;

int main(int argc, char * argv[])
{

  MatrixXd m = MatrixXd::Random(6, 8);
  writeToFile("testEigen.eg", m);
  eigen_dump(m);

  MatrixXd m2;
  readFromFile("testEigen.eg", m2);

  eigen_dump(m2);

  ofstream ofs("testMultiMat.eg", ios::binary);
  MatrixXd w1 = MatrixXd::Random(6, 8);
  MatrixXd w2 = MatrixXd::Random(2, 4);
  MatrixXd w3 = MatrixXd::Random(3, 8);
  writeToFile(ofs, w1);
  writeToFile(ofs, w2);
  writeToFile(ofs, w3);
  ofs.close();

  eigen_dump(w1);
  eigen_dump(w2);
  eigen_dump(w3);

  ifstream ifs("testMultiMat.eg", ios::binary);
  while (ifs.peek() != ifstream::traits_type::eof()) {
    MatrixXd r;
    readFromFile(ifs, r);
    eigen_dump(r);
  }
  ifs.close();
}
