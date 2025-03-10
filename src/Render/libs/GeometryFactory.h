//***************************************************************************************
// GeometryGenerator.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Defines a static class for procedurally generating the geometry of 
// common mathematical objects.
//
// All triangles are generated "outward" facing.  If you want "inward" 
// facing triangles (for example, if you want to place the camera inside
// a sphere to simulate a sky), you will need to:
//   1. Change the Direct3D cull mode or manually reverse the winding order.
//   2. Invert the normal.
//   3. Update the texture coordinates and tangent vectors.
//***************************************************************************************

#pragma once

#include "d3dUtils.h"

class GeometryFactory
{
public:
	static GeometryFactory& instance()
	{
		static GeometryFactory geometry;
		return geometry;
	}

	GeometryFactory();
	~GeometryFactory();
	void Initialize(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	
	static Mesh* GetCube();
	static Mesh* GetGeosphere();
	static Mesh* GetGrid();
	static Mesh* GetSphere();

	static Mesh* LoadGeometryFromFile(std::wstring path, bool invertV = true);

private:
	ID3D12Device* mpDevice;
	ID3D12GraphicsCommandList* mpCommandList;

	Mesh* Cube;
	Mesh* Geosphere;
	Mesh* Grid;
	Mesh* Sphere;
	
	void Subdivide(MeshData& meshData);
	Vertex MidPoint(const Vertex& v0, const Vertex& v1);
	void GenerateGeometryBuffer(Mesh* geo);

	///<summary>
	/// Creates a box centered at the origin with the given dimensions, where each
	/// face has m rows and n columns of vertices.
	///</summary>
	Mesh* CreateBox(float width, float height, float depth, uint32 numSubdivisions);

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	Mesh* CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	///</summary>
	Mesh* CreateGeosphere(float radius, uint32 numSubdivisions);
	
	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	///</summary>
	Mesh* CreateGrid(float width, float depth, uint32 m, uint32 n);

	///<summary>
	/// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
	///</summary>
	Mesh* CreateQuad(float x, float y, float w, float h, float depth);
};

