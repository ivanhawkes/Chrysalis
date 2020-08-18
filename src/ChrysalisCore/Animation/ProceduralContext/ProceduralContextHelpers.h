#pragma once

namespace Chrysalis
{
namespace ProceduralContextHelpers
{
template< typename TRequestType >
class CRequestList
{
	typedef std::vector< TRequestType > TRequestsVector;

public:
	uint32 AddRequest(TRequestType& request)
	{
		request.id = m_nextId;
		m_requests.push_back(request);

		// It is extremely unlikely that we'll wrap around and get a clash with a current request, since requests will usually have a short life span.
		++m_nextId;

		return request.id;
	}

	void RemoveRequest(const uint32 cancelRequestId)
	{
		typename TRequestsVector::iterator itEnd = m_requests.end();
		for (typename TRequestsVector::iterator it = m_requests.begin(); it != itEnd; ++it)
		{
			const TRequestType& request = *it;
			if (cancelRequestId == request.id)
			{
				m_requests.erase(it);
				return;
			}
		}
	}

	const TRequestType& GetRequest(const size_t index) const
	{
		return m_requests [index];
	}

	const size_t GetCount() const
	{
		return m_requests.size();
	}

private:
	uint32 m_nextId {0};
	TRequestsVector m_requests;
};
}
}