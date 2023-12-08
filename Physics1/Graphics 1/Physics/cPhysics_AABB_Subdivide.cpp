// This divides up a model into AABB's in the form of an oct tree
// This is recursive
// Also has some useful functions for checking if a triangle is within a box!


#include "cAABB.h"

#include "sTriangle.h"

//#include <vector>

bool BoxBoxIntersectTest(glm::vec3 B1minXYZ, glm::vec3 B1maxXYZ, glm::vec3 B2minXYZ, glm::vec3 B2maxXYZ);
bool BoxBoxIntersectTest2(glm::vec3 B1ctr, glm::vec3 B1ext, glm::vec3 B2ctr, glm::vec3 B2ext);

void cAABB::MakeTree(std::vector<sTriangle_A>* parentTris, unsigned int maxTri)
{

}

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




