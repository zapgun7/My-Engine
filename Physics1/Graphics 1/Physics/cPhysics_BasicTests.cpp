#include "cPhysics.h"


float Clamp(float n, float min, float max)
{
	if (n < min) return min;
	if (n > max) return max;
	return n;
}



//== = Section 5.2.7: ============================================================ =
//
// Returns true if sphere s intersects triangle ABC, false otherwise.
bool cPhysics::m_TestSphereTriangle(float sphereRadius, glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2,
						glm::vec3 sphereCentre)
{
	// Find point P on triangle ABC closest to sphere center
	glm::vec3 closestPoint = this->m_ClosestPtPointTriangle(sphereCentre, vert0, vert1, vert2);

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
	if (a <= std::numeric_limits<double>::epsilon() && e <= std::numeric_limits<double>::epsilon())
	{
		// Both segments degenerate into points
		s = 0.0f;
		t = 0.0f;
		c1 = p1;
		c2 = p2;
		return glm::dot(c1 - c2, c1 - c2);
	}
	if (a <= std::numeric_limits<double>::epsilon())
	{
		// First segment degenerates into a point
		s = 0.0f;
		t = f / e;
		t = Clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = glm::dot(d1, r);
		if (e <= std::numeric_limits<double>::epsilon())
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

