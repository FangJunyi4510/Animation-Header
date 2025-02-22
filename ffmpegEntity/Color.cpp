#include "Color.h"

std::ostream& std::operator<<(std::ostream& out,const anim::Color& c){
	return out<<"RGBA64("<<c.red<<","<<c.green<<","<<c.blue<<","<<c.alpha<<")";
}
