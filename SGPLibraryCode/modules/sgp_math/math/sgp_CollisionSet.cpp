

CollNode::CollNode()
{
	triangle = NULL;
	front = NULL;
	back = NULL;
}

CollNode::~CollNode()
{
	if (front != NULL) 
		delete front;
	front = NULL;
	if (back  != NULL) 
		delete back;
	back = NULL;

	CollisionTriangle *curr = triangle;
	while(curr != NULL)
	{
		CollisionTriangle *next = curr->next;
		delete curr;
		curr = next;
	}
}

#define DIFF_IN_PLANE 0.05f

void CollNode::build(SortedSet <CollisionTriangle *> &triangles, const uint32 cutWeight, const uint32 diffWeight, const uint32 coplanarWeight)
{

	// Try to find the "best" triangle plane to split along
	uint32 best = 0xFFFFFFFF;
	uint32 bestIndex = 0;

	for(int32 i = 0; i < triangles.size(); i++)
	{
		Vector3D norm;
		norm.Cross(triangles[i]->v[2] - triangles[i]->v[0], triangles[i]->v[1] - triangles[i]->v[0]);
		norm.Normalize();
		float off = -(norm * triangles[i]->v[0]);

		uint32 value = 0;
		int32 diff = 0;
		for(int32 j = 0; j < triangles.size(); j++)
		{
			uint32 pos = 0;
			uint32 neg = 0;

			for(uint32 k = 0; k < 3; k++)
			{
				float d = triangles[j]->v[k] * norm + off;
				if(std::fabs(d) > DIFF_IN_PLANE)
				{
					if(d > 0) 
						pos++;
					else 
						neg++;
				}
			}

			if(pos)
			{
				if(neg)
					value += cutWeight;
				else 
					diff += diffWeight;
			} 
			else 
			{
				if(neg) 
					diff -= diffWeight;
				else 
					value += coplanarWeight;
			}
		}
		value += std::abs(diff);

		if(value < best)
		{
			best = value;
			bestIndex = i;
		}
	}

	// The "best" plane
	normal.Cross(triangles[bestIndex]->v[2] - triangles[bestIndex]->v[0], triangles[bestIndex]->v[1] - triangles[bestIndex]->v[0]);
	normal.Normalize();
	offset = -(normal * triangles[bestIndex]->v[0]);

	SortedSet <CollisionTriangle *> frontTris;
	SortedSet <CollisionTriangle *> backTris;

	// Split into back and front list, and a list of triangles that are in the plane
	for(int32 j = 0; j < triangles.size(); j++)
	{
		float d[3];
		uint32 pos = 0;
		uint32 neg = 0;
		for(uint32 k = 0; k < 3; k++)
		{
			d[k] = triangles[j]->v[k] * normal + offset;
			if(std::fabs(d[k]) > DIFF_IN_PLANE)
			{
				if(d[k] > 0)
					pos++;
				else
					neg++;
			}
		}

		if(pos && neg)
		{ 
			// Plane cuts the triangle
			frontTris.add(triangles[j]);
			backTris.add(new CollisionTriangle(triangles[j]->v[0], triangles[j]->v[1], triangles[j]->v[2], triangles[j]->auxData));

		} 
		else if(pos)
		{ 
			// Front triangle
			frontTris.add(triangles[j]);
		} 
		else if(neg)
		{ 
			// Back triangle
			backTris.add(triangles[j]);
		} 
		else
		{ 
			// Triangle in the plane
			triangles[j]->next = triangle;
			triangle = triangles[j];
		}

	}

	if(frontTris.size() > 0)
	{
		front = new CollNode();
		front->build(frontTris, cutWeight, diffWeight, coplanarWeight);
	}

	if(backTris.size() > 0)
	{
		back = new CollNode();
		back->build(backTris, cutWeight, diffWeight, coplanarWeight);
	}
}

bool CollNode::intersect(const Vector3D &v0, const Vector3D &v1, Vector3D *point, bool solid, void **auxData) const
{
	float d0 = (normal * v0) + offset;
	float d1 = (normal * v1) + offset;

	if(d0 > 0)
	{
		if(front != NULL && front->intersect(v0, v1, point, solid, auxData)) 
			return true;
		if(d1 < 0)
		{
			if(isect(v0, v1, point, auxData)) 
				return true;
			if(back != NULL && back->intersect(v0, v1, point, solid, auxData))
				return true;
		}
	} 
	else 
	{
		if(back != NULL && back->intersect(v0, v1, point, solid, auxData)) 
			return true;
		if(d1 > 0)
		{
			if(!solid && isect(v0, v1, point, auxData))
				return true;
			if(front != NULL && front->intersect(v0, v1, point, solid, auxData))
				return true;
		}
	}

	return false;
}


bool CollNode::isect(const Vector3D &v0, const Vector3D &v1, Vector3D *point, void **auxData) const 
{
	CollisionTriangle *curr = triangle;

	float k = (normal * v0 + offset) / (normal * (v0 - v1));
	Vector3D p = v0 + (v1 - v0) * k;

	while(curr != NULL)
	{
		// Find interpolation constant
		uint32 prev = 2;
		bool result = true;
		for(uint32 i = 0; i < 3; i++)
		{
			Vector3D edgeNormal;
			edgeNormal.Cross(normal, curr->v[i] - curr->v[prev]);
			if( (edgeNormal * p) - (edgeNormal * curr->v[i]) < 0 )
			{
				result = false;
				break;
			}
			prev = i;
		}

		if(result)
		{
			if(point != NULL)
				*point = p;
			if(auxData != NULL)
				*auxData = curr->auxData;
			return true;
		}
		
		curr = curr->next;
	}
	return false;
}

bool CollNode::pushSphere(Vector3D &pos, const float radius) const 
{
	float d = (pos * normal) + offset;

	bool pushed = false;
	if(std::fabs(d) < radius)
	{
		CollisionTriangle *above = aboveTriangle(pos);

		if(above != NULL)
		{
			pos +=  normal * (radius - d); 
			pushed = true;
		}
	}

	if(front != NULL && d > -radius)
		pushed |= front->pushSphere(pos, radius);
	if(back  != NULL && d <  radius)
		pushed |= back ->pushSphere(pos, radius);

	return pushed;
}

CollisionTriangle *CollNode::aboveTriangle(const Vector3D &pos) const
{
	CollisionTriangle *curr = triangle;

	while(curr != NULL)
	{

		uint32 prev = 2;
		bool result = true;
		for(uint32 i = 0; i < 3; i++)
		{
			Vector3D edgeNormal;
			edgeNormal.Cross(normal, curr->v[i] - curr->v[prev]);
			if( (edgeNormal * pos) - (edgeNormal * curr->v[i]) < 0 )
			{
				result = false;
				break;
			}
			prev = i;
		}

		if(result)	
			return curr;

		curr = curr->next;
	}

	return NULL;
}

CollisionSet::CollisionSet()
{
	top = NULL;
}


CollisionSet::~CollisionSet()
{
	release();
}

void CollisionSet::release()
{
	if(top != NULL) 
		delete top;
	top = NULL;
}

void CollisionSet::build(const uint32 cutWeight, const uint32 diffWeight, const uint32 coplanarWeight)
{
	if(top != NULL) 
		delete top;

	if(triangles.size() > 0)
	{
		top = new CollNode();
		top->build(triangles, cutWeight, diffWeight, coplanarWeight);

		triangles.clear();
	}
}

bool CollisionSet::intersect(const Vector3D &v0, const Vector3D &v1, Vector3D *point, bool solid, void **auxData) const 
{
	if(top != NULL) 
		return top->intersect(v0, v1, point, solid, auxData);
	return false;
}



bool CollisionSet::pushSphere(Vector3D &pos, const float radius) const 
{
	if(top != NULL)
		return top->pushSphere(pos, radius);

	return false;
}
