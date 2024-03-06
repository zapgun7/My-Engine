// This divides up a model into AABB's in the form of an oct tree
// This is recursive
// Also has some useful functions for checking if a triangle is within a box!


#include "cAABB.h"

//#include "sTriangle.h"
#include <iostream>

#include "../cVAOManager/sModelDrawInfo.h"
#include "../cVAOManager/cVAOManager.h"

//#include <vector>

bool BoxBoxIntersectTest(glm::vec3 B1minXYZ, glm::vec3 B1maxXYZ, glm::vec3 B2minXYZ, glm::vec3 B2maxXYZ);
bool BoxBoxIntersectTest2(glm::vec3 B1ctr, glm::vec3 B1ext, glm::vec3 B2ctr, glm::vec3 B2ext);

bool cAABB::StartMakeTree(std::string modelName, cVAOManager* borrowedManager, unsigned int maxTri)
{
	// Start by getting the model draw info
	sModelDrawInfo meshDrawInfo;// = borrowedManager->FindDrawInfoByModelName(modelName);
	if (!borrowedManager->FindDrawInfoByModelName(modelName, meshDrawInfo))
	//if (meshDrawInfo == nullptr)
	{
		// Didn't find it!!
		return false;
	}
	glm::vec3 maxXYZ = meshDrawInfo.maxExtents_XYZ;
	glm::vec3 minXYZ = meshDrawInfo.minExtents_XYZ;

	this->centerPosition = (maxXYZ + minXYZ) * 0.5f;
	this->halfLengths = maxXYZ - this->centerPosition;
	
	int ID = 0;
	std::vector<sTriangle_A> modelTriangles;
	for (unsigned int i = 0; i < meshDrawInfo.numberOfIndices; i += 3)
	{
		sTriangle_A currTri;
		currTri.vertices[0].x = meshDrawInfo.pVertices[meshDrawInfo.pIndices[i]].x;
		currTri.vertices[0].y = meshDrawInfo.pVertices[meshDrawInfo.pIndices[i]].y;
		currTri.vertices[0].z = meshDrawInfo.pVertices[meshDrawInfo.pIndices[i]].z;

		currTri.vertices[1].x = meshDrawInfo.pVertices[meshDrawInfo.pIndices[i + 1]].x;
		currTri.vertices[1].y = meshDrawInfo.pVertices[meshDrawInfo.pIndices[i + 1]].y;
		currTri.vertices[1].z = meshDrawInfo.pVertices[meshDrawInfo.pIndices[i + 1]].z;

		currTri.vertices[2].x = meshDrawInfo.pVertices[meshDrawInfo.pIndices[i + 2]].x;
		currTri.vertices[2].y = meshDrawInfo.pVertices[meshDrawInfo.pIndices[i + 2]].y;
		currTri.vertices[2].z = meshDrawInfo.pVertices[meshDrawInfo.pIndices[i + 2]].z;

		currTri.ID = ID++;

		modelTriangles.push_back(currTri);
	}

	this->MakeTree(&modelTriangles, maxTri, true);
	
	return true;
}

void cAABB::MakeTree(std::vector<sTriangle_A>* parentTris, unsigned int maxTri, bool isRoot)
{
	// Start by getting a vector of all triangles that lie within this box
	std::vector<sTriangle_A> boxedTris;
	if (!isRoot)
		boxedTris = trisInBox(parentTris, this->XYZMin(), this->XYZMax());
	else
		boxedTris = *parentTris;

	if (boxedTris.size() <= maxTri)
	{
		// This AABB doesn't have TOO many triangles, so we store them and return
		triangles = boxedTris;
		return;
	}

	// Still too many triangles, we need to create 8 new children
	// Map keys will be 0-7
	// This works clock-wise starting in -x -z, starting at -y then +y
	// #: xyz
	// 0: ---
	// 1: -+-
	// 2: --+
	// 3: -++
	// 4: +-+
	// 5: +++
	// 6: +--
	// 7: ++-
	glm::vec3 newHalflengths = this->halfLengths * 0.5f; // Only need to calculate this once, as all new children will share this

	// 0 //
	cAABB* AABB0 = new cAABB();
	AABB0->centerPosition.x = this->centerPosition.x - (this->halfLengths.x / 2); // -x
	AABB0->centerPosition.y = this->centerPosition.y - (this->halfLengths.y / 2); // -y
	AABB0->centerPosition.z = this->centerPosition.z - (this->halfLengths.z / 2); // -z
	AABB0->halfLengths = newHalflengths;
	AABB0->MakeTree(&boxedTris, maxTri, false);
	mapChild_pAABBs[0] = AABB0;

	// 1 //
	cAABB* AABB1 = new cAABB();
	AABB1->centerPosition.x = this->centerPosition.x - (this->halfLengths.x / 2); // -x
	AABB1->centerPosition.y = this->centerPosition.y + (this->halfLengths.y / 2); // +y
	AABB1->centerPosition.z = this->centerPosition.z - (this->halfLengths.z / 2); // -z
	AABB1->halfLengths = newHalflengths;
	AABB1->MakeTree(&boxedTris, maxTri, false);
	mapChild_pAABBs[1] = AABB1;

	// 2 //
	cAABB* AABB2 = new cAABB();
	AABB2->centerPosition.x = this->centerPosition.x - (this->halfLengths.x / 2); // -x
	AABB2->centerPosition.y = this->centerPosition.y - (this->halfLengths.y / 2); // -y
	AABB2->centerPosition.z = this->centerPosition.z + (this->halfLengths.z / 2); // +z
	AABB2->halfLengths = newHalflengths;
	AABB2->MakeTree(&boxedTris, maxTri, false);
	mapChild_pAABBs[2] = AABB2;

	// 3 //
	cAABB* AABB3 = new cAABB();
	AABB3->centerPosition.x = this->centerPosition.x - (this->halfLengths.x / 2); // -x
	AABB3->centerPosition.y = this->centerPosition.y + (this->halfLengths.y / 2); // +y
	AABB3->centerPosition.z = this->centerPosition.z + (this->halfLengths.z / 2); // +z
	AABB3->halfLengths = newHalflengths;
	AABB3->MakeTree(&boxedTris, maxTri, false);
	mapChild_pAABBs[3] = AABB3;

	// 4 //
	cAABB* AABB4 = new cAABB();
	AABB4->centerPosition.x = this->centerPosition.x + (this->halfLengths.x / 2); // +x
	AABB4->centerPosition.y = this->centerPosition.y - (this->halfLengths.y / 2); // -y
	AABB4->centerPosition.z = this->centerPosition.z + (this->halfLengths.z / 2); // +z
	AABB4->halfLengths = newHalflengths;
	AABB4->MakeTree(&boxedTris, maxTri, false);
	mapChild_pAABBs[4] = AABB4;

	// 5 //
	cAABB* AABB5 = new cAABB();
	AABB5->centerPosition.x = this->centerPosition.x + (this->halfLengths.x / 2); // +x
	AABB5->centerPosition.y = this->centerPosition.y + (this->halfLengths.y / 2); // +y
	AABB5->centerPosition.z = this->centerPosition.z + (this->halfLengths.z / 2); // +z
	AABB5->halfLengths = newHalflengths;
	AABB5->MakeTree(&boxedTris, maxTri, false);
	mapChild_pAABBs[5] = AABB5;

	// 6 //
	cAABB* AABB6 = new cAABB();
	AABB6->centerPosition.x = this->centerPosition.x + (this->halfLengths.x / 2); // +x
	AABB6->centerPosition.y = this->centerPosition.y - (this->halfLengths.y / 2); // -y
	AABB6->centerPosition.z = this->centerPosition.z - (this->halfLengths.z / 2); // -z
	AABB6->halfLengths = newHalflengths;
	AABB6->MakeTree(&boxedTris, maxTri, false);
	mapChild_pAABBs[6] = AABB6;

	// 7 //
	cAABB* AABB7 = new cAABB();
	AABB7->centerPosition.x = this->centerPosition.x + (this->halfLengths.x / 2); // +x
	AABB7->centerPosition.y = this->centerPosition.y + (this->halfLengths.y / 2); // +y
	AABB7->centerPosition.z = this->centerPosition.z - (this->halfLengths.z / 2); // -z
	AABB7->halfLengths = newHalflengths;
	AABB7->MakeTree(&boxedTris, maxTri, false);
	mapChild_pAABBs[7] = AABB7;

	return;
}

// Returns vector of triangles that intersect with the box (from the provided triangles)
std::vector<sTriangle_A> cAABB::trisInBox(std::vector<sTriangle_A>* triangles, glm::vec3 minCorner, glm::vec3 maxCorner)
{
	// Make object we'll return at the end
	std::vector<sTriangle_A> returnSet;

	// Iterate through all triangles
	for (std::vector<sTriangle_A>::iterator itTri = triangles->begin();
		itTri != triangles->end(); itTri++)
	{
		bool isInBox = false;

		//////////////// VERTEX IN BOX CHECK //////////////////////
		for (unsigned int i = 0; i < 3; i++)
		{
			if (itTri->vertices[i].x < minCorner.x) continue;
			if (itTri->vertices[i].y < minCorner.y) continue;
			if (itTri->vertices[i].z < minCorner.z) continue;
			if (itTri->vertices[i].x > maxCorner.x) continue;
			if (itTri->vertices[i].y > maxCorner.y) continue;
			if (itTri->vertices[i].z > maxCorner.z) continue;

			isInBox = true;
			break;
		}
		if (isInBox)
		{
			// 1 Vector is in the box, add triangle and go to next one
			returnSet.push_back(*itTri);
			continue;
		}

		// No vertices in box, check if plane made by triangle intersects box

		/////////////////////////// TRIANGLE PLANE INTERSECTS AABB CHECK ////////////////////////

		// Info Setup
		//glm::vec3 face1 = itTri->vertices[0] - itTri->vertices[1];
		//glm::vec3 face2 = itTri->vertices[2] - itTri->vertices[0];
		//glm::vec3 pN = glm::cross(face1, face2);
		//pN = glm::normalize(pN);

		glm::vec3 pN = glm::normalize(glm::cross(itTri->vertices[0] - itTri->vertices[1],  itTri->vertices[2] - itTri->vertices[0])); // tri plane norm
		glm::vec3 boxExtents = maxCorner - minCorner;
		boxExtents /= 2; // Calculate positive extents
		glm::vec3 boxCtr = minCorner + boxExtents; // Calculate box center

		// Compute projection interval radius of box onto plane normal
		float r = boxExtents.x * abs(pN.x) + boxExtents.y * abs(pN.y) + boxExtents.z * abs(pN.z);

		float d = glm::dot(pN, itTri->vertices[0]);
		// Compute distance of box center from plane
		float s = glm::dot(pN, boxCtr) - d;

// 		bool temp = (s <= r);
// 		bool temp2 = (!s <= r);
// 
// 		if (temp)
// 		{
// 			continue;
// 		}


		if (!(abs(s) <= r))
		{
			// Does not intersect, we can move on to the next triangle
			continue;
		}
		// Intersects, so we have to calculate if the triangle itself intersects

		//////////////////// TRIANGLE FORMED AABB INTERSECTS AABB CHECK //////////////////////   ** first SAT check

		// Start by forming an AABB around the triangle and comparing to the AABB in question
		glm::vec3 triMinXYZ = itTri->vertices[0];
		glm::vec3 triMaxXYZ = itTri->vertices[0];

		for (unsigned int i = 1; i < 3; i++)
		{
			if (itTri->vertices[i].x < triMinXYZ.x) triMinXYZ.x = itTri->vertices[i].x;
			else if (itTri->vertices[i].x > triMaxXYZ.x) triMaxXYZ.x = itTri->vertices[i].x;

			if (itTri->vertices[i].y < triMinXYZ.y) triMinXYZ.y = itTri->vertices[i].y;
			else if (itTri->vertices[i].y > triMaxXYZ.y) triMaxXYZ.y = itTri->vertices[i].y;

			if (itTri->vertices[i].z < triMinXYZ.z) triMinXYZ.z = itTri->vertices[i].z;
			else if (itTri->vertices[i].z > triMaxXYZ.z) triMaxXYZ.z = itTri->vertices[i].z;
		}

		if (!BoxBoxIntersectTest(triMinXYZ, triMaxXYZ, minCorner, maxCorner))
		{
			// Not intersecting continue to next triangle
			continue;
		}
		

		///////////////////// TRIANGLE INTERSECTS AABB CHECK (Separating Axis Test) ////////////////////
		// This is taken from Ericson, Real Time Collision Detection

		float p0, p1, p2;//, r; // r is already defined

		// Set triangle points
		glm::vec3 v0 = itTri->vertices[0];
		glm::vec3 v1 = itTri->vertices[1];
		glm::vec3 v2 = itTri->vertices[2];

		// Already have AABB center and extents above
		// boxCtr
		// boxExtents
		float e0 = boxExtents.x;
		float e1 = boxExtents.y;
		float e2 = boxExtents.z;

		// Translate triangle as conceptually moving AABB to origin

		v0 -= boxCtr;
		v1 -= boxCtr;
		v2 -= boxCtr;

		// Compute edge vectors for triangle
		glm::vec3 f0 = v1 - v0;
		glm::vec3 f1 = v2 - v1;
		glm::vec3 f2 = v0 - v2;



		// Now there are a total of 13 axes to test
		// Start with 9 axes (category 3)

		//a00
		p0 = v0.z * v1.y - v0.y * v1.z;
		p2 = v2.z * (v1.y - v0.y) - v2.y * (v1.z - v0.z);
		r = e1 * abs(f0.z) + e2 * abs(f0.y);
		if (glm::max(-glm::max(p0, p2), glm::min(p0, p2)) > r)
		{
			// Separating axis found: not intersecting
			continue;
		}

		//a01
		p0 = v0.y * (v1.z - v2.z) + v0.z * (v2.y - v1.y);//-v0.y * v2.z + v0.y * v1.z + v0.z * v2.y - v0.z * v1.y;
		p1 = v1.z * v2.y - v1.y * v2.z;
		r = e1 * abs(f1.z) + e2 * abs(f1.y);
		if (glm::max(-glm::max(p0, p1), glm::min(p0, p1)) > r)
		{
			// Separating axis found: not intersecting
			continue;
		}

		//a02
		//p0 = v0.y * (v2.z - v0.z) + v0.z * (v0.y - v2.y);
		p1 = v1.y * (v2.z - v0.z) + v1.z * (v0.y - v2.y);
		p2 = v2.z * v0.y - v2.y * v0.z;//v2.y * (v2.z - v0.z) + v2.z * (v0.y - v2.y);
		r = e1 * abs(f2.z) + e2 * abs(f2.y);
		if (glm::max(-glm::max(p1, p2), glm::min(p1, p2)) > r)
		{
			// Separating axis found: not intersecting
			continue;
		}

		//a10
		p0 = v0.x * v1.z - v0.z *v1.x;//v0.x * (v1.z - v0.z) + v0.z * (v0.x - v1.x);
		//p1 = -v1.x *  v0.z + v1.z * v0.x;//v1.x * (v1.z - v0.z) + v1.z * (v0.x - v1.x);
		p2 = v2.x * (v1.z - v0.z) + v2.z * (v0.x - v1.x);
		r = e0 * abs(f0.z) + e2 * abs(f0.x);
		if (glm::max(-glm::max(p0, p2), glm::min(p0, p2)) > r)
		{
			// Separating axis found: not intersecting
			continue;
		}

		//a11
		p0 = v0.x * (v2.z - v1.z) + v0.z * (v1.x - v2.x);
		p1 = v1.x * v2.z - v1.z * v2.x;//v1.x * (v2.z - v1.z) + v1.z * (v1.x - v2.x);
		//p2 = v2.x * (v2.z - v1.z) + v2.z * (v1.x - v2.x);
		r = e0 * abs(f1.z) + e2 * abs(f1.x);
		if (glm::max(-glm::max(p0, p1), glm::min(p0, p1)) > r)
		{
			// Separating axis found: not intersecting
			continue;
		}

		//a12
		//p0 = v0.x * (v0.z - v2.z) + v0.z * (v2.x - v0.x);
		p1 = v1.x * (v0.z - v2.z) + v1.z * (v2.x - v0.x);
		p2 = v2.x * v0.z - v2.z * v0.x;//v2.x * (v0.z - v2.z) + v2.z * (v2.x - v0.x);
		r = e0 * abs(f2.z) + e2 * abs(f2.x);
		if (glm::max(-glm::max(p1, p2), glm::min(p1, p2)) > r)
		{
			// Separating axis found: not intersecting
			continue;
		}

		//a20
		p0 = v0.y * v1.x - v0.x * v1.y;//v0.x * (v0.y - v1.y) + v0.y * (v1.x - v0.x);
		//p1 = v1.x * (v0.y - v1.y) + v1.y * (v1.x - v0.x);
		p2 = v2.x * (v0.y - v1.y) + v2.y * (v1.x - v0.x);
		r = e0 * abs(f0.y) + e1 * abs(f0.x);
		if (glm::max(-glm::max(p0, p2), glm::min(p0, p2)) > r)
		{
			// Separating axis found: not intersecting
			continue;
		}

		//a21
		p0 = v0.x * (v1.y - v2.y) + v0.y * (v2.x - v1.x);
		p1 = v1.y * v2.x - v1.x * v2.y;//v1.x * (v1.y - v2.y) + v1.y * (v2.x - v1.x);
		//p2 = v2.x * (v1.y - v2.y) + v2.y * (v2.x - v1.x);
		r = e0 * abs(f1.y) + e1 * abs(f1.x);
		if (glm::max(-glm::max(p0, p1), glm::min(p0, p1)) > r)
		{
			// Separating axis found: not intersecting
			continue;
		}

		//a21
		//p0 = v0.x * (v2.y - v0.y) + v0.y * (v0.x - v2.x);
		p1 = v1.x * (v2.y - v0.y) + v1.y * (v0.x - v2.x);
		p2 = v2.y * v0.x - v2.x * v0.y;//v2.x * (v2.y - v0.y) + v2.y * (v0.x - v2.x);
		r = e0 * abs(f2.y) + e1 * abs(f2.x);
		if (glm::max(-glm::max(p1, p2), glm::min(p1, p2)) > r)
		{
			// Separating axis found: not intersecting
			continue;
		}


		// Now we test the 3 axes corresponding to the face normals of the AABB (category 1)
		if ((glm::max(glm::max(v0.x, v1.x), v2.x) < -e0) || (glm::min(glm::min(v0.x, v1.x), v2.x) > e0))
			continue;
		if ((glm::max(glm::max(v0.y, v1.y), v2.y) < -e1) || (glm::min(glm::min(v0.y, v1.y), v2.y) > e1))
			continue;
		if ((glm::max(glm::max(v0.z, v1.z), v2.z) < -e2) || (glm::min(glm::min(v0.z, v1.z), v2.z) > e2))
			continue;


		// Test separating axis corresponding to triangle face normal (category 2)
		// We do it above, the plane AABB test, might move it down here later

		returnSet.push_back(*itTri);

	}

	return returnSet;
}

// If you wanna pass center and extents
bool BoxBoxIntersectTest2(glm::vec3 B1ctr, glm::vec3 B1ext, glm::vec3 B2ctr, glm::vec3 B2ext)
{
	return BoxBoxIntersectTest(B1ctr - B1ext, B1ctr - B1ext, B2ctr - B2ext, B2ctr + B2ext);
}

// If you wanna pass min and max points
bool BoxBoxIntersectTest(glm::vec3 B1minXYZ, glm::vec3 B1maxXYZ, glm::vec3 B2minXYZ, glm::vec3 B2maxXYZ)
{
	// Test if B1 max values are less than B2's min values
	if (B1maxXYZ.x < B2minXYZ.x) return false;
	if (B1maxXYZ.y < B2minXYZ.y) return false;
	if (B1maxXYZ.z < B2minXYZ.z) return false;

	// Test if B1 min values are more than B2's max values
	if (B1minXYZ.x > B2maxXYZ.x) return false;
	if (B1minXYZ.y > B2maxXYZ.y) return false;
	if (B1minXYZ.z > B2maxXYZ.z) return false;


	return true;

}




