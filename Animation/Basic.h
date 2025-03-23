#pragma once

#include <complex>
#include <memory>

namespace anim{

class AbstractAnimation;

using Point=std::complex<double>;
using Vector=Point;
using pAnim=std::shared_ptr<AbstractAnimation>;
using Anim=AbstractAnimation;

}

