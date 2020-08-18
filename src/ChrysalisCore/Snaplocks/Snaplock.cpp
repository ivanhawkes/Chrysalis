#include <StdAfx.h>

#include "Snaplock.h"


namespace Chrysalis
{
void ISnaplock::Serialize(Serialization::IArchive & ar)
{
	ar(m_snaplockType, "snaplockType", "SnaplockType");
	ar(m_isMale, "isMale", "Is this a male snaplock?");
	ar(m_isInUse, "isInUse", "Is this snaplock in use?");
	ar(m_children, "children", "Children Snaplocks");
}


std::vector<ISnaplock> ISnaplock::GetChildren()
{
	return m_children;
}


void ISnaplock::AddSnaplock(ISnaplock snaplock)
{
	m_children.push_back(snaplock);
}
}