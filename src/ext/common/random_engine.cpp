#include "random_engine.h"

namespace random_engine {
    
template<> const utility::power<yarn2::modulus,yarn2::gen> yarn2::g = utility::power<yarn2::modulus,yarn2::gen>();
template<> const utility::power<yarn3::modulus,yarn3::gen> yarn3::g = utility::power<yarn3::modulus,yarn3::gen>();
template<> const utility::power<yarn3s::modulus,yarn3s::gen> yarn3s::g = utility::power<yarn3s::modulus,yarn3s::gen>();
template<> const utility::power<yarn4::modulus,yarn4::gen> yarn4::g = utility::power<yarn4::modulus,yarn4::gen>();
template<> const utility::power<yarn5::modulus,yarn5::gen> yarn5::g = utility::power<yarn5::modulus,yarn5::gen>();
template<> const utility::power<yarn5s::modulus,yarn5s::gen> yarn5s::g = utility::power<yarn5s::modulus,yarn5s::gen>();

};