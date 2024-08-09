#include "Location.hpp"

const int &Location::hasAlias()const{
    return(this->_alias_check);
}

const int &Location::hasRoot()const{
    return(this->_root_check);
}

const bool& Location::hasPathInfo(void) const{
    return(this->_path_info);
}

const bool& Location::isGenerated() const{
    return(this->_is_generated);
}

const bool& Location::isExactMatch(void) const{
    return(this->_exact_match);
}
