#include "cSoftBodyVerlet.h"

#if _DEBUG 
#include <iostream>
#endif

cSoftBodyVerlet::cSoftBodyVerlet()
{

}

cSoftBodyVerlet::~cSoftBodyVerlet()
{

}


// This is for loading the original model
bool cSoftBodyVerlet::CreateSoftBody(sModelDrawInfo ModelInfo, glm::mat4 matInitalTransform /*=glm::mat4(1.0f)*/ )
{
	// Copy the model info
	this->m_ModelVertexInfo = ModelInfo;
	// Copy the vertices and indices as well
	// The cModelDrawInfo has pointers to the originally loaded vertices.
	sVertex* pVerticesLocalCopy = new sVertex[ModelInfo.numberOfVertices];
	for (unsigned int index = 0; index != ModelInfo.numberOfVertices; index++)
	{
		// TODO: If we are taking these vertices, apply some soft boy tranform, 
		//	then updating the original VAO buffers (which we ARE going to do),
		//	then we have to be aware that this will CHANGE the original VAO.
		// In other words, if we have TWO bouncy soft bunnies, we will
		//	have to load TWO meshes into the VAO...because the soft body
		//	physics will update each one. 
		glm::vec4 theVertex = glm::vec4(ModelInfo.pVertices[index].x,
										ModelInfo.pVertices[index].y,
										ModelInfo.pVertices[index].z, 1.0f);
		// Apply transformation
		theVertex = matInitalTransform * theVertex;

		pVerticesLocalCopy[index].x = theVertex.x;
		pVerticesLocalCopy[index].y = theVertex.y;
		pVerticesLocalCopy[index].z = theVertex.z;
		pVerticesLocalCopy[index].w = ModelInfo.pVertices[index].w;

//		pVerticesLocalCopy[index].x = ModelInfo.pVertices[index].x;
//		pVerticesLocalCopy[index].y = ModelInfo.pVertices[index].y;
//		pVerticesLocalCopy[index].z = ModelInfo.pVertices[index].z;
//		pVerticesLocalCopy[index].w = ModelInfo.pVertices[index].w;

// 		pVerticesLocalCopy[index].r = ModelInfo.pVertices[index].r;
// 		pVerticesLocalCopy[index].g = ModelInfo.pVertices[index].g;
// 		pVerticesLocalCopy[index].b = ModelInfo.pVertices[index].b;
// 		pVerticesLocalCopy[index].a = ModelInfo.pVertices[index].a;

		pVerticesLocalCopy[index].nx = ModelInfo.pVertices[index].nx;
		pVerticesLocalCopy[index].ny = ModelInfo.pVertices[index].ny;
		pVerticesLocalCopy[index].nz = ModelInfo.pVertices[index].nz;
		pVerticesLocalCopy[index].nw = ModelInfo.pVertices[index].nw;

		pVerticesLocalCopy[index].u = ModelInfo.pVertices[index].u;
		pVerticesLocalCopy[index].v = ModelInfo.pVertices[index].v;
	}//for (unsigned int index



	unsigned int* pIndicesLocalCopy = new unsigned int[ModelInfo.numberOfIndices];
	for (unsigned int index = 0; index != ModelInfo.numberOfIndices; index++)
	{
		pIndicesLocalCopy[index] = ModelInfo.pIndices[index];
	}//for (unsigned int index

	this->m_ModelVertexInfo.pVertices = pVerticesLocalCopy;
	this->m_ModelVertexInfo.pIndices = pIndicesLocalCopy;



	// From this point on, we are referencing the copy of the original vertices

	// Go through the model info and add the particles from the vertex locations
	this->vec_pParticles.reserve(this->m_ModelVertexInfo.numberOfVertices);

	for (unsigned int index = 0; index != this->m_ModelVertexInfo.numberOfVertices; index++)
	{
		sParticle* pParticle = new sParticle();
		pParticle->position.x = this->m_ModelVertexInfo.pVertices[index].x;
		pParticle->position.y = this->m_ModelVertexInfo.pVertices[index].y;
		pParticle->position.z = this->m_ModelVertexInfo.pVertices[index].z;

		pParticle->old_position = pParticle->position;

		// Save the pointer address.
		// Note the "&" (address of)
		pParticle->pModelVertex = &(this->m_ModelVertexInfo.pVertices[index]);

		this->vec_pParticles.push_back(pParticle);
	}//for ( unsigned int index =...

	// Constraints
	// Note: we are stepping through the array by 3s (3 vertices per triangle)
	for ( unsigned int index = 0; index < this->m_ModelVertexInfo.numberOfIndices; index += 3 )
	{
		// Example: ply file has “3 733 1026 736” for this triangle.
		// pIndices would have:
		// 
		//	 733	this->m_ModelVertexInfo.pIndices[ index + 0 ]
		//  1026	this->m_ModelVertexInfo.pIndices[ index + 1 ]
		//   736	this->m_ModelVertexInfo.pIndices[ index + 2 ]
		//
		sParticle* pParticle1 = this->vec_pParticles[ this->m_ModelVertexInfo.pIndices[index /* +0 */] ];
		sParticle* pParticle2 = this->vec_pParticles[ this->m_ModelVertexInfo.pIndices[index + 1] ];
		sParticle* pParticle3 = this->vec_pParticles[ this->m_ModelVertexInfo.pIndices[index + 2] ];

		// Add neighbours
		pParticle1->neighbours.push_back(pParticle2); pParticle1->neighbours.push_back(pParticle3);
		pParticle2->neighbours.push_back(pParticle1); pParticle2->neighbours.push_back(pParticle3);
		pParticle3->neighbours.push_back(pParticle1); pParticle3->neighbours.push_back(pParticle2);


		sConstraint* pEdge1 = new sConstraint();
		pEdge1->pParticleA = pParticle1;
		pEdge1->pParticleB = pParticle2;
		pEdge1->restLength = this->calcDistanceBetween(pEdge1->pParticleA, pEdge1->pParticleB);
		pEdge1->tightFact = this->tightnessFactor;

		sConstraint* pEdge2 = new sConstraint();
		pEdge2->pParticleA = pParticle2;
		pEdge2->pParticleB = pParticle3;
		pEdge2->restLength = this->calcDistanceBetween(pEdge2->pParticleA, pEdge2->pParticleB);
		pEdge2->tightFact = this->tightnessFactor;

		sConstraint* pEdge3 = new sConstraint();
		pEdge3->pParticleA = pParticle3;
		pEdge3->pParticleB = pParticle1;
		pEdge3->restLength = this->calcDistanceBetween(pEdge3->pParticleA, pEdge3->pParticleB);
		pEdge3->tightFact = this->tightnessFactor;

		this->vec_pConstraints.push_back(pEdge1);
		this->vec_pConstraints.push_back(pEdge2);
		this->vec_pConstraints.push_back(pEdge3);
	}//for ( unsigned int index

	return true;
}

void cSoftBodyVerlet::UpdateVertexPositions(void)
{
	// Copy the current particle positions to the local vertex locations

	// ENTER CRITICAL SECTION
	for ( sParticle* curParticle : this->vec_pParticles)
	{
		if (!curParticle->isPartModel) continue;
		curParticle->pModelVertex->x = curParticle->position.x;
		curParticle->pModelVertex->y = curParticle->position.y;
		curParticle->pModelVertex->z = curParticle->position.z;
	}
	// LEAVE CRITICAL SECTION

	// At this point, our local sModelDrawInfo pVertices array has the 
	//	current locations of the particles (i.e. the mesh is updated)

	// TODO: Update normals, etc.

	return;
}

void cSoftBodyVerlet::UpdateNormals(void)
{
	// Go through each triagle and calculate the normals. 
	// Accumulate those normals at each vertex.
	// Normalize the normals

	// Clear the existing normals because they are invalid
	for ( unsigned int vertIndex = 0; vertIndex != this->m_ModelVertexInfo.numberOfVertices; vertIndex++ )
	{
		this->m_ModelVertexInfo.pVertices[vertIndex].nx = 0.0f;
		this->m_ModelVertexInfo.pVertices[vertIndex].ny = 0.0f;
		this->m_ModelVertexInfo.pVertices[vertIndex].nz = 0.0f;
	}


	for ( unsigned int triIndex = 0; triIndex != this->m_ModelVertexInfo.numberOfTriangles; triIndex++ ) // Dog walking time
	{
		// Indices are sets of 3, one per 
		unsigned int vertAIndex = this->m_ModelVertexInfo.pIndices[triIndex * 3 + 0];
		unsigned int vertBIndex = this->m_ModelVertexInfo.pIndices[triIndex * 3 + 1];
		unsigned int vertCIndex = this->m_ModelVertexInfo.pIndices[triIndex * 3 + 2];

		// note the references so that when we update this, it will update the mesh
		// (otherwise we'll be updating a copy of it)
		sVertex& vertexA = this->m_ModelVertexInfo.pVertices[vertAIndex];
		sVertex& vertexB = this->m_ModelVertexInfo.pVertices[vertBIndex];
		sVertex& vertexC = this->m_ModelVertexInfo.pVertices[vertCIndex];

		glm::vec3 vertA = glm::vec3(vertexA.x, vertexA.y, vertexA.z);
		glm::vec3 vertB = glm::vec3(vertexB.x, vertexB.y, vertexB.z);
		glm::vec3 vertC = glm::vec3(vertexC.x, vertexC.y, vertexC.z);

		glm::vec3 triangleEdgeAtoB = vertB - vertA;
		glm::vec3 triangleEdgeAtoC = vertC - vertA;

		glm::vec3 theNormal = glm::cross(triangleEdgeAtoB, triangleEdgeAtoC);
		theNormal = glm::normalize(theNormal);

		// Add (accumulate) this normal to the three vertices
		vertexA.nx += theNormal.x;
		vertexA.ny += theNormal.y;
		vertexA.nz += theNormal.z;

		vertexB.nx += theNormal.x;
		vertexB.ny += theNormal.y;
		vertexB.nz += theNormal.z;

		vertexC.nx += theNormal.x;
		vertexC.ny += theNormal.y;
		vertexC.nz += theNormal.z;
	}// for ( unsigned int triIndex = 0

	// ENTER CRITICAL SECTION

	// Now normalize the accumulated normals
	for (unsigned int vertIndex = 0; vertIndex != this->m_ModelVertexInfo.numberOfVertices; vertIndex++)
	{
		glm::vec3 theNomral = glm::vec3(this->m_ModelVertexInfo.pVertices[vertIndex].nx,
										this->m_ModelVertexInfo.pVertices[vertIndex].ny,
										this->m_ModelVertexInfo.pVertices[vertIndex].nz);

		theNomral = glm::normalize(theNomral);

		this->m_ModelVertexInfo.pVertices[vertIndex].nx = theNomral.x;
		this->m_ModelVertexInfo.pVertices[vertIndex].ny = theNomral.y;
		this->m_ModelVertexInfo.pVertices[vertIndex].nz = theNomral.z;
	}

	// LEAVE CRITICAL SECTION

	return;
}


float cSoftBodyVerlet::calcDistanceBetween(sParticle* pPartA, sParticle* pPartB)
{
	return glm::distance(pPartA->position, pPartB->position);
}



void cSoftBodyVerlet::VerletUpdate(double deltaTime)
{
	if (deltaTime > MAX_DELTATIME)
	{
		deltaTime = MAX_DELTATIME;
	}


	for (sParticle* pCurrentParticle : vec_pParticles )
	{
		glm::vec3 current_pos = pCurrentParticle->position;
		glm::vec3 old_pos = pCurrentParticle->old_position;

// From Michael's ancient (2016) code:
//		CVector& x = this->m_vec_x[index];
//		CVector temp = x;
//		CVector& oldx = this->m_vec_oldx[index];
//
//		CVector& a = this->m_vec_a[index];
//		x += x - oldx + (a * fTimeStep * fTimeStep);
//
//		oldx = temp;

		// This is the actual Verlet integration step (notice there isn't a velocity)
		pCurrentParticle->position += (current_pos - old_pos) + (this->acceleration * (float)(deltaTime * deltaTime) );

		pCurrentParticle->old_position = current_pos;

		// Check if there is a LARGE different between old and new positions


		this->cleanZeros(pCurrentParticle->position);
		this->cleanZeros(pCurrentParticle->old_position);
	}

	return;
}

void cSoftBodyVerlet::ApplyCollision(double deltaTime)
{
	bool isGrounded = false;
	// HACK: Stop any particles that go below the "ground"
	for (sParticle* pCurrentParticle : vec_pParticles)
	{
		if ( pCurrentParticle->position.y < 0.0f )
		{
			pCurrentParticle->position.y = 0.0f;
			isGrounded = true;

			// "Friction"
			const float FRICTION_VAL = 2.0f; // Higher = more friction

			// X
			float diff = (pCurrentParticle->old_position.x - pCurrentParticle->position.x) * FRICTION_VAL;
			pCurrentParticle->position.x += diff * static_cast<float>(deltaTime);

			// Z
			diff = (pCurrentParticle->old_position.z - pCurrentParticle->position.z) * FRICTION_VAL;
			pCurrentParticle->position.z += diff * static_cast<float>(deltaTime);
		}
	}

	canJump = isGrounded;


//	this->vec_pParticles[5'000]->position = glm::vec3(0.0f, 30.0f, 0.0f);

	// Collide with a sphere at 20 units above the origin
	//	with a radius of 5 units.
	// Check to see if this particle is inside this sphere...
// 	for (sParticle* pCurrentParticle : vec_pParticles)
// 	{
// 		glm::vec3 sphereCentre = glm::vec3(0.0f, 20.0f, 24.0f);
// 		float sphereRadius = 15.0f;
// 
// 		float distanceToSphere = glm::distance(pCurrentParticle->position,
// 											   sphereCentre);
// 		if (distanceToSphere < sphereRadius )
// 		{
// 			// it's 'inside' the sphere
// 			// Shift or slide the point along the ray from the centre of the sphere
// 			glm::vec3 particleToCentreRay = pCurrentParticle->position - sphereCentre;
// 			// Normalize to get the direction
// 			particleToCentreRay = glm::normalize(particleToCentreRay);
// 			// 
// 			pCurrentParticle->position = (particleToCentreRay * sphereRadius) + sphereCentre;
// 
// 		}
// 	}//for (sParticle* pCurrentParticle



	return;
}


void cSoftBodyVerlet::SatisfyConstraints(void)
{
	//const unsigned int NUM_ITERATIONS = 1;
	
	for ( unsigned int iteration = 0; iteration != this->iterations; iteration++ )
	{
		// This is ONE pass of the constraint resolution
		for (sConstraint* pCurConstraint : this->vec_pConstraints )
		{
			if ( pCurConstraint->bIsActive )
			{


				cSoftBodyVerlet::sParticle* pX1 = pCurConstraint->pParticleA;
				cSoftBodyVerlet::sParticle* pX2 = pCurConstraint->pParticleB;
		
				glm::vec3 delta = pX2->position - pX1->position;

				float deltaLength = glm::length(delta);


				float diff = (deltaLength - pCurConstraint->restLength) / deltaLength;

				// -diff = push     +diff = pull
				if (pCurConstraint->relationType == sConstraint::PULL)
				{
					diff = diff < 0 ? 0 : diff;
				}
				else if (pCurConstraint->relationType == sConstraint::PUSH)
				{
					diff = diff > 0 ? 0 : diff;
				}

				// If we were having this 'tear' or break apart, 
				//	you could check some maximum length and if it's 'too long'
				//	then the constraint 'breaks'
				// Handle this by:
				// - Setting a bool (where it doesn't check the constraint any more)
				// - Remove the constraint (but removing from a vector is sketchy...)

//				if ( diff > 0.1f )
//				{
//					pCurConstraint->bIsActive = false;
//				}

				// Making this non-one, will change how quickly the objects move together
				// For example, making this < 1.0 will make it "bouncier"
				//float tightnessFactor = 0.01f;
				float tightFac = pCurConstraint->tightFact;

				if (!pX1->isStatic)
					pX1->position += delta * 0.5f * diff * tightFac;
				if (!pX2->isStatic)
					pX2->position -= delta * 0.5f * diff * tightFac;

				this->cleanZeros(pX1->position);
				this->cleanZeros(pX2->position);
			}//if (pCurConstraint->bIsActive)

		}//for (sConstraint* pCurConstraint...
	}//for ( unsigned int iteration

	return;
}

void cSoftBodyVerlet::cleanZeros(glm::vec3& value)
{
	// 1.192092896e–07F 
	const float minFloat = 1.192092896e-07f;
	if ((value.x < minFloat) && (value.x > -minFloat))
	{
		value.x = 0.0f;
	}
	if ((value.y < minFloat) && (value.y > -minFloat))
	{
		value.y = 0.0f;
	}
	if ((value.z < minFloat) && (value.z > -minFloat))
	{
		value.z = 0.0f;
	}
}

// Michael's original 2016 code from the original Hitman article on gamasutra:
// from the satisfyConstraints() method
//for (int itCount = 0; itCount != 10; itCount++)
////for ( int itCount = 0; itCount != this->numIterations; itCount++ )
//{
//	int numConstraints = this->m_vecConstraints.size();
//	for (int index = 0; index != numConstraints; index++)
//	{
//
//		// Apply constraints...
//		if (index > static_cast<int>(this->m_vecConstraints.size()))
//		{
//			int x = 0;
//		}
//		CConstraint& c = this->m_vecConstraints[index];
//		// 
//		unsigned int indexA = c.particleA;
//		unsigned int indexB = c.particleB;
//
//		if (indexA > this->m_numParticles)
//		{
//			int stop = 0;
//		}
//		if (indexB > this->m_numParticles)
//		{
//			int stop = 0;
//		}
//
//		CVector& x1 = this->m_vec_x[indexA];
//		CVector& x2 = this->m_vec_x[indexB];
//		CVector delta = x2 - x1;
//		float deltaMag = delta.Magnitude();
//		float deltalength = sqrt(deltaMag * deltaMag);
//
//		// Speed up by placing this on separate lines... why? who knows?
//		// (Actually, I think it's because of some wonkly bug in the CVector3f...)
//		//float diff=( deltalength - c.restlength ) / deltalength;
//
//		float diffA = (deltalength - c.restlength);
//		float diff = diffA / deltalength;
//
//		// Making this non-one, will change how quickly the objects move together
//		// For example, making this < 1.0 will make it "bouncier"
//		float tightnessFactor = 1.0f;
//
//		x1 += delta * 0.5 * diff * tightnessFactor;
//		x2 -= delta * 0.5 * diff * tightnessFactor;
//
//		x1.CleanZero();
//		x2.CleanZero();
//	}
//}// for ( int itCount = 0; itCount != this->numIterations; 


glm::vec3 cSoftBodyVerlet::getCentrePoint(void)
{
	glm::vec3 centreXYZ = glm::vec3(0.0f);

	for (sParticle* pCurrentParticle : this->vec_pParticles)
	{
		centreXYZ += pCurrentParticle->position;
	}
	// Get average
	centreXYZ /= (float)this->vec_pParticles.size();

	return centreXYZ;
}


void cSoftBodyVerlet::findCentreVerticesOfWheel(std::vector< unsigned int >& particleIndices)
{
	particleIndices.clear(); 

	// Wheel is 1.0 x 1.0 x 0.3
	// 'hub' in the wheel looks like it's around 0.1 wide
	const float CLOSE_ENOUGH_EPSILON = 0.05f;

	for ( unsigned int index = 0; index != this->vec_pParticles.size(); index++ )
	{
		sParticle* pCurrentParticle = this->vec_pParticles[index];

		if ( fabs(pCurrentParticle->position.y) <= CLOSE_ENOUGH_EPSILON )
		{
			if ( fabs(pCurrentParticle->position.z) <= CLOSE_ENOUGH_EPSILON )
			{
				// Likely along the axis of the wheel
				particleIndices.push_back(index);
			}
		}
	}//for ( unsigned int index

	return;
}

	// Creates constraints from the other vertices to the edges of the wheel
void cSoftBodyVerlet::CreateWheelBracing(void)
{
	// When the wheel is at the origin, the three vertices along the x axis are:
	//  43
	// 113
	// 167
	sParticle* pAxleParticle43 = this->vec_pParticles[43];
	sParticle* pAxleParticle113 = this->vec_pParticles[113];
	sParticle* pAxleParticle167 = this->vec_pParticles[167];

	for (unsigned int index = 0; index != this->vec_pParticles.size(); index++)
	{
		sParticle* pCurrentParticle = this->vec_pParticles[index];
		// 
		// Skip the axle particles
		if (index == 43) { continue; }
		if (index == 113) { continue; }
		if (index == 167) { continue; }

		// Connect this particle to each of the centre axle particles:

		{	// To particle 43
			sParticle* pThisParticle = this->vec_pParticles[index];
			float distBetween = this->calcDistanceBetween(pThisParticle, pAxleParticle43);

			sConstraint* pBracingConstraint = new sConstraint();
			pBracingConstraint->pParticleA = pThisParticle;
			pBracingConstraint->pParticleB = pAxleParticle43;
			pBracingConstraint->restLength = distBetween;
			this->vec_pConstraints.push_back(pBracingConstraint);
		}
		
		{	// To particle 113
			sParticle* pThisParticle = this->vec_pParticles[index];
			float distBetween = this->calcDistanceBetween(pThisParticle, pAxleParticle113);

			sConstraint* pBracingConstraint = new sConstraint();
			pBracingConstraint->pParticleA = pThisParticle;
			pBracingConstraint->pParticleB = pAxleParticle113;
			pBracingConstraint->restLength = distBetween;
			this->vec_pConstraints.push_back(pBracingConstraint);
		}	
	
		{	// To particle 167
			sParticle* pThisParticle = this->vec_pParticles[index];
			float distBetween = this->calcDistanceBetween(pThisParticle, pAxleParticle167);

			sConstraint* pBracingConstraint = new sConstraint();
			pBracingConstraint->pParticleA = pThisParticle;
			pBracingConstraint->pParticleB = pAxleParticle167;
			pBracingConstraint->restLength = distBetween;
			this->vec_pConstraints.push_back(pBracingConstraint);
		}	
	}
	return;
}


void cSoftBodyVerlet::CreateRandomBracing(unsigned int numberOfBraces, float minDistanceBetweenVertices)
{
	CreateRandomBracing(numberOfBraces, minDistanceBetweenVertices, this->tightnessFactor, 0);
}



void cSoftBodyVerlet::CreateRandomBracing(unsigned int numberOfBraces,
										  float minDistanceBetweenVertices,
										  float constraintTightness, 
										  float addRestDist)
{
	for ( unsigned int count = 0; count != numberOfBraces; count++ )
	{
		// Assume the distance is OK
		bool bKeepLookingForParticles = false;

		do 
		{
			// Assume the distance is OK
			bKeepLookingForParticles = false;

			// Pick two random vertices
			// NOTE: Here, rand() might not be great because there's usually
			//	onlly about 32,000 possible integer values...
			// Meaning that if your are chosing from something LARGER than
			//	around 32,000, you'll miss a bunch of values. 
			// HOWEVER, you could also multiply rand() by itself
			unsigned int particleIndex1 = rand() % this->vec_pParticles.size();
			unsigned int particleIndex2 = rand() % this->vec_pParticles.size();

			sParticle* pParticle1 = this->vec_pParticles[particleIndex1];
			sParticle* pParticle2 = this->vec_pParticles[particleIndex2];
			float distBetween = this->calcDistanceBetween(pParticle1, pParticle2);

			// Distance OK?
			if ( distBetween < minDistanceBetweenVertices )
			{
				// No
				bKeepLookingForParticles = true;
			}
			else
			{
				// Distance is OK, so make a constraint
				sConstraint* pBracingConstraint = new sConstraint();
				pBracingConstraint->tightFact = constraintTightness;
				pBracingConstraint->pParticleA = pParticle1;
				pBracingConstraint->pParticleB = pParticle2;
				pBracingConstraint->restLength = this->calcDistanceBetween(pBracingConstraint->pParticleA, pBracingConstraint->pParticleB) + addRestDist;
			
				this->vec_pConstraints.push_back(pBracingConstraint);
			}

		} while (bKeepLookingForParticles);

	}

	return;
}

void cSoftBodyVerlet::BuildPlatform(void)
{
	std::vector<sParticle*> fourHighest; fourHighest.reserve(4);

	for (sParticle* currPart : vec_pParticles)
	{
		if (currPart->position.y > 0)
			fourHighest.push_back(currPart);
	}

	// Now we have our four
	for (sParticle* currPart : fourHighest)
	{
		this->AddRopeAttachment(currPart, 0.9f);
	}

}

void cSoftBodyVerlet::AddRopeAttachment(sParticle* partToRope, float tightness)
{
	const float ROPE_HEIGHT = 5.0f;
	// Here we create a new particle some length above this one, add a PULL attachment type to it
	sParticle* newRopePart = new sParticle();
	newRopePart->position = partToRope->position; newRopePart->position.y += ROPE_HEIGHT; newRopePart->old_position = newRopePart->position;
	newRopePart->isPartModel = false;
	newRopePart->isStatic = true;
	this->vec_pParticles.push_back(newRopePart);
	
	sConstraint* newConstraint = new sConstraint();
	newConstraint->pParticleA = newRopePart;
	newConstraint->pParticleB = partToRope;
	newConstraint->tightFact = tightness;
	newConstraint->restLength = ROPE_HEIGHT + 1.0f/*so the plat kinda drops*/; // Might want to change this to calcDistBetween() later
	newConstraint->relationType = sConstraint::PULL;
	this->vec_pConstraints.push_back(newConstraint);
}

void cSoftBodyVerlet::Jump(double& deltaTime)
{
	if (!canJump) return;


	sParticle* theHighestParticle = vec_pParticles[0];

	for (unsigned int i = 1; i < vec_pParticles.size(); i++)
	{
		if (vec_pParticles[i]->position.y > theHighestParticle->position.y)
		{
			theHighestParticle = vec_pParticles[i];
		}
	}
	const float JUMPFORCE = 0.5f;
	// Now we have the highest particle
	theHighestParticle->position.y += JUMPFORCE * static_cast<float>(deltaTime);

	for (sParticle* currPart : theHighestParticle->neighbours)
	{
		currPart->position.y += JUMPFORCE * static_cast<float>(deltaTime) * 0.5f;
		for (sParticle* currPart2 : currPart->neighbours)
		{
			currPart2->position.y += JUMPFORCE * static_cast<float>(deltaTime) * 0.2f;
		}
	}

}

void cSoftBodyVerlet::Move(glm::vec3& dir, double& deltaTime)
{
	sParticle* theFarthestParticle = vec_pParticles[0];
	glm::vec3 goal = this->getCentrePoint() + dir * 10.0f;
	float currDist = glm::distance(theFarthestParticle->position, goal);

	float newDist = 0.0f;

	for (unsigned int i = 1; i < vec_pParticles.size(); i++)
	{
		// Distance to goal dir ov current particle
		newDist = glm::distance(vec_pParticles[i]->position, goal);

		if (newDist < currDist) // If closer than one stored, store that one
		{
			theFarthestParticle = vec_pParticles[i];
			currDist = newDist;
		}
	}


	const float MOVEFORCE = 0.2f;
	// Now we have the highest particle
	theFarthestParticle->position += dir * MOVEFORCE * static_cast<float>(deltaTime);

	for (sParticle* currPart : theFarthestParticle->neighbours)
	{
		currPart->position += dir * MOVEFORCE * static_cast<float>(deltaTime) * 0.5f;
		for (sParticle* currPart2 : currPart->neighbours)
		{
			currPart2->position += dir * MOVEFORCE * static_cast<float>(deltaTime) * 0.2f;
		}
	}
}

