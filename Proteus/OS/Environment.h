//
//  Environment.hpp
//  Einstein
//
//  Created by Matthias Melcher on 4/5/19.
//

#ifndef PROTEUS_OS_ENVIRONMENT_H
#define PROTEUS_OS_ENVIRONMENT_H

#include "Proteus.h"

namespace NewtOS {


	class TEnvironment
	{
	public:
		T_GETSET_MEMBER_W(0x10, KUInt32, 10_DomainAccess);
	};


} // namespace


#endif /* PROTEUS_OS_ENVIRONMENT_H */
