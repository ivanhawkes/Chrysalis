#pragma once

class XmlNodeRef;


// TODO: Dig into the code to find out what this does.

enum eGeometrySlot
{
	eIGS_FirstPerson = 0,
	eIGS_ThirdPerson,
	eIGS_Owner,
	eIGS_OwnerAnimGraph,
	eIGS_OwnerAnimGraphLooped,
	eIGS_Aux0,
	eIGS_Destroyed,
	eIGS_Aux1,
	eIGS_ThirdPersonAux,
	eIGS_Last,
	eIGS_LastAnimSlot = eIGS_OwnerAnimGraphLooped + 1,
	eIGS_LastLayerSlot = eIGS_ThirdPerson + 1
};


class CItemGeometryParameter
{
public:
	CItemGeometryParameter();
	~CItemGeometryParameter();

	
	/** Full pathname of the model file. */
	string modelPath;

	/** The material. */
	string material;
	
	/** The position. */
	Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
	
	/** The angles. */
	Ang3 angles = Ang3 (0.0f, 0.0f, 0.0f);
	
	/** The scale. */
	float scale = 1.0f;
	
	/** The slot. */
	int slot = eGeometrySlot::eIGS_Last;
	
	/** true to use CGF streaming. */
	bool useCgfStreaming = false;
	
	/** true to use parent material. */
	bool useParentMaterial = false;


	/** Resets this object to it's default state. */
	void Reset();

	/**
	Reads the given node.

	\param	node	The node to read.

	\return	true if it succeeds, false if it fails.
	*/
	bool Read(const XmlNodeRef& node);
};

