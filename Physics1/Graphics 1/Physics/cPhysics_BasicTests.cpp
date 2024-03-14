#include "cPhysics.h"


float Clamp(float n, float min, float max)
{
	if (n < min) return min;
	if (n > max) return max;
	return n;
}

// Support function that returns the AABB vertex with index n
// Used in IntersectMovingSphereAABB
glm::vec3 Corner(cAABB* b, int n)
{
	glm::vec3 p;
	p.x = ((n & 1) ? b->xMax() : b->xMin());
	p.y = ((n & 2) ? b->yMax() : b->yMin());
	p.z = ((n & 4) ? b->zMax() : b->zMin());

	return p;
}



//== = Section 5.2.7: ============================================================ =
//
// Returns true if sphere s intersects triangle ABC, false otherwise.
bool cPhysics::m_TestSphereTriangle(float sphereRadius, glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2,
						glm::vec3 sphereCentre)
{
	// Find point P on triangle ABC closest to sphere center
	glm::vec3 closestPoint = m_ClosestPtPointTriangle(sphereCentre, vert0, vert1, vert2);

	// Sphere and triangle intersect if the (squared) distance from sphere
	// center to point p is less than the (squared) sphere radius
	glm::vec3 v = closestPoint - sphereCentre;

	bool isItIntersecting = false;

	if (glm::dot(v, v) <= (sphereRadius * sphereRadius))
	{
		isItIntersecting = true;
	}

	return isItIntersecting;
}


int cPhysics::m_TestSphereAABB(sPhysicsProperties* pSphere, cAABB* b, glm::vec3& q)
{
	float radius = ((sPhysicsProperties::sSphere*)pSphere->pShape)->radius;
	// Find point q on AABB closest to sphere center
	m_ClosestPtPointAABB(pSphere->position, b, q);

	// Sphere and AABB intersect if the (squared) distance from sphere
	// center to point q is less than the (squarde) sphere radius
	glm::vec3 v = q - pSphere->position;
	return glm::dot(v, v) <= radius * radius;
}


// From: Real-Time Collision Detection- Ericson, Christer- 9781558607323- Books - Amazon.ca
// https://www.amazon.ca/Real-Time-Collision-Detection-Christer-Ericson/dp/1558607323/ref=pd_lpo_sccl_1/137-6663593-0701065?pd_rd_w=YiI8A&content-id=amzn1.sym.687e7c56-2b08-4044-894f-bbad969cf967&pf_rd_p=687e7c56-2b08-4044-894f-bbad969cf967&pf_rd_r=JWS56NJC99QEH56TYFJX&pd_rd_wg=zBE6V&pd_rd_r=d611733e-ec29-4b30-bd70-89f092f1991a&pd_rd_i=1558607323&psc=1
// Chapter 5:
glm::vec3 cPhysics::m_ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 bc = c - b;

	// Compute parametric position s for projection P' of P on AB,
	// P' = A + s*AB, s = snom/(snom+sdenom)
	float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);

	// Compute parametric position t for projection P' of P on AC,
	// P' = A + t*AC, s = tnom/(tnom+tdenom)
	float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

	if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

	// Compute parametric position u for projection P' of P on BC,
	// P' = B + u*BC, u = unom/(unom+udenom)
	float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

	if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
	if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out


	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
	glm::vec3 n = glm::cross(b - a, c - a);
	float vc = glm::dot(n, glm::cross(a - p, b - p));
	// If P outside AB and within feature region of AB,
	// return projection of P onto AB
	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		return a + snom / (snom + sdenom) * ab;

	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
	float va = glm::dot(n, glm::cross(b - p, c - p));
	// If P outside BC and within feature region of BC,
	// return projection of P onto BC
	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		return b + unom / (unom + udenom) * bc;

	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
	float vb = glm::dot(n, glm::cross(c - p, a - p));
	// If P outside CA and within feature region of CA,
	// return projection of P onto CA
	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
		return a + tnom / (tnom + tdenom) * ac;

	// P must project inside face region. Compute Q using barycentric coordinates
	float u = va / (va + vb + vc);
	float v = vb / (va + vb + vc);
	float w = 1.0f - u - v; // = vc / (va + vb + vc)
	return u * a + v * b + w * c;
}


// Slightly modified version of above, but takes into account the point it is comparing is already on the plane of the triangle
// Thus returning the point provided if it gets past the vertex and edge tests
glm::vec3 cPhysics::m_ClosestPtTriPlanePointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 bc = c - b;

	// Compute parametric position s for projection P' of P on AB,
	// P' = A + s*AB, s = snom/(snom+sdenom)
	float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);

	// Compute parametric position t for projection P' of P on AC,
	// P' = A + t*AC, s = tnom/(tnom+tdenom)
	float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

	if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

	// Compute parametric position u for projection P' of P on BC,
	// P' = B + u*BC, u = unom/(unom+udenom)
	float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

	if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
	if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out


	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
	glm::vec3 n = glm::cross(b - a, c - a);
	float vc = glm::dot(n, glm::cross(a - p, b - p));
	// If P outside AB and within feature region of AB,
	// return projection of P onto AB
	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		return a + snom / (snom + sdenom) * ab;

	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
	float va = glm::dot(n, glm::cross(b - p, c - p));
	// If P outside BC and within feature region of BC,
	// return projection of P onto BC
	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		return b + unom / (unom + udenom) * bc;

	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
	float vb = glm::dot(n, glm::cross(c - p, a - p));
	// If P outside CA and within feature region of CA,
	// return projection of P onto CA
	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
		return a + tnom / (tnom + tdenom) * ac;


	// P must be in face region, so return the provided point!
	return p;
}

// Returns distance as a float
// Sets c1 and c2 points as the closest points of each segment
// S1(s) = P1 + s * (Q1 - P1)
// S2(t) = P2 + t * (Q2 - P2)
float cPhysics::m_ClosestPtSegmentSegment(glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, glm::vec3 q2, float& s, float& t, glm::vec3& c1, glm::vec3& c2)
{
	// Direction vectors of both segments
	glm::vec3 d1 = q1 - p1;
	glm::vec3 d2 = q2 - p2;
	
	glm::vec3 r = p1 - p2;
	float a = glm::dot(d1, d1); // Squared length of S1
	float e = glm::dot(d2, d2); // Squared length of S2
	float f = glm::dot(d2, r);


	// Check if either or both points degenerate into points
	if (a <= std::numeric_limits<float>::epsilon() && e <= std::numeric_limits<float>::epsilon())
	{
		// Both segments degenerate into points
		s = 0.0f;
		t = 0.0f;
		c1 = p1;
		c2 = p2;
		return glm::dot(c1 - c2, c1 - c2);
	}
	if (a <= std::numeric_limits<float>::epsilon())
	{
		// First segment degenerates into a point
		s = 0.0f;
		t = f / e;
		t = Clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = glm::dot(d1, r);
		if (e <= std::numeric_limits<float>::epsilon())
		{
			// Second segment degenerates into a point
			t = 0.0f;
			s = Clamp(-c / a, 0.0f, 1.0f);
		}
		else
		{
			// General nondegenerate case starts here
			float b = glm::dot(d1, d2);
			float denom = a * e - b * b;

			// If segments not parallel, compute closest point on L1 to L2 and
			// clamp to segment S1. Else pick arbitrary s (here 0)
			if (denom != 0.0f)
			{
				s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			}
			else s = 0.0f;

			// Compute point on L2 closest to S1(s) using
			// t = dot((P1 + D1 * s) - P2, D2) / dot(D2, D2) = b*s + f) / e
			//t = (b * s + f) / e;
			float tnom = b * s + f;

			// If t in [0, 1] dont. Else clamp t, recompute s for new value
			// of t using s = dot((P2 + D2 * t) - P1, D1) / dot(D1, D1) = (t * b - c) / a
			// and clamp s to [0, 1]
// 			if (t < 0.0f)
// 			{
// 				t = 0.0f;
// 				s = Clamp(-c / a, 0.0f, 1.0f);
// 			}
// 			else if (t > 1.0f) 
// 			{
// 				t = 1.0f;
// 				s = Clamp((b - c) / a, 0.0f, 1.0f);
// 			}
			if (tnom < 0.0f)
			{
				t = 0.0f;
				s = Clamp(-c / a, 0.0f, 1.0f);
			}
			else if (tnom > e)
			{
				t = 1.0f;
				s = Clamp((b - c) / a, 0.0f, 1.0f);
			}
			else
			{
				t = tnom / e;
			}
		}
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;
	return glm::dot(c1 - c2, c1 - c2);
}



// Gets the closest point on the triangle to the line segment
glm::vec3 cPhysics::m_ClosestPtLineSegTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	// Compare 3 edges of triangle with segment with above function
	// For each endpoint, if they project into triangle compute point to plane distance
	// Last test would be checking for an intersection

	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 line = p2 - p1;


	// Should start by checking if both endpoints lie in the triangle if projected onto it
	// If both do, either the endpoint is the closest point or it intersects and we can return


	// Start by getting the normal of the triangle
	glm::vec3 triNorm = glm::cross(ab, ac);




	// If line is parallel to triangle
	if (glm::dot(line, triNorm) <= 0)
	{
		// Calculate point(on line) to plane distance and return
	}





	float s, t;
	glm::vec3 c1, c2;

	float smallestDistanceSoFar = FLT_MAX;
	float currDist;

	// First edge
	currDist = m_ClosestPtSegmentSegment(p1, p2, a, b, s, t, c1, c2);
	if (currDist < smallestDistanceSoFar) smallestDistanceSoFar = currDist;

	// Second edge
	currDist = m_ClosestPtSegmentSegment(p1, p2, b, c, s, t, c1, c2);
	if (currDist < smallestDistanceSoFar) smallestDistanceSoFar = currDist;

	// Third edge
	currDist = m_ClosestPtSegmentSegment(p1, p2, c, a, s, t, c1, c2);
	if (currDist < smallestDistanceSoFar) smallestDistanceSoFar = currDist;





	return glm::vec3(0);
}


void cPhysics::m_ClosestPtPointAABB(glm::vec3 p, cAABB* b, glm::vec3& q)
{
	for (unsigned int i = 0; i < 3; i++)
	{
		float v = p[i];
		if (v < b->XYZMin()[i]) v = b->XYZMin()[i]; // v = max(v, b.min[i])
		if (v > b->XYZMax()[i]) v = b->XYZMax()[i]; // v = min(v, b.max[i])
		q[i] = v;
	}
}


void cPhysics::m_ClosestPtSegmentAABB(glm::vec3& p1, glm::vec3& p2, cAABB& b, glm::vec3& q)
{
	// Closest point is one of 3:
	// - Segment to edge (segment-segment)
	// - Endpoint to face (point-plane)
	// - Endpoint to edge (point-segment)
}


int cPhysics::m_IntersectMovingSpherePlane(sPhysicsProperties* pSphere, glm::vec3 pn, float pd, float& t, glm::vec3& q)
{
	sPhysicsProperties::sSphere* pSphereShape = (sPhysicsProperties::sSphere*)(pSphere->pShape); // To access the radius

	glm::vec3 v = pSphere->position - pSphere->oldPosition;

	float dist = glm::dot(pn, pSphere->oldPosition) - pd;

	if (abs(dist) <= pSphereShape->radius) //This isn't where it is intersecting, but this case should almost never happen :)
	{
		// We are somehow in the triangle, so check if we're moving from back-facing to front-facing, or riding parallel along it
		float denom = glm::dot(pn, v);
		//if (denom * dist >= 0.0f)
		if (denom * dist >= -4.5e-06) // Floating point error bandaid
		{
			// No intersection; sphere is moving parallel or away from plane
			return 0;
		}

		// Sphere is already overlapping plane and moving towards it
		// Set time of intersection to 0 and q to sphere center
		t = 0.0f;
		//q = pSphere->oldPosition;
		q = pSphere->oldPosition - (pn * dist);

		return 1;
	}
	else
	{
	// Avoiding scenario where already in sphere, should never happen. Lets objects pass through backs of tris
		float denom = glm::dot(pn, v);
		if (denom * dist >= 0.0f)
		{
			// No intersection; sphere is moving parallel or away from plane
			return 0;
		}
		else
		{
			// Sphere moving towards the plane

			// Use +r in computations if sphere in front of plane, else -r
			float r = dist > 0.0f ? pSphereShape->radius : -pSphereShape->radius;
			t = (r - dist) / denom;
			q = pSphere->oldPosition + t * v - r * pn;
			return 1;
		}
	}
}


int cPhysics::m_IntersectRaySphere(glm::vec3 p, glm::vec3 d, sPhysicsProperties* pSphere, float& t, glm::vec3& q) // This can be cheaper by only detecting if a ray hits it, doesn't care about t or q (pg. 179)
{
	sPhysicsProperties::sSphere* pSphereShape = (sPhysicsProperties::sSphere*)(pSphere->pShape); // To access the radius

	//d = glm::normalize(d);

	glm::vec3 m = p - pSphere->oldPosition;
	float b = glm::dot(m, d);
	float c = glm::dot(m, m) - pSphereShape->radius * pSphereShape->radius;

	// Exit if r's origin outside s (c > 0) and r pointing away from s (b > 0)
	if (c > 0.0f && b > 0.0f) return 0;
	float discr = b * b - c;

	// Negative discriminant corresponds to ray missing sphere
	if (discr < 0.0f) return 0;

	// Ray now found to intersect sphere, compute smallest t value of intersection
	t = -b - sqrt(discr);

	// If t is negative, ray started inside sphere so clamp t to zero
	if (t < 0.0f) t = 0.0f;
	q = p + t * d;
	return 1;
}

int cPhysics::m_IntersectRayAABB(glm::vec3 p, glm::vec3 d, cAABB* a, float& tmin, glm::vec3& q) // pg. 180
{
	tmin = 0.0f; // Set to -FLT_MAX to get first hit on line
	float tmax = FLT_MAX;

	// For all 3 slabs
	for (unsigned int i = 0; i < 3; i++)
	{
		if (abs(d[i]) < std::numeric_limits<float>::epsilon())
		{
			// Ray is parallel to slab; no hit if origin not within slab
			if (p[i] < a->XYZMin()[i] || p[i] > a->XYZMax()[i]) return 0;
		}
		else
		{
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / d[i];
			float t1 = (a->XYZMin()[i] - p[i]) * ood;
			float t2 = (a->XYZMax()[i] - p[i]) * ood;

			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) // Swap them
			{
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}

			// Compute the intersection of slab intersection intervals
			tmin = glm::max<float>(tmin, t1);
			tmax = glm::min<float>(tmax, t2);

			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return 0;
		}
	}

	// Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
	q = p + d * tmin;
	return 1;
}

int cPhysics::m_IntersectMovingSphereAABB(sPhysicsProperties* pSphere, cAABB* b, float& t) // TODO don't need t, so can prob reduce the load on this function?
{
	glm::vec3 d = pSphere->position - pSphere->oldPosition;
	float r = ((sPhysicsProperties::sSphere*)pSphere->pShape)->radius;

	// Compute the AABB resulting from expanding b by the sphere radius r
	cAABB e; // Don't want to copy over all data (especially triangles if leaf node) so only copy over dimensions
	e.centerPosition = b->centerPosition;
	e.halfLengths = b->halfLengths + glm::vec3(r);

	glm::vec3 q;
	if (!m_IntersectRayAABB(pSphere->oldPosition, d, &e, t, q) || t > 1.0f)
	{
		return 0;
	}

	// For now, we won't check the proper rounded corners; i.e. this function is incomplete but still works

	return 1;
}


int cPhysics::m_IntersectMovingCapsuleAABB(sPhysicsProperties* pCapsule, cAABB* b)
{
	// Make a lozenge with the 2 capsule positions, see if it intersects the aabb. Simple, no? :)

	// OTHER: Make line with old and new pos, get closest point on that to aabb.
	// Then from that point make a line perpendicular that represents the height, closest point and compare if dist < radius

	glm::vec3 d = pCapsule->position - pCapsule->oldPosition;
	sPhysicsProperties::sCapsule* capsule= ((sPhysicsProperties::sCapsule*)pCapsule->pShape);
	float r = capsule->radius;

	cAABB e;
	e.centerPosition = b->centerPosition;
	e.halfLengths = b->halfLengths + glm::vec3(r);

	glm::vec3 currUpVec = pCapsule->upVec * capsule->halfLength;

	glm::vec3 oldUp = pCapsule->oldPosition + currUpVec;
	glm::vec3 newUp = pCapsule->position + currUpVec;


	if (m_IntersectSegmentAABB(oldUp, newUp, e)) return 1; // Check segment made by top of capsule line

	glm::vec3 oldDown = pCapsule->oldPosition - currUpVec;
	glm::vec3 newDown = pCapsule->position - currUpVec;
	if (m_IntersectSegmentAABB(oldDown, newDown, e)) return 1; // Check segment made by bottom of capsule line
	if (m_IntersectSegmentAABB(newUp, newDown, e)) return 1; // Check segment made by capsule line of new position

	return 0;
}



int cPhysics::m_IntersectSegmentAABB(glm::vec3& p0, glm::vec3& p1, cAABB& b)
{
	glm::vec3 c = b.centerPosition;
	glm::vec3 e = b.halfLengths;
	glm::vec3 m = (p0 + p1) * 0.5f;
	glm::vec3 d = p1 - m;
	m = m - c;

	// Try world coordinate axes as separating axes
	float adx = abs(d.x);
	if (abs(m.x) > e.x + adx) return 0;

	float ady = abs(d.y);
	if (abs(m.y) > e.y + ady) return 0;

	float adz = abs(d.z);
	if (abs(m.z) > e.z + adz) return 0;


	// Add epsilon term to counteract arithmetic errors when segment is 
	// (near) parallel to a coordinate axis

	adx += std::numeric_limits<float>::epsilon(); ady += std::numeric_limits<float>::epsilon(); adz += std::numeric_limits<float>::epsilon();

	// Try cross products of segments of segment direction vector with coordinate axes
	if (abs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady) return 0;
	if (abs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx) return 0;
	if (abs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx) return 0;

	// No separating axis found, segment must be overlapping  AABB
	return 1;
}

int cPhysics::m_IntersectSegmentTriangle(glm::vec3 p, glm::vec3 q, sTriangle_A* pTri/*glm::vec3 a, glm::vec3 b, glm::vec3 c*/, float& u, float& v, float& w, glm::vec3& norm, float& t)
{
	glm::vec3 a = pTri->vertices[0];
	glm::vec3 b = pTri->vertices[1];
	glm::vec3 c = pTri->vertices[2];

	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 qp = p - q;

	

	glm::vec3 n = glm::cross(ab, ac);


	float d = glm::dot(qp, n);
	if (d <= 0.0f) return 0;


	glm::vec3 ap = p - a;
	t = glm::dot(ap, n);
	if (t < 0.0f) return 0;
	if (t > d) return 0;    // Exclude this for a ray test!


	// Compute barycentric coords and test if within bounds
	glm::vec3 e = glm::cross(qp, ap);
	v = glm::dot(ac, e);
	if (v < 0.0f || v > d) return 0;

	w = -glm::dot(ab, e);
	if (w < 0.0f || v + w > d) return 0;

	// Segment/ray intersects triangle. Perform delayed division and compute last barycentric coord
	norm = glm::normalize(n);
	//return 1;


	float ood = 1.0f / d;
	t *= ood;
	v *= ood;
	w *= ood;
	u = 1.0f - v - w;

	return 1;
}