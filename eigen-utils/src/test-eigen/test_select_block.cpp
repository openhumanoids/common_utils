#include <eigen_utils/eigen_utils.hpp>

using namespace Eigen;
using namespace eigen_utils;
using namespace std;

int main(int argc, char * argv[])
{

  MatrixXd test = ArrayXXd::Random(5, 5);
  eigen_dump(test);
  ArrayXi indices = ArrayXi::LinSpaced(3, 0, 3);
  eigen_dump(indices.transpose());
  eigen_dump(selectColsByIndices(test, indices));
  eigen_dump(selectRowsByIndices(test, indices));
  eigen_dump(selectBlockByIndices(test,indices, indices));

  cerr<<"\n\n";
  Array<bool,Dynamic,1> indicator = ArrayXi::LinSpaced(test.rows(), 0, test.rows()-1) <3;
  eigen_dump(indicator.transpose());
  eigen_dump(selectColsByIndicator(test, indicator));
  eigen_dump(selectRowsByIndicator(test, indicator));
  eigen_dump(selectBlockByIndicators(test,indicator, indicator));

}
