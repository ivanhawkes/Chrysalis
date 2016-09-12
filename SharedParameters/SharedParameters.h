/**
 \file	d:\Chrysalis\Source\Chrysalis\SharedParameters\SharedParameters.h

 Declares macros to enable better use of shared parameters as complete structs / classes rather than
 embedding them in another class.
 */

#pragma once

#include <SharedParams/ISharedParams.h>


#define SHARED_PARAMS_BODY(name)											\
	virtual ISharedParams *Clone() const									\
	{																		\
		return new name(*this);												\
	}																		\
																			\
	virtual const CSharedParamsTypeInfo &GetTypeInfo() const				\
	{																		\
		return s_typeInfo;													\
	}																		\
																			\
	static const CSharedParamsTypeInfo s_typeInfo;


namespace SharedParameters
{
	/**
	 Template for shared parameters.
	
	 \tparam	T	Generic type parameter.
	 \param	rootParams	Generally this will be the root of the XML document that contains the struct
	 					values. In some cases it might be preferable to pass in a sub-document.
	 \param	className 	Used for creating a unique hash index for the shared parameter. Pass in the
	 					general class of the struct e.g. item, weapon.
	 \param	entityName	Used for creating a unique hash index for the shared parameter. Pass in the
	 					name of the entity which will own this parameter. This allows each 'class'
	 					and 'entity' to have it's own unique shared parameter group.
	 \param	nodeName  	Name of the XML node which will we will read the parameters from. This is
	 					expected to be a sub-node of the root node passed in.
	
	 \return	The shared parameters.
	 */

	template< typename T >
	std::shared_ptr<const T> GetSharedParameters(XmlNodeRef rootParams, string className, string entityName, string nodeName)
	{
		ISharedParamsManager* pSharedParamsManager = gEnv->pGame->GetIGameFramework()->GetISharedParamsManager();
		CRY_ASSERT(pSharedParamsManager);

		// If no parameter set exists we should attempt to create and register one.
		CryFixedStringT<256> sharedParameterName;
		sharedParameterName.Format("%s::%s::%s", className.c_str(), entityName.c_str(), nodeName.c_str());
		std::shared_ptr<const T> sharedParameter = CastSharedParamsPtr<T>(pSharedParamsManager->Get(sharedParameterName));
		if (!sharedParameter)
		{
			// Load in the shared parameters then register a new set of parameters and retrieve a shared pointer to them.
			T newSharedParameter;
			XmlNodeRef node = rootParams->findChild(nodeName);
			if (node)
				newSharedParameter.Read(node);
			sharedParameter = CastSharedParamsPtr<T>(pSharedParamsManager->Register(sharedParameterName, newSharedParameter));
		}

		CRY_ASSERT(sharedParameter.get());
		return sharedParameter;
	}
};