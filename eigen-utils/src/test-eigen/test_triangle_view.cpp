#include <eigen_utils/eigen_utils.hpp>

using namespace Eigen;
using namespace eigen_utils;
using namespace std;

int main(int argc, char * argv[])
{

  Matrix<double, 9, 1> nined;
//  nined.setLinSpaced(1, 9);

  Map<const Matrix3d> mated(nined.data());
  eigen_dump(mated);

}
