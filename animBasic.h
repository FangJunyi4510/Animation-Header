#pragma once

#include <complex>
#include <memory>

namespace anim{

class AbstractImage;

using Point=std::complex<double>;
using Vector=Point;
using pImage=std::shared_ptr<AbstractImage>;

}

